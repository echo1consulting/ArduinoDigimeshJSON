#include <ArduinoJson.h>
#include <Bee.h>
#include <SoftwareSerial.h>

Bee XBee(&Serial, 9600);

// This is roughly 726. This was calculated using: https://bblanchon.github.io/ArduinoJson/assistant/
const size_t bufferSize = JSON_ARRAY_SIZE(3) + 3 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 3 * JSON_OBJECT_SIZE(6) + 400;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // wait serial port initialization
  }

  // Set the callback
  XBee.setCallback(beeCallback);
}

void beeCallback(BeePointerFrame *frame) {
  Serial.write(frame->data, frame->dataLength);
}

void loop() {

  // Set the buffer size dynamically
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Create the root of the object tree.
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& device = jsonBuffer.createObject();

  // Add values in the object
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root.set<long>("time", 1351824120);
  device["nam"] = "dev-one";
  device["tim"] = 1351824120;

  // Add an array for sensors
  JsonArray& sensors = device.createNestedArray("sen");

  // Add gps sensor data
  JsonObject& gps = jsonBuffer.createObject();
  gps["lat"] = "41.40338";
  gps["lon"] = "2.17403";
  gps["spe"] = "60";

  // Add a gps sensor and associate the gps data
  JsonObject& sensor = jsonBuffer.createObject();
  sensor["typ"] = "gps";
  sensor["dat"] = gps;
  sensors.add(sensor);

  // Initialize a local message variable
  String message = "";

  // Print the json to the message variable
  device.printTo(message);

  // Set the message length
  int messageLen = message.length() + 1;

  // Set a sized array for packets
  char packets[messageLen];

  // Populate the packets array with characters
  message.toCharArray(packets, messageLen);

  // Send the packets over xbee
  XBee.sendData(packets, messageLen);

  // Set a delay
  delay(2000);

  // Tick (internal xbee method)
  XBee.tick();

}
