import cmd
import logging
from min_protocol import min_layer1
from min_protocol.min_layer2 import MinMessageDispatcher, MinMessageCommunicator

__author__ = 'marcus'
import argparse


logging.basicConfig(level=logging.DEBUG)

PING_MESSAGE = 0xf0
READ_PIN_MESSAGE = 0x10

# our callbacks
def ping_received(frame):
    time = min_layer1.min_decode(frame.payload[0:4])
    diff = min_layer1.min_decode(frame.payload[4:6])
    if not be_quiet:
        print "ping: %s after %s " % (time, diff)


class MinProtocolDemo(cmd.Cmd):
    def __init__(self):
        cmd.Cmd.__init__(self)

        self.communicator = MinMessageCommunicator(
            serial_port=serial_port,
            baud_rate=serial_baudrate,
            info_handlers={
                PING_MESSAGE: ping_received,
            }
        )

    def do_digital_read(self, pin):
        """digital_read [pin]
            Read the digital pin"""
        pin = int(pin)
        if 0 <= pin <= 18:
            payload = [pin]
            answer = self.communicator.ask_for_answer(
                frame_id=READ_PIN_MESSAGE,
                payload=payload
            )
            pin = int(answer.payload[0])
            pinmode = bool(answer.payload[1])
            if pinmode:
                print "Pin %i is HIGH" % pin
            else:
                print "Pin %i is LOW" \
                      "" % pin
        else:
            print "There is no pin %s" % pin

    def do_send_error(self, type=None):
        """send_error [type]
            Send one of various wrecked packages"""

        if not type:
            type = '0'

        error_frames = {
            # a wrong frame end
            '0': [170, 170, 170, 16, 1, 10, 60, 27, 87],
            # missing header
            '1': [170, 170, 16, 1, 10, 60, 27, 87],
            # a wrong length
            '2': [170, 170, 170, 16, 1, 60, 27, 87],
            # a wrong CRC
            '3': [170, 170, 170, 16, 1, 10, 60, 27, 87],
            # two missing header
            '4': [170, 16, 1, 10, 60, 27, 87],
        }
        error_frame = error_frames.get(type, None)
        if error_frame:
            self.communicator.serial_handler.send_queue.put(error_frame)
        else:
            print "There is no error frame ", type


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="MIN controller")

    parser.add_argument('-p', dest='port', type=str, help="Serial port, e.g. COM3", required=True)
    parser.add_argument('-b', '--baud', default=9600, type=int)
    parser.add_argument('-v', '--show_raw', action='store_true')
    parser.add_argument('-q', '--quiet', action='store_true')

    args = parser.parse_args()

    be_quiet = args.quiet

    if args.show_raw:
        min_layer1.SHOW_RAW_FOR_DEBUG = True

    serial_port = args.port
    serial_baudrate = args.baud

    demo = MinProtocolDemo()
    try:
        demo.cmdloop("Intro")
    except KeyboardInterrupt:
        print "\n\nthanks"

