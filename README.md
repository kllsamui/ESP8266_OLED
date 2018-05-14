# ESP8266_OLED
play with OLED / webserver

board info: http://dong-sen.com/d-duino/

my ESP Blog : http://kll.engineering-news.org/kllfusion01/articles.php?article_id=148

using Arduino IDE 1.8.5

and test MQTT with

pubsubclient on ESP8266

# RPI 
install 
mosquitto
and for python3 use
pip3 install paho-mqtt

my RPI Blog : http://kll.engineering-news.org/kllfusion01/articles.php?article_id=149

pls. find under [RPI] python tools to:

 + send command to ESP
 
 + collect measuring records (float values) flat JSON { "Val1":0.12, "Val2":3.45, "Val3":67.8 }
 to a SQLITE3 database at /static/data/myMQTT.db ( with auto table name of mqtt topic and auto local timestamp )
