import cmd
import min_protocol

__author__ = 'marcus'
import argparse

PING_MESSAGE = 255

# Called when a MIN frame has been received successfully from the serial line
def received_frame(frame):
    message_id = frame.get_id()
    data = frame.get_payload()

    if args.quiet:
        pass
    if min_protocol.SHOW_RAW_FOR_DEBUG:
        if message_id == 0x0e:  # Deadbeef message
            print("RX deadbeef: " + ':'.join('{:02x}'.format(i) for i in data))
        elif message_id == 0x23:  # Environment message
            temperature = -20.0 + (min_protocol.min_decode(data[0:2]) * 0.0625)
            humidity = min_protocol.min_decode(data[2:4]) * 0.64
            print("Environment: temperature={0}C, humidity={1}%".format(temperature, humidity))
        elif message_id == 0x24:  # Motor status message
            status = data[0]
            position = min_protocol.min_decode(data[1:5])
            print("Motor: status={}, position={}".format(status, position))
        elif message_id == 0x02:
            print("Ping received: " + ':'.join('{:02x}'.format(i) for i in data))


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

