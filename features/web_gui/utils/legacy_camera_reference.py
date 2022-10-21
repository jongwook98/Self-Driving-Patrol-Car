#!/usr/bin/env python3.8
# pylint: disable=E1101, I1101
""" To deal with the Camera """

import sys
import threading
import time

import cv2

import posix_ipc
from common.common import SHARED_MEMORY_NAME
from common.shared_memory import SharedMemory

###############################################################################
#  Camera class


class Camera(threading.Thread):
    """ class for the camera """

    CAMERA_CAHNNEL = 3

    def __init__(self, index: int) -> None:
        super().__init__()

        self.__cv = threading.Condition()
        self.__status = False
        self.__frame = None

        self.__cam = self.__shm = None
        self.__dimensions = (0, 0, 0)
        self.__open(index)

    def __del__(self) -> None:
        self.thread_stop()

        with self.__cv:
            self.__cv.notifyAll()

        self.__close()

    ###########################################################################
    # Public methods
    def run(self) -> None:
        """ do thread """
        while self.__status is True:
            _, self.__frame = self.__cam.read()
            if self.__frame.any():
                if self.__shm.copy_to_mem(self.__frame.tobytes(), 0) is True:
                    cv2.imshow("Video1", self.__frame)

            with self.__cv:
                self.__cv.notifyAll()

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

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

    @property
    def dimensions(self) -> tuple:
        """ camera dimensions getter """
        return self.__dimensions

    ###########################################################################
    # Internal methods
    def __check_camera(self) -> bool:
        return bool(self.__cam) and bool(self.__cam.isOpened())

    def __open(self, index: int) -> bool:
        self.__cam = cv2.VideoCapture(index)
        if self.__check_camera() is False:
            return False

        width = int(self.__cam.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(self.__cam.get(cv2.CAP_PROP_FRAME_HEIGHT))
        channel = self.__cam.get(cv2.CAP_PROP_CHANNEL)
        if channel == -1:
            channel = 3

        self.__dimensions = (width, height, channel)
        self.__shm = SharedMemory(name=SHARED_MEMORY_NAME,
                                  flags=posix_ipc.O_CREAT | posix_ipc.O_RDWR,
                                  mode=0o666,
                                  size=width * height * channel)

        return True

    def __close(self) -> bool:
        if self.__check_camera() is False:
            return False

        self.__cam.release()
        cv2.destroyAllWindows()
        return True


def camera_main() -> None:
    """ main """
    cam = Camera(0)
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
