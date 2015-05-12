from Queue import Queue, Empty, Full
import logging
from threading import Lock
from min_protocol.min_layer1 import FRAME_DROPPED_PACKAGE_ID, DEFAULT_MESSAGE_ANSWER_RESPONSE_TIMEOUT, Frame, \
    FRAME_INFO_PACKAGE_PATTERN, FRAME_ERROR_PACKAGE_PATTERN, MAX_QUEUE_PUT_WAIT_TIME, SerialHandler

__author__ = 'marcus'

COMMUNICATION_LOCK = Lock()

COMMUNICATION_ERROR_FRAME = FRAME_ERROR_PACKAGE_PATTERN + 0x2

LOGGER = logging.getLogger(__name__)

class MinMessageDispatcher(object):
    def __init__(self, message_callbacks=None):
        # an callback int message_id -> callback for the payload
        if not message_callbacks:
            self.message_callbacks = {}
        else:
            self.message_callbacks = message_callbacks

    def received_frame(self, frame):
        message_id = frame.get_id()
        if message_id == FRAME_DROPPED_PACKAGE_ID:
            LOGGER.warn("Frame dropped")
        else:
            callback = self.message_callbacks.get(message_id, None)
            if callback:
                data = frame.get_payload()
                try:
                    callback(message_id, data)
                except Exception as e:
                    LOGGER.error("Callback for %s threw exception: %s", message_id, e, exc_info=True)
            else:
                LOGGER.warn("Ignoring unknown message id %s", message_id)


class MinMessageCommunicationError(Exception):
    def __init__(self, request_frame, answer_frame=None):
        self.request_frame = request_frame
        self.answer_frame = answer_frame

    def __str__(self):
        error = 'MinMessageCommunicationError: received ', self.answer_frame
        if self.request_frame:
            error = error, "for ", self.request_frame
        return error


class MinMessageCommunicator(object):
    def __init__(self, serial_port, baud_rate, info_handlers=None, response_timeout=None, serial_handler=None):
        if serial_handler:
            self.serial_handler = serial_handler
            serial_handler.received_frame_handler = self.received_frame
            serial_handler.error_handler = self.dropped_frame

        else:
            self.serial_handler = SerialHandler(port=serial_port, baudrate=baud_rate,
                                                received_frame_handler=self.received_frame,
                                                error_handler=self.dropped_frame)

        self.response_queue = Queue()
        self.waiting_for_answer=False

        if not response_timeout:
            self.response_timeout = DEFAULT_MESSAGE_ANSWER_RESPONSE_TIMEOUT
        else:
            self.response_timeout = response_timeout
        if not info_handlers:
            self.info_handlers = {}
        else:
            self.info_handlers = dict(info_handlers)

    def ask_for_answer(self, frame_id, payload, response_timeout=None):
        frame = Frame(
            serial_handler=self.serial_handler,
            frame_id=frame_id,
            payload=payload
        )
        if not response_timeout:
            response_timeout = self.response_timeout
        # todo do we have to empty the queue???
        with COMMUNICATION_LOCK:
            self.waiting_for_answer=True
            frame.transmit()
            try:
                answer = self.response_queue.get(block=True, timeout=response_timeout)
                answer_id = answer.frame_id
                if answer_id != frame_id \
                        or answer_id >= FRAME_ERROR_PACKAGE_PATTERN:
                    raise MinMessageCommunicationError(frame, answer)
                else:
                    return answer
            except Empty:
                # we got no answer?!
                raise MinMessageCommunicationError(frame)
            finally:
                self.waiting_for_answer=False

    def stop(self):
        self.serial_handler.stop()

    def received_frame(self, frame):
        try:
            frame_id = frame.frame_id
            if FRAME_INFO_PACKAGE_PATTERN <= frame_id < FRAME_ERROR_PACKAGE_PATTERN:
                info_handler = self.info_handlers.get(frame_id, None)
                if info_handler:
                    info_handler(frame)
                else:
                    LOGGER.warn("No info handler for info id %s", frame_id)
            else:
                self.response_queue.put(frame, block=True, timeout=MAX_QUEUE_PUT_WAIT_TIME)
        except Full:
            LOGGER.error("Unable to put an frame %s on the response queue since it is full", frame)

    def dropped_frame(self, frame):
        if self.waiting_for_answer:
            error_frame = Frame(
                serial_handler=None,
                frame_id=COMMUNICATION_ERROR_FRAME
            )
            self.response_queue.put(error_frame)
        LOGGER.error("Frame dropped")