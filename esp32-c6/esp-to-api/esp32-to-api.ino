/*
Code for ESP32 C6 development board. 

Purpose is to connect to WiFi Network, stored in Preferences.h and send some data to a 
remote API.
*/
#include <Preferences.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <DHT.h>


#define DHT22_PIN  21 // ESP32 pin GPIO21 connected to DHT22 sensor
DHT dht22(DHT22_PIN, DHT22);

const char* serverUrl = "http://192.168.0.152:8001/temp_hum_sensor_data/";

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

    dht22.begin(); // initialize the DHT22 sensor
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
    // read humidity
    float humi  = dht22.readHumidity();
    // read temperature in Celsius
    float tempC = dht22.readTemperature();

    // check whether the reading is successful or not
    if ( isnan(tempC) || isnan(humi)) {
      Serial.println("Failed to read from DHT22 sensor!");
    } else {
      Serial.print("Humidity: ");
      Serial.print(humi);
      Serial.print("%");

      Serial.print("  |  ");

      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.print("°C  ~  ");
    }

      // Get current datetime (replace with RTC or NTP if needed)
    String datetime = "2025-03-19T12:34:56";  // Replace with actual datetime logic

    // Create JSON payload
    String payload = "{";
    payload += "\"temperature\":" + String(temperature, 2) + ",";
    payload += "\"humidity\":" + String(humidity, 2) + ",";
    payload += "\"macaddress\":\"" + String(macAddress) + "\",";
    payload += "}";

    // Send HTTP POST request
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }

      http.end();
    } else {
      Serial.println("WiFi not connected");
    }

    // Wait before sending the next reading
    delay(300000);  // Send data every 10 seconds
}
