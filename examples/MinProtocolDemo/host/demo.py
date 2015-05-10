import cmd
import logging
import min_protocol

__author__ = 'marcus'
import argparse


logging.basicConfig(level=logging.DEBUG)

PING_MESSAGE = 0xf0
READ_PIN_MESSAGE = 0x10

# our callbacks
def ping_received(message_id, payload):
    time = min_protocol.min_decode(payload[0:4])
    diff = min_protocol.min_decode(payload[4:6])
    if not be_quiet:
        print "ping: %s after %s " % (time, diff)


def digital_read_received(message_id, payload):
    pin = int(payload[0])
    pinmode = bool(payload[1])
    if pinmode:
        print "Pin %i is HIGH" % pin
    else:
        print "Pin %i is LOW" \
              "" % pin


class MinProtocolDemo(cmd.Cmd):
    def do_digital_read(self, pin):
        """digital_read [pin]
            Read the digital pin"""
        pin = int(pin)
        if 0 <= pin <= 18:
            payload = [pin]
            frame = min_protocol.Frame(serial_handler=controller,
                                       frame_id=READ_PIN_MESSAGE,
                                       payload=payload)
            frame.transmit()
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
            #a wrong length
            '2': [170, 170, 170, 16, 1, 60, 27, 87],
            #a wrong CRC
            '3': [170, 170, 170, 16, 1, 10, 60, 27, 87],
            # two missing header
            '4': [170, 16, 1, 10, 60, 27, 87],
        }
        error_frame = error_frames.get(type, None)
        if error_frame:
            controller.send_queue.put(error_frame)
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
        min_protocol.SHOW_RAW_FOR_DEBUG = True

    message_dispatcher = min_protocol.MinMessageDispatcher(message_callbacks={
        READ_PIN_MESSAGE: digital_read_received,
        PING_MESSAGE: ping_received
    })

    controller = min_protocol.SerialHandler(port=args.port, baudrate=args.baud,
                                            received_frame_handler=message_dispatcher.received_frame)

    demo = MinProtocolDemo()
    try:
        demo.cmdloop("Intro")
    except KeyboardInterrupt:
        print "\n\nthanks"

