#!/usr/bin/env python3.8
# pylint: disable=E1101, I1101
""" To deal with the Camera """

import sys
import threading
import time

import cv2
import numpy

import posix_ipc
from common.common import (CAM_CHANNEL, CAM_HEIGHT, CAM_WIDTH,
                           SHARED_MEMORY_NAME)
from common.shared_memory import SharedMemory

###############################################################################
#  Camera class


class Camera(threading.Thread):
    """ class for the camera """
    def __init__(self,
                 *,
                 width: int = CAM_WIDTH,
                 height: int = CAM_HEIGHT,
                 channel: int = CAM_CHANNEL) -> None:
        super().__init__()

        self.__cv = threading.Condition()
        self.__status = False
        self.__frame = None

        self.__shm = None
        self.__dimensions = (width, height, channel)
        self.__open()

    def __del__(self) -> None:
        self.thread_stop()

        with self.__cv:
            self.__cv.notifyAll()

    ###########################################################################
    # Public methods
    def run(self) -> None:
        """ do thread """
        while self.__status is True:
            width, height, channel = self.__dimensions
            read_buf = self.__shm.copy_from_mem(0)[0:width * height * channel]

            np_data = numpy.frombuffer(read_buf, dtype=numpy.uint8)
            np_data_3c = np_data.reshape(height, width, channel)

            _, self.__frame = cv2.imencode(".jpg", np_data_3c)

            with self.__cv:
                self.__cv.notifyAll()

    def read_frame(self) -> bytes:
        """ return the frame """
        with self.__cv:
            self.__cv.wait()

        if self.__frame.any() is False:
            return b'\x00'

        return self.__frame.tobytes()

    def thread_start(self) -> None:
        """ start the thread """
        self.__status = True
        self.start()

    def thread_stop(self) -> None:
        """ stop the thread """
        self.__status = False
        self.join()

    ###########################################################################
    # Internal methods
    def __check_camera(self) -> bool:
        return bool(self.__shm)

    def __open(self) -> bool:
        if self.__check_camera() is True:
            return False

        width, height, channel = self.__dimensions
        self.__shm = SharedMemory(name=SHARED_MEMORY_NAME,
                                  flags=posix_ipc.O_RDWR,
                                  mode=0o666,
                                  size=width * height * channel)

        return True


def camera_main() -> None:
    """ main """
    cam = Camera(width=640, height=360, channel=3)
    cam.thread_start()

    condition = True
    while condition:
        try:
            print("main,,")
            time.sleep(1)
        except KeyboardInterrupt:
            condition = False

    cam.thread_stop()


if __name__ == "__main__":
    sys.exit(camera_main() or 0)
