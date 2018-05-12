// KLL test MQTT on ESP8266 OLED
// rev with OLED ( buit this time in a separate TAB )
// + build in AKN on remote commands
/* noticed following problem at boot
pi@RPI3_MQTT:~/projects/py3_mqtt $ MQTTshow
pcu213/out PCU booted
pcu213/out 1
pcu213/out 0
pcu213/out PCU booted
 */
// + + make a noisy sinus "analog" signal and send it to "sensor application"

// this example for 
// pushbutton on GPIO0 ( change state )
// output relay LED on GPIO12
// my board not have a LED there connected so i just leave it as it is
/*
https://www.esp8266.com/viewtopic.php?f=29&t=8746&sid=c67335f0ddfa2991b7d11b8a6f1d96cb#
 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/

#include <ESP8266WiFi.h>
// need library:  [sketch][include library][manage libraries][search "pubsubclient"]
// https://pubsubclient.knolleary.net/
#include <PubSubClient.h>
// need library:
#include <Bounce2.h>
#include <EEPROM.h>


const char* ssid = "kll-wlan_2.4G";
const char* password = "WELOVEKOH-SAMUI";
#define useFIXIP      // or disable with //
#if defined useFIXIP
IPAddress STAip(192,168,1,213);
IPAddress STAgateway(192, 168, 1, 1);
IPAddress STAsubnet(255, 255, 255, 0);
#endif


// RPI3 as MQTT broker //YourMQTTBroker'sIP
const char* mqtt_server = "192.168.1.203";  

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
// KLL some global texts for OLED
String connectinfo3 = "try MQTT";
// make a new page on the OLED ( with 5 lines ) to show last 5 mqtt topics
String mqttcom1 = "1";
String mqttcom2 = "2";
String mqttcom3 = "3";
String mqttcom4 = "4";
String mqttcom5 = "5";
String mqttcom6 = "6";

// KLL try new device "pcu"+ IP and ?channel? naming instead of "Sonoff1"
const char* outTopic = "pcu213/out";
const char* inTopic = "pcu213/in";
const char* sensorTopic = "pcu213/sensor/out";
// KLL try learn how to use Client info like:  https://mosquitto.org/man/mosquitto-conf-5.html
// mosquitto.conf
// clientid_prefixes prefix
// connection_messages [ true
// allow_anonymous [ true | false ]
// user <username>   // password_file
// mosquitto_pub [[-h hostname] [-i client_id] [-I client id prefix][-q message QoS][ [-u username] [-P password] ]
// https://mosquitto.org/man/mosquitto_pub-1.html
const char* mqttCl_Name = "kll_engineering";  // use where ?
const char* mqttCl_Id_prefix   = "kll";       // if set in mosquitto.conf works like a mini password? pubsubclient can use?
const char* mqttCl_Id   = "kll/pcu213";       // used in client.connect(mqttCl_Id)
const char* mqttUsr   = "default";       // used in client.connect(mqttCl_Id,mqttUsr,mqttPwd)
const char* mqttPwd   = "password";

// -i mqttCl_Id -I mqttCl_Id_prefix

// local hardware
int relay_pin = 12;
int button_pin = 0;
bool relayState = LOW;

// Instantiate a Bounce object :
Bounce debouncer = Bounce();


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
#if defined useFIXIP
  WiFi.config(STAip,STAgateway,STAsubnet); 
#endif

  while (WiFi.status() != WL_CONNECTED) {
    extButton();
    for(int i = 0; i<500; i++){
      extButton();
      delay(1);
    }
    Serial.print(".");
  }
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  Serial.println("");
  Serial.print("WiFi connected IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  new_mqtt_info(String(topic)+"_"+(char)payload[0]);
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(relay_pin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("relay_pin -> LOW");
    relayState = LOW;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
    client.publish(outTopic, "AKN 0");
  } else if ((char)payload[0] == '1') {
    digitalWrite(relay_pin, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("relay_pin -> HIGH");
    relayState = HIGH;
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
    client.publish(outTopic, "AKN 1");
  } else if ((char)payload[0] == '2') {
    relayState = !relayState;
    digitalWrite(relay_pin, relayState);  // Turn the LED off by making the voltage HIGH
    Serial.print("relay_pin -> switched to ");
    Serial.println(relayState);
    EEPROM.write(0, relayState);    // Write state to EEPROM
    EEPROM.commit();
    client.publish(outTopic, "AKN 2");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
//    if (client.connect("ESP8266Client")) {   // now use client ID, user , password
    if (client.connect(mqttCl_Id,mqttUsr,mqttPwd)) {
      Serial.print("connected as: "); Serial.print(mqttCl_Id);Serial.print(" usr: "); Serial.print(mqttUsr); Serial.println(" and pwd ");
      // Once connected, publish an announcement...
      client.publish(outTopic, "pcu213 reconnected");
      // ... and resubscribe
      client.subscribe(inTopic);
      connectinfo3 = "MQTT connected";
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for(int i = 0; i<5000; i++){
        extButton();
        connectinfo3 = "retry MQTT";
        show_OLED();        // see TAB OLED
        delay(1);
      }
    }
  }
}

void extButton() {
  debouncer.update();
   
   // Call code if Bounce fell (transition from HIGH to LOW) :
   if ( debouncer.fell() ) {
     Serial.println("Debouncer fell");
     // Toggle relay state :
     relayState = !relayState;
     digitalWrite(relay_pin,relayState);
     EEPROM.write(0, relayState);    // Write state to EEPROM
     if (relayState == 1){
      client.publish(outTopic, "1 by extButton fell");
     }
     else if (relayState == 0){
      client.publish(outTopic, "0 by extButton fell");
     }
   }
}

void setup() {
  EEPROM.begin(512);              // Begin eeprom to store on/off state
  pinMode(relay_pin, OUTPUT);     // Initialize the relay pin as an output
  pinMode(button_pin, INPUT);     // Initialize the relay pin as an output
  pinMode(13, OUTPUT);
  relayState = EEPROM.read(0);
  digitalWrite(relay_pin,relayState);
 
  debouncer.attach(button_pin);   // Use the bounce2 library to debounce the built in button
  debouncer.interval(50);         // Input must be low for 50 ms
 
  digitalWrite(13, LOW);          // Blink to indicate setup
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
 
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  OLED_setup();
}

void loop() {
  show_OLED();        // see TAB OLED
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  extButton();
  make_signal();
}

