'''
This python program is deletes last database infomation
'''
import os
import sqlite3 as sql

DB = ['accident.db', 'defective.db', 'over_speeding.db']
DB_PATH = ['accident', 'defective_lane', 'over_speeding']
CASE = 2

CON = sql.connect(DB[CASE])
CURSOR = CON.cursor()

CURSOR.execute("DELETE FROM Testcases WHERE case_ = 1")
CURSOR.execute('SELECT * From Testcases')

ROWS = CURSOR.execute('SELECT COUNT(*) FROM Testcases').fetchone()

DB_NAME = DB_PATH[CASE]
DB_DELETE_NUM = 1

PATH = f"static/img/{DB_NAME}/{DB_NAME}_{DB_DELETE_NUM}.jpg"

if os.path.isfile(PATH):
    os.remove(PATH)

for ROW in range(ROWS[0]):
    DB_NUM = ROW + 1
    DB_OLD_NUM = ROW + 2
    PATH = "static/img/{DB_NAME}/{DB_NAME}_{DB_NUM}.jpg"
    OLDPATH = "static/img/{DB_NAME}/{DB_NAME}_{DB_OLD_NUM}.jpg"

    S_PA = '/' + PATH
    S_OLD = '/' + OLDPATH

    CURSOR.execute(
        f"UPDATE Testcases SET case_ = {DB_NUM} WHERE case_ = {DB_OLD_NUM}")
    CURSOR.execute(
        f"UPDATE Testcases SET image_path = {S_PA} WHERE image_path = {S_OLD}")

    if os.path.isfile(OLDPATH):
        os.rename(OLDPATH, PATH)

CON.commit()
CON.close()
