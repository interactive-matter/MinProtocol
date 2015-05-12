import argparse
from random import random
from threading import Thread
from time import sleep
from demo import ping_received, PING_MESSAGE, READ_PIN_MESSAGE, READ_PIN_ANALOG_MESSAGE
from min_protocol import min_layer1
from min_protocol.min_layer1 import SerialHandler, SHOW_RAW_FOR_DEBUG
from min_protocol.min_layer2 import MinMessageCommunicator, MinMessageCommunicationError

__author__ = 'marcus'


class ErrornousSerialHandler(SerialHandler):
    def __init__(self, port, baudrate, received_frame_handler, error_rate):
        SerialHandler.__init__(self, port, baudrate, received_frame_handler)
        self.error_rate = error_rate

    def receiver(self):
        """
        Receive loop that takes a byte at a time from the serial port and creates a frame
        """
        while True:
            # Read a byte from the serial line (blocking call)
            data = self.serial.read(size=1)
            rx_byte = ord(data[0])
            if random() < self.error_rate:
                # we induce an error
                if random() < 0.5:
                    # we drop the byte
                    # and do nothing
                    print "Dropping RX on wire" + '0x{:02x}'.format(rx_byte)
                else:
                    # introdcue a new random byte
                    new_rx_byte = int(256.0 * random())
                    print "Replacing RX on wire" + '0x{:02x}'.format(rx_byte) + " with " + '0x{:02x}'.format(
                        new_rx_byte)
                    rx_byte = new_rx_byte
                    self.build_received_frame(rx_byte)
            else:
                print("Data RX on wire: " + '0x{:02x}'.format(rx_byte))
                self.build_received_frame(rx_byte)

    def sender(self):
        """
        Feed the queue into the serial port (blocking on reading the queue and the sending)
        """
        while True:
            frame_data = self.send_queue.get()
            new_data = []
            for data in frame_data:
                if random() < self.error_rate:
                    # we induce an error
                    if random() < 0.5:
                        # we drop the byte
                        # and do nothing
                        print "Dropping TX on wire" + '0x{:02x}'.format(data)
                    else:
                        # introdcue a new random byte
                        new_tx_byte = int(256.0 * random())
                        print "Replacing TX on wire" + '0x{:02x}'.format(data) + " with " + '0x{:02x}'.format(
                            new_tx_byte)
                        new_data.append(new_tx_byte)
                else:
                    new_data.append(data)
            print("Data TX on wire: %s" % ':'.join('0x{:02x}'.format(i) for i in frame_data))
            self.serial.write(data=frame_data)


def stress_test(serial_port, serial_baudrate, error_rate, nr_of_threads, seconds):
    # construct our own erro communicator
    errornous_handler = ErrornousSerialHandler(
        port=serial_port, baudrate=serial_baudrate,
        received_frame_handler=None,
        error_rate=error_rate
    )
    errornous_communicator = MinMessageCommunicator(
        serial_port=None, baud_rate=None,
        serial_handler=errornous_handler,
        info_handlers={
            PING_MESSAGE: ping_received,
        }
    )

    test_running = True

    def run_test():
        while test_running:
            # read an digital pin
            payload = [7]
            print "asking for digital pin"
            try:
                answer = errornous_communicator.ask_for_answer(
                    frame_id=READ_PIN_MESSAGE,
                    payload=payload
                )
                pin = int(answer.payload[0])
                pinmode = bool(answer.payload[1])
                if pinmode:
                    print "Pin %i is HIGH" % pin
                else:
                    print "Pin %i is LOW" % pin
            except MinMessageCommunicationError:
                print "failed to read digital pin"
            # do an analog read
            payload = [2]
            print "asking for anlogue pin"
            try:
                answer = errornous_communicator.ask_for_answer(
                    frame_id=READ_PIN_ANALOG_MESSAGE,
                    payload=payload
                )
                pin = int(answer.payload[0])
                pin_value_float = min_layer1.min_decode_float(answer.payload[1:5])
                pin_value_int = min_layer1.min_decode_int(answer.payload[5:7])
                print "Pin %i is at %f - roughly %f" % (pin, pin_value_float, pin_value_int / 100.0)
            except MinMessageCommunicationError:
                print "failed to read analog pin"

    threads = []
    for i in range(nr_of_threads):
        threads.append(Thread(target=run_test))

    print "running error test with error rate %0.2f with %i threads for %i seconds" % (
        error_rate, nr_of_threads, seconds)

    for runner_thread in threads:
        runner_thread.start()
    # the threads will do it's work and hence we can relax here
    sleep(seconds)

    test_running = False

    for runner_thread in threads:
        runner_thread.join()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="MIN controller")

    parser.add_argument('-p', dest='port', type=str, help="Serial port, e.g. COM3", required=True)
    parser.add_argument('-b', '--baud', default=9600, type=int)
    parser.add_argument('-e', '--errorrate', default=0.1, type=float)
    parser.add_argument('-t', '--threads', default=2, type=int)
    parser.add_argument('-s', '--seconds', default=120, type=int)
    parser.add_argument('-v', '--show_raw', action='store_true')

    args = parser.parse_args()

    if args.show_raw:
        min_layer1.SHOW_RAW_FOR_DEBUG = True

    serial_port = args.port
    serial_baudrate = args.baud
    error_rate = args.errorrate
    threads = args.threads
    seconds = args.seconds

    stress_test(
        serial_port=serial_port, serial_baudrate=serial_baudrate,
        error_rate=error_rate, nr_of_threads=threads, seconds=seconds
    )

