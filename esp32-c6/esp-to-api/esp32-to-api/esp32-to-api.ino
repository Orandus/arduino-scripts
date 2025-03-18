/*
Code for ESP32 C6 development board. 

Purpose is to connect to WiFi Network, stored in Preferences.h and send some data to a 
remote API.
*/
#include <Preferences.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

Preferences preferences;

constexpr uint8_t LED_PIN = 8; //port for integrated RGB LED
constexpr uint8_t NUM_LEDS = 1;

//Wifi settings
String ssid;
String password;
String macAddress;

Adafruit_NeoPixel rgbLed(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct RGB {
    uint8_t r, g, b;
};

constexpr RGB COLOR_OFF   = {0, 0, 0};

constexpr RGB COLOR_RED   = {255, 0, 0};
constexpr RGB COLOR_GREEN = {0, 255, 0};
constexpr RGB COLOR_BLUE  = {0, 0, 255};
constexpr RGB COLOR_WHITE = {255, 255, 255};
constexpr RGB CUSTOM_COLOR = {255, 0, 255}; 

void setup() {
    Serial.begin(115200);

    rgbLed.begin(); 
    rgbLed.show();
    delay(1000);
    setColor(COLOR_RED);
    
    connectToWifi();

    macAddress = WiFi.macAddress();
    Serial.println("Mac Address: " + macAddress);
}

void connectToWifi(){
    setColor(COLOR_RED);
    preferences.begin("credentials", false);
    
    ssid = preferences.getString("ssid", ""); 
    password = preferences.getString("password", "");

    if (ssid == "" || password == ""){
      Serial.println("No values saved for ssid or password");
    }
    else {
      // Connect to Wi-Fi
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      Serial.print("Connecting to WiFi: " + ssid);
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
      }
      setColor(COLOR_GREEN);
      Serial.println("\nConnected to the WiFi network: " + ssid);
      Serial.print("Local ESP32 IP: ");
      Serial.println(WiFi.localIP());  
    }
}

void setColor(const RGB& color) {
    rgbLed.setPixelColor(0, rgbLed.Color(color.r, color.g, color.b));
    rgbLed.show();
}

void loop() {

}
