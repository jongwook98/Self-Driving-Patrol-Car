'''
This python program is creates database
'''
import sqlite3

CON = sqlite3.connect('database_name.db')
print('Database creation successful')

CON.execute("create table Testcases ("
            "case_ integer, "
            "image_path text, "
            "information text, "
            "datetime text"
            ")")
print('Table creation successful')
CON.close()
