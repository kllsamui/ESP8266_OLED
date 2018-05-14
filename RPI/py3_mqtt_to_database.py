#!/usr/bin/env python3
"""filename: py3_mqtt_to_database.py
try to catch measurement data from esp8266 mqtt client ( the mqtt broker is here local host)
and save to a database
"""
import paho.mqtt.subscribe as subscribe
import json
import os
from sqlite3 import dbapi2 as sqlite3
import time, datetime

mqtt_broker="127.0.0.1"
mqtt_topic="pcu213/sensor/out"

# configuration
# for DB see /static/data/
DATABASE = 'myMQTT.db'
DB_exists = False
TABLE_NAME = mqtt_topic.replace("/","_")		# SQL not take the mqtt topic structure
TB_exists = False


APP_ROOT = os.path.dirname(os.path.abspath(__file__))   # refers to application_top
DiagP = False	#True

#_________________________________________________________SQL tools
def check_DB_file():
	"""returns if path and file exists"""
	db_file = os.path.join("static","data",DATABASE)
	if (os.path.isfile(db_file)):
		return True
	else:
		return False

DB_exists = check_DB_file()
if (DiagP): print("have db file: ",DB_exists)

def get_db():
	"""Opens a new database connection if there is none yet for the
	current application context.
	"""
	db_file = os.path.join("static","data",DATABASE)
	if (os.path.isfile(db_file)):
		if (DiagP): print("found db")
	else:
		print("first run? not find db file %s" %(db_file)) 

	sqlite_db = sqlite3.connect(db_file)
	sqlite_db.row_factory = sqlite3.Row

	return sqlite_db

def check_TB_exists(mytable):
	haveTB = False
	db = get_db()
	with db:
		cur = db.cursor()
		try:
			sqlstr = "SELECT * FROM "
			sqlstr += mytable
			if (DiagP): print(sqlstr)
			cur.execute(sqlstr)
			return True
		except sqlite3.Error as e:
			return False

TB_exists = check_TB_exists(TABLE_NAME)
if (DiagP): print("have table: ",TB_exists)

def make_table_in_db(mytable,mycollist):
	""" make new table in db
	"""
	if (DiagP): print("table: %s collist: %s" % (mytable,mycollist))
	db = get_db()
	with db:
		cur = db.cursor()
		sqlstr = "CREATE TABLE IF NOT EXISTS "
		sqlstr += mytable
#		sqlstr += " (Id INTEGER PRIMARY KEY AUTOINCREMENT, Date DATETIME DEFAULT current_timestamp"
		sqlstr += " (Id INTEGER PRIMARY KEY AUTOINCREMENT, Date DATE DEFAULT (datetime('now','localtime'))"
		for cols in mycollist:
			sqlstr += ", "
			sqlstr += cols
			sqlstr += " REAL"
		sqlstr += " );"
		if (DiagP): print(sqlstr)
		cur.execute(sqlstr)

def store_signal_in_db(mytable,mycollist,myvallist):
	""" make new entry with new mqtt data
	"""
	if (DiagP): print("table: %s collist: %s vallist: %s" % (mytable,mycollist,myvallist))
	db = get_db()
	with db:
		cur = db.cursor()
		ccount=len(mycollist)
		sqlstr = "INSERT INTO "
		sqlstr += mytable
		sqlstr += " ("
		for cols in range(ccount):
			if (cols > 0):
				sqlstr += ","
			sqlstr += mycollist[cols]
		sqlstr += " ) VALUES ("
		for cols in range(ccount):
			if (cols > 0):
				sqlstr += ","
			sqlstr += str(myvallist[cols])
		sqlstr += " );"
		if (DiagP): print(sqlstr)
		cur.execute(sqlstr)



def on_message_do(client, userdata, message):
	#       print("client: %s userdata: %s" % (client, userdata))	#	shows: client: <paho.mqtt.client.Client object at 0x76a0c910> userdata: None
	signaltext = message.payload.decode("utf-8")			# bytearray to string
	thistime = datetime.datetime.now()
	stime = '{0:%Y-%m-%d %H:%M:%S}'.format(thistime)		# time only for python show, sql make own
	print("time: %s, topic: %s, messagestr: %s" % (stime, message.topic, signaltext))			#	shows: pcu213/sensor/out '{"SIN":16.39}'

	signalvars = json.loads(signaltext)				#	print(signalvars)					#	shows: {'SIN': 15.19}
	# 								check out JSON convert to lists
	columnsnames = list(signalvars.keys())				#	print(columnsnames)
	columnsvals  = list(signalvars.values())			#	print(columnsvals)
	columns_count = len(columnsnames)
	if (DiagP): print("columns_count: ",columns_count)

	if (TB_exists == False): make_table_in_db(TABLE_NAME,columnsnames)
	store_signal_in_db(TABLE_NAME,columnsnames,columnsvals)


#_________________________________________________________MAIN
if __name__ == '__main__':
	subscribe.callback(on_message_do,mqtt_topic, hostname=mqtt_broker)
