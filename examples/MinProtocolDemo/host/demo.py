import cmd
import min_protocol

__author__ = 'marcus'
import argparse

PING_MESSAGE = 255

# our callbacks
def ping_received(message_id, payload):
    time = min_protocol.min_decode(payload[0:4])
    diff = min_protocol.min_decode(payload[4:6])
    print "ping: %s after %s " % (time, diff)


class MinProtocolDemo(cmd.Cmd):
    def do_digital_read(self, pin):
        """digital_read [pin]
            Read the digital pin"""
        print "not implemented for", pin
        # payload = min_encode_32(1000000) + min_encode_16(5000)
        # f = Frame(controller, frame_id=0x36, payload=payload)
        # f.transmit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="MIN controller")

    parser.add_argument('-p', dest='port', type=str, help="Serial port, e.g. COM3", required=True)
    parser.add_argument('-b', '--baud', default=9600, type=int)
    parser.add_argument('-v', '--show_raw', action='store_true')
    parser.add_argument('-q', '--quiet', action='store_true')

    args = parser.parse_args()

    if args.show_raw:
        min_protocol.SHOW_RAW_FOR_DEBUG = True

    message_dispatcher = min_protocol.MinMessageDispatcher(message_callbacks={
        PING_MESSAGE: ping_received
    })

    controller = min_protocol.SerialHandler(port=args.port, baudrate=args.baud,
                                            received_frame_handler=message_dispatcher.received_frame)

    demo = MinProtocolDemo()
    demo.cmdloop("Intro")

