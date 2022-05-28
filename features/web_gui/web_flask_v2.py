# pylint: disable=c-extension-no-member, no-member
'''
This is python flask web page for patrol project
'''
import mmap
import sqlite3 as sql
import time

import cv2
import numpy as np
from flask import Flask, Response, render_template
from flask_bootstrap import Bootstrap
from flask_navigation import Navigation

import posix_ipc as pi

SHMEM = pi.SharedMemory('shm',
                        flags=pi.O_CREAT | pi.O_RDWR,
                        mode=0o666,
                        size=2764800)

APP = Flask(__name__)
APP.config['SECRET_KEY'] = '1234125'
BOOTSTRAP = Bootstrap(APP)
NAV = Navigation(APP)

NAV.Bar('top', [
    NAV.Item('Home', 'index'),
    NAV.Item('Accident Scene', 'accident'),
    NAV.Item('Defective lane', 'defective_lane'),
    NAV.Item('Over Speeding', 'over_speeding')
])


def gen_frames():
    '''
    get frames form cv2.VideoCapture
    '''
    while True:
        shared_buffer = mmap.mmap(SHMEM.fd, 2764800)
        byte_buffer = shared_buffer.read()

        np_buffer = np.frombuffer(byte_buffer, dtype=np.uint8)
        np_buffer_3c = np.reshape(np_buffer, (720, 1280, 3))

        ret, encode_buffer = cv2.imencode('.jpg', np_buffer_3c)
        time.sleep(0.2)
        if ret is False:
            break

        frame = encode_buffer.tobytes()
        yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"


@APP.route("/stream")
def stream():
    '''
    frames data streaming to web page
    '''
    return Response(gen_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


@APP.route('/')
def index():
    '''
    Start web page, information : project introduced
    '''
    return render_template('index.html')


@APP.route('/accident/')
def accident():
    '''
    accident page, query database
    '''
    con = sql.connect('accident.db')
    con.row_factory = sql.Row

    cur = con.cursor()
    cur.execute('select * from Testcases')

    rows = cur.fetchall()
    return render_template('accident.html', rows=rows)


@APP.route('/defective_lane/')
def defective_lane():
    '''
    defective_lane page, query database
    '''
    con = sql.connect('defective.db')
    con.row_factory = sql.Row

    cur = con.cursor()
    cur.execute('select * from Testcases')

    rows = cur.fetchall()
    return render_template('defective_lane.html', rows=rows)


@APP.route('/over_speeding/')
def over_speeding():
    '''
    over_speeding page, query database
    '''
    con = sql.connect('over_speeding.db')
    con.row_factory = sql.Row

    cur = con.cursor()
    cur.execute('select * from Testcases')

    rows = cur.fetchall()
    return render_template('over_speeding.html', rows=rows)


if __name__ == '__main__':
    APP.run(host='0.0.0.0', port=5000, debug=True)
