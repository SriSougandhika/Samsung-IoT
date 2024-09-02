#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "System"; // Define your own WiFi SSID
const char* password = "123456789"; // Define your own WiFi password
const char* mqttServer = "broker.emqx.io"; // MQTT broker server
const int mqttPort = 1883; // MQTT broker port
const char* mqttUser = "emqx"; // MQTT username
const char* mqttPassword = "public"; // MQTT password

String projectName = "panic"; // User-defined project name
String buttonTopic = projectName + "/button"; // MQTT topic to publish button presses
String touchTopic = projectName + "/touch"; // MQTT topic to publish touch events

const int buttonPin = 22; // Digital pin for button
const int touchPin = 23; // Digital pin for touch sensor

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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(projectName.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
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
  setup_wifi();
  client.setServer(mqttServer, mqttPort);

  // Initialize button and touch pin
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(touchPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    // Button is pressed, publish a message
    client.publish(buttonTopic.c_str(), "pressed");
    delay(1000); // debounce delay
  }

  // Check if the touch sensor is touched
  if (digitalRead(touchPin) == HIGH) {
    // Touch sensor is touched, publish a message
    client.publish(touchTopic.c_str(), "touched");
    delay(1000); // debounce delay
  }
}
