import cmd
import logging
from min_protocol import min_layer1
from min_protocol.min_layer2 import MinMessageDispatcher, MinMessageCommunicator

__author__ = 'marcus'
import argparse


logging.basicConfig(level=logging.DEBUG)

PIN_MODE_MESSAGE = 0x10
READ_PIN_MESSAGE = 0x11
WRITE_PIN_MESSAGE = 0x12
READ_PIN_ANALOG_MESSAGE = 0x13
WRITE_PIN_ANALOG_MESSAGE = 0x14

PING_MESSAGE = 0xf0

# our callbacks
def ping_received(frame):
    time = min_layer1.min_decode_int(frame.payload[0:4])
    diff = min_layer1.min_decode_int(frame.payload[4:6])
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

    def do_pin_mode(self, args=None):
        """do_pin_mode [pin],[mode]
            set the output mode of the pin. Output mode can be OUTPUT, O, PULL_UP, P, INPUT, I"""
        if args is None or args is '':
            print "No arguments given"
            return
        arg_parts = args.split(',')
        if not len(arg_parts) == 2:
            print "cannot decode arguments ", args
            return
        pin = arg_parts[0]
        mode = arg_parts[1]
        mode = mode.upper()
        if mode in ('OUTPUT', 'OUT', 'O'):
            output_code = 1
        elif mode in ('INPUT', 'IN', 'I'):
            output_code = 0
        elif mode in ('PULL_UP', 'PU', 'P'):
            output_code = -1
        else:
            print "there is not output mode ", mode
            return
        pin = int(pin)
        if 0 <= pin <= 18:
            payload = [pin, output_code]
            answer = self.communicator.ask_for_answer(
                frame_id=PIN_MODE_MESSAGE,
                payload=payload
            )
            pin = int(answer.payload[0])
            pin_mode = bool(answer.payload[1])
            if pin_mode > 0:
                print "Pin %i is OUTPUT" % pin
            elif pin_mode < 0:
                print "Pin %i is INPUT with PULL UP" % pin
            else:
                # must be zero
                print "Pin %i is INPUT" % pin
        else:
            print "There is no pin %s" % pin

    def do_digital_read(self, pin=None):
        """digital_read [pin]
            Read the digital pin"""
        if pin is None or pin is '':
            print "No pin given"
            return
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
                print "Pin %i is LOW" % pin
        else:
            print "There is no pin %s" % pin

    def do_digital_write(self, args=None):
        """do_digital_write [pin],[mode]
            set the output value of the pin."""
        if args is None or args is '':
            print "No arguments given"
            return
        arg_parts = args.split(',')
        if not len(arg_parts) == 2:
            print "cannot decode arguments ", args
            return
        pin = arg_parts[0]
        value = arg_parts[1]
        value = value.upper()
        pin = int(pin)
        if value in ('1', 'HIGH', 'TRUE', 'ON'):
            value = 1
        else:
            value = 0
        if 0 <= pin <= 18:
            payload = [pin, value]
            answer = self.communicator.ask_for_answer(
                frame_id=WRITE_PIN_MESSAGE,
                payload=payload
            )
            pin = int(answer.payload[0])
            pin_value = bool(answer.payload[1])
            if pin_value:
                print "Pin %i is HIGH" % pin
            else:
                # must be zero
                print "Pin %i is LOW" % pin

    def do_analog_read(self, pin=None):
        """analog_read [pin]
            Read the analog pin"""
        if pin is None or pin is '':
            print "No pin given"
            return
        pin = int(pin)
        if 0 <= pin <= 5:
            payload = [pin]
            answer = self.communicator.ask_for_answer(
                frame_id=READ_PIN_ANALOG_MESSAGE,
                payload=payload
            )
            pin = int(answer.payload[0])
            pin_value_float = min_layer1.min_decode_float(answer.payload[1:5])
            pin_value_int = min_layer1.min_decode_int(answer.payload[5:7])
            print "Pin %i is at %f - roughly %f" % (pin, pin_value_float, pin_value_int / 100.0)

    def do_analog_write(self, args=None):
        """analog read [pin],[value]
            Write the analog pin (0.0 to 1.1)"""
        if args is None or args is '':
            print "No arguments given"
            return
        arg_parts = args.split(',')
        if not len(arg_parts) == 2:
            print "cannot decode arguments ", args
            return
        pin = arg_parts[0]
        if pin is None or pin is '':
            print "No pin given"
            return
        pin = int(pin)
        value = arg_parts[1]
        value = float(value)
        if value < 0:
            print "%s is below 0 - cut to 0.0" % value
            value = 0.
        if value > 1:
            print "%s is beyond 1 - cut to 1.0" % value
            value = 1.
        if 0 <= pin <= 5:
            payload = [pin] + min_layer1.min_encode_float(value)
            answer = self.communicator.ask_for_answer(
                frame_id=WRITE_PIN_ANALOG_MESSAGE,
                payload=payload
            )
            pin = int(answer.payload[0])
            pin_value_float = min_layer1.min_decode_float(answer.payload[1:5])
            pin_output_value = min_layer1.min_decode_int(answer.payload[5:7])
            print "Pin %i is at %f (%i)" % (pin, pin_value_float, pin_output_value)


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

