#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino_JSON.h>
#include "config.h"

uint32_t chipid;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);
Adafruit_MQTT_Publish pub_register = Adafruit_MQTT_Publish(&mqtt, TOPIC_REGISTER);
Adafruit_MQTT_Publish pub_sensor = Adafruit_MQTT_Publish(&mqtt, TOPIC_BME280);
Adafruit_BME280 bme;

void WIFI_connect();
void MQTT_connect();
void BME280_connect();
void printValues();

void setup() {
  Serial.begin(115200);
  while (!Serial);

  WIFI_connect();
  BME280_connect();
}

void loop() {
  MQTT_connect();
  printValues();
  delay(DELAY_INTERVAL);
}

void WIFI_connect() {
  Serial.print("\n\nConnecting to WIFI (");
  Serial.print(WLAN_SSID);
  Serial.print(") ");

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.print("\nWiFi connected: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void MQTT_connect() {
  int8_t ret;
  uint8_t retries = 3;

  // stop if already connected
  if (mqtt.connected()) {
    return;
  }

  Serial.println("Connecting to MQTT ... ");

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    if (!(retries--)) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  
  Serial.println("MQTT Connected!\n");

  // register device
  chipid = ESP.getChipId();
  String topic = String(TOPIC_BME280) + "/" + String(chipid);
  pub_register.publish(topic.c_str());
  Serial.print("Register device: ");
  Serial.println(topic);
  Serial.println();
}

void BME280_connect() {
  unsigned status = bme.begin(0x76);
  
  if (!status) {
    Serial.println("BME280 connect failed!");
    Serial.println("    VCC - 3V3");
    Serial.println("    GND - GND");
    Serial.println("    SCL - D1");
    Serial.println("    SDA - D2");
    while (1) delay(10);
  }
  
  Serial.println("BME280 connected.\n");
}

void printValues () {
  float temp = bme.readTemperature();
  float pres = bme.readPressure() / 100.0F;
  float humi = bme.readHumidity();

  JSONVar data;
  data["id"] = String(chipid);
  data["temp"] = temp;
  data["pres"] = pres;
  data["humi"] = humi;
  
  String json = JSON.stringify(data);
  Serial.print(json);

  bool flag = pub_sensor.publish(json.c_str());
  Serial.println(flag ? "  OK!" : "  Failed");
}