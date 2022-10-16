#!/usr/bin/env python3.8
""" for GUI using the fastapi """

import atexit

from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse, StreamingResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

from common.common import CAM_CHANNEL, CAM_HEIGHT, CAM_WIDTH
from utils.camera import Camera

###############################################################################
# for Camera

cam = Camera(width=CAM_WIDTH, height=CAM_HEIGHT, channel=CAM_CHANNEL)
cam.thread_start()


def cam_close() -> None:
    """ when exit the web, need to execute the close code """
    cam.thread_stop()


atexit.register(cam_close)

###############################################################################
# for fastapi

# Get FastAPI
app = FastAPI()

# mount the static directory
app.mount("/static", StaticFiles(directory="static"), name="static")

# set the templates
templates = Jinja2Templates(directory="templates")


@app.get("/", response_class=HTMLResponse)
async def main_page(request: Request):
    """ load the main page """
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/video")
async def get_video():
    """ get the video """
    return StreamingResponse(
        response_jpeg(),
        media_type="multipart/x-mixed-replace; boundary=frame")


def response_jpeg() -> bin:
    """ yield the jpeg frame """
    while True:
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + cam.read_frame() +
               b'\r\n')
