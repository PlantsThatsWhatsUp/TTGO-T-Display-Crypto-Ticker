
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "CoinMarketCapApi.h"
#include <HTTPClient.h>
#include "alert.h"
#include "info.h"
#include "eth.h"
#include "matic.h"

//---------------------------------------//

// Edit this for your network

#define wifi_ssid "XXX"
#define wifi_password "XXX"



//---------------------------------------//


WiFiClientSecure client;



#include <TFT_eSPI.h>
#include <SPI.h>
#include <Button2.h>

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI        19
#define TFT_SCLK        18
#define TFT_CS          5
#define TFT_DC          16
#define TFT_RST         23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
int vref = 1100;
TFT_eSPI tft = TFT_eSPI(135, 240); 

#define TFT_AQUA  0x04FF
#define TFT_GRAY  0x8410

void setup()
{
    Serial.begin(115200);
    Serial.println(F("Boot Ticker"));

    tft.setRotation(1);  //rotate 90 degree
    tft.fillScreen(TFT_BLACK);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("connecting to wifi");
    delay(1000);
    button_init();
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}



void printTickerData(String ticker, String ID)
{
    Serial.println("---------------------------------");
    Serial.println("Getting ticker data for " + ticker);

    // This displays the logos, so you'll want to replace these files with any coins you prefer to display (i.e., eth.h and matic.h)
    tft.fillRect(0, 0, 240, 135, TFT_BLACK);
    if (ID == "ETH") {
      tft.fillRect(0, 0, 240, 135, TFT_BLACK);
      tft.pushImage(0, 0, ethWidth, ethHeight, eth);
    }
    else if (ID == "MATIC") {
      tft.fillRect(0, 0, 240, 135, TFT_BLACK);
      tft.pushImage(0, 0, maticWidth, maticHeight, matic);
    }
    // Use the kraken API 
    HTTPClient http;
    http.begin("https://api.kraken.com/0/public/Ticker?pair=" + ticker); 
    int httpCode = http.GET(); 
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    float crypto_price = doc["result"][ticker]["c"][0];  //get price
    float crypto_v = doc["result"][ticker]["v"][1]; // get volume
    //display volume in million
    float crypto_volume = crypto_v * crypto_price /1000000;
    

  
        
        Serial.print("Price of " + ID + " ");
        Serial.println(String(crypto_price));
        Serial.println(String(crypto_volume));

    


    


        tft.setTextColor(TFT_GRAY);
        tft.drawString(ID + " USD", 60, 15, 4);
        
       

      
        
        tft.setTextColor(TFT_WHITE);

        tft.fillRect(80, 37, 160, 80, TFT_BLACK); //price
        tft.setTextColor(TFT_GREEN);
         tft.drawString(String(crypto_price), 60, 45, 6);
        tft.fillRect(0, 90, 50, 120, TFT_BLACK);
        tft.setTextColor(TFT_GRAY);
        tft.drawString("volume: ", 12, 105, 4);
        tft.setTextColor(TFT_AQUA);
        String(crypto_volume2) = String(crypto_volume) + " mil";
        tft.drawString(String(crypto_volume2), 115, 105, 4);
        
        //get & display voltage
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = " " + String(battery_voltage) + "V";
        tft.setTextColor(TFT_GRAY);
        Serial.println(voltage);
        tft.drawString(voltage, 190, 0, 2);

}



 
//set up the buttons  
void button_init()
{
    btn1.setPressedHandler([](Button2 & b) {
    printTickerData("MATICUSD", "MATIC");
    Serial.println("button 1 test");
    });
    btn2.setPressedHandler([](Button2 & b) {
    printTickerData("XETHZUSD", "ETH");
    Serial.println("button 2 test");
    });
}
void button_loop()
{
    btn1.loop();
    btn2.loop();
}
void loop()
{
  button_loop();

}
