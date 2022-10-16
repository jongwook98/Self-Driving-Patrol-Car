#!/usr/bin/env python3
# pylint: disable=I1101
""" for shared memory """

import mmap
import os
import sys

import posix_ipc

from .common import (BINARY_SEMAPHORE_MODE, BINARY_SEMAPHORE_NAME,
                     SHARED_MEMORY_NAME, is_root)
from .semaphore import Semaphore

###############################################################################
#  SharedMemory class


class SharedMemory:
    """ custom the shared memory """
    def __init__(self, *, name: str, flags: int, mode: int, size: int) -> None:
        self.__shm = None
        self.__mmap = None
        self.__bsp = None

        if is_root() is False:
            return

        if self.__open(name, flags, mode, size) is False:
            return

    def __del__(self) -> None:
        self.__close()

    ###########################################################################
    # public methods
    def copy_to_mem(self, write_buf: bin, offset: int) -> bool:
        """ set data to the shared memory """
        if self.__check_instance() is False:
            return False

        self.__bsp.lock()
        self.__mmap.seek(offset, os.SEEK_SET)
        self.__mmap.write(write_buf)
        self.__bsp.unlock()

        return True

    def copy_from_mem(self, offset: int) -> bin:
        """ get data from the shared memory """
        if self.__check_instance() is False:
            return 0b0

        self.__bsp.lock()
        self.__mmap.seek(offset, os.SEEK_SET)
        read_buf = self.__mmap.read()
        self.__bsp.unlock()

        return read_buf

    ###########################################################################
    # Internal methods
    def __check_instance(self) -> bool:
        return bool(self.__shm) and bool(self.__mmap) and bool(self.__bsp)

    def __align_offset(self, mem_size: int) -> int:
        page_size = os.sysconf("SC_PAGE_SIZE")
        return (mem_size + page_size) & ~(page_size - 1)

    def __open(self, name: str, flags: int, mode: int, size: int) -> bool:
        if self.__check_instance() is True:
            return False

        try:
            req_size = self.__align_offset(size)

            self.__shm = posix_ipc.SharedMemory(name=name,
                                                flags=flags,
                                                mode=mode,
                                                size=req_size)

            self.__mmap = mmap.mmap(self.__shm.fd, req_size)

            self.__bsp = Semaphore(name=BINARY_SEMAPHORE_NAME,
                                   flags=flags,
                                   mode=mode,
                                   init=BINARY_SEMAPHORE_MODE)
            return True
        except posix_ipc.ExistentialError:
            self.__shm = None
            self.__mmap = None
            return False

    def __close(self) -> bool:
        if self.__check_instance() is False:
            return False

        self.__mmap.close()
        self.__shm.close_fd()
        return True


def shared_memory_main() -> None:
    """ main """
    shm = SharedMemory(name=SHARED_MEMORY_NAME,
                       flags=posix_ipc.O_RDWR,
                       mode=0o666,
                       size=1280 * 720 * 3)

    print(shm)


if __name__ == "__main__":
    sys.exit(shared_memory_main() or 0)
