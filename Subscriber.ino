#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "System"; // Define your own WiFi SSID
const char* password = "123456789"; // Define your own WiFi password
const char* mqttServer = "broker.emqx.io"; // MQTT broker server
const int mqttPort = 1883; // MQTT broker port
const char* mqttUser = "emqx"; // MQTT username
const char* mqttPassword = "public"; // MQTT password

String buttonTopic = "panic/button"; // MQTT topic to subscribe for button status
String touchTopic = "panic/alarm"; // MQTT topic to subscribe for touch status

const int buttonLedPin = 23; // GPIO pin where the button LED is connected
const int touchLedPin = 22; // GPIO pin where the touch LED is connected

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to a string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  if (strcmp(topic, buttonTopic.c_str()) == 0) {
    if (message == "pressed") {
      Serial.println("Button pressed - Turning on button LED");
      digitalWrite(buttonLedPin, HIGH); // Turn on button LED when button is pressed
    } else if (message == "not pressed") {
      Serial.println("Button not pressed - Turning off button LED");
      digitalWrite(buttonLedPin, LOW); // Turn off button LED when button is not pressed
    }
  } else if (strcmp(topic, touchTopic.c_str()) == 0) {
    if (message == "one") {
      Serial.println("Not taken medicine from last two days");
      digitalWrite(touchLedPin, HIGH); // Turn on touch LED when touch is detected
    } else if (message == "zero") {
      Serial.println("Stopped medicine alarm");
      digitalWrite(touchLedPin, LOW); // Turn off touch LED when touch is not detected
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe(buttonTopic.c_str());
      client.subscribe(touchTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonLedPin, OUTPUT);
  pinMode(touchLedPin, OUTPUT);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
