#!/usr/bin/env python3
# pylint: disable=I1101
""" for Semaphore """

import sys

import posix_ipc

from .common import BINARY_SEMAPHORE_MODE, BINARY_SEMAPHORE_NAME, is_root

###############################################################################
#  Semaphore class


class Semaphore:
    """ class for the custom semaphore """
    def __init__(self, *, name: str, flags: int, mode: int, init: int) -> None:
        self.__semaphore = None

        if is_root() is False:
            return

        if self.__open(name, flags, mode, init) is False:
            return

    def __del__(self) -> None:
        self.__close()

    ###########################################################################
    # public methods
    def lock(self) -> None:
        """ lock """
        if self.__check_instance() is False:
            return

        self.__semaphore.acquired()

    def unlock(self) -> None:
        """ unlock """
        if self.__check_instance() is False:
            return

        self.__semaphore.release()

    ###########################################################################
    # Internal methods
    def __check_instance(self) -> bool:
        return bool(self.__semaphore)

    def __open(self, name: str, flags: int, mode: int, init: int) -> bool:
        if self.__check_instance() is False:
            return False

        try:
            self.__semaphore = posix_ipc.Semaphore(name=name,
                                                   flags=flags,
                                                   mode=mode,
                                                   initial_value=init)

            return True
        except posix_ipc.ExistentialError:
            self.__semaphore = None
            return False

    def __close(self) -> bool:
        if self.__check_instance() is False:
            return False

        self.__semaphore.close()
        return True


def semaphore_main() -> None:
    """ main """
    binary_semaphore = Semaphore(name=BINARY_SEMAPHORE_NAME,
                                 flags=posix_ipc.O_RDWR,
                                 mode=0o666,
                                 init=BINARY_SEMAPHORE_MODE)
    print(binary_semaphore)


if __name__ == "__main__":
    sys.exit(semaphore_main() or 0)
