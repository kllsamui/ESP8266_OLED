#!/usr/bin/env python3
# filename: py3_mqtt.py
import paho.mqtt.publish as publish
import time, datetime
mqtt_broker="127.0.0.1"		# local host
# command channel to ESP8266 on 192.168.1.213 to operate LED/RELAY output
mqtt_topic="pcu213/in"

def mqtt_send(val):
	publish.single(mqtt_topic,val, hostname=mqtt_broker)
	stime='{0:%Y-%m-%d %H:%M:%S}'.format(datetime.datetime.now())
	print("%s sending: %s to %s" % (stime,val,mqtt_topic))



mqtt_send("1")
time.sleep(5)
mqtt_send("0")
print("end")
