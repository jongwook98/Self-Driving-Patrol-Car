#!/usr/bin/env python3
""" common method """

import os

SHARED_MEMORY_NAME = "Shared_Memory_for_Camera_Info"

BINARY_SEMAPHORE_MODE = 1
BINARY_SEMAPHORE_NAME = "Binary_Semaphore_for_Getting_Camera_Info"

CAM_WIDTH = 640
CAM_HEIGHT = 360
CAM_CHANNEL = 3


def is_root() -> bool:
    """ check the root user """
    user = bool(os.getuid() == 0)
    if user is False:
        print("Need to execute as the root user!")

    return user
