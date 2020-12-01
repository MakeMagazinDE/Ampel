/* This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */
 
#include <SparkFun_SCD30_Arduino_Library.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#define AnzahlLEDs 8 //8 für einen Ring, 4 für LED Streifen

#define Grenze_Gelb 1000
#define Grenze_Rot  1400

int CO2 = 0 ;

//Reading CO2, humidity and temperature from the SCD30 By: Nathan Seidle SparkFun Electronics 
//https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

SCD30 airSensorSCD30; // Objekt SDC30 Umweltsensor

CRGB leds[AnzahlLEDs]; //Array für 8 RGB LEDs

void setup(){ // Einmalige Initialisierung
  
  WiFi.forceSleepBegin(); // Wifi off
  
  //OnBoard LEDs initialisieren und Selbsttest
  pinMode( 12 , OUTPUT); //D6 am Wemos, Rot
  pinMode( 13 , OUTPUT); //D7 am Wemos, Gelb
  pinMode( 14 , OUTPUT); //D5 am Wemos, Grün
  //Selbsttest
  digitalWrite( 12 , HIGH );
  delay(500);
  digitalWrite( 13 , HIGH );
  delay(500);
  digitalWrite( 14 , HIGH );
  delay(500);

  FastLED.addLeds<NEOPIXEL, D8>(leds, AnzahlLEDs);
    
  //OnBoard LEDs aus  
  digitalWrite( 12 , LOW );
  digitalWrite( 13 , LOW );
  digitalWrite( 14 , LOW );

  //Debug Infos auf seriellen Port
  Serial.begin(115200);
  Serial.println();

  //i2c starten
  Wire.begin();

  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

  if (airSensorSCD30.begin() == false) {
    Serial.println("The SCD30 did not respond. Please check wiring."); 
    while(1) {
      //Rotes Blinken OnBoard
      digitalWrite( 12 , HIGH );
      delay(300);
      digitalWrite( 12 , LOW );
      delay(300);
    } 
  }

  airSensorSCD30.setAutoSelfCalibration(false); // Sensirion no auto calibration
  airSensorSCD30.setMeasurementInterval(2);     // CO2-Messung alle 2 s

  Wire.setClock(100000L);            // 100 kHz SCD30 
  Wire.setClockStretchLimit(200000L);// CO2-SCD30

  //kann losgehen :-)
}

void loop() { // Kontinuierliche Wiederholung 
  CO2 = airSensorSCD30.getCO2() ;
  Serial.print("CO2:"+String(String(CO2)));
  if (CO2 < Grenze_Gelb)
  {
    //WS2812B LEDs auf grün
    for (int i=0; i<AnzahlLEDs; i++)
    {
      //RGB Grün
      leds[i] = 0x00ff00;
    }
    FastLED.show();

    //OnBoard LEDs auf grün
    digitalWrite( 12 , LOW );
    digitalWrite( 13 , LOW );
    digitalWrite( 14 , HIGH );
    Serial.print(" = Grün");
    Serial.println();
  }
  else
  {
    if (( ( CO2 ) < ( Grenze_Rot ) ))
    {
      //WS2812B LEDs auf gelb
      for (int i=0; i<AnzahlLEDs; i++)
      {
         //RGB Gelb
        leds[i] = 0xff4000;
      }
      FastLED.show();

      //OnBoard LEDs auf gelb 
      digitalWrite( 12 , LOW );
      digitalWrite( 13 , HIGH );
      digitalWrite( 14 , LOW );
      Serial.print(" = Gelb");
      Serial.println();
    }
    else
    {
      //WS2812B LEDs auf rot
      for (int i=0; i<AnzahlLEDs; i++)
      {
        //RGB Rot
        leds[i] = 0xff0000;
      }
      FastLED.show();

      //OnBoard LEDs auf rot      
      digitalWrite( 12 , HIGH );
      digitalWrite( 13 , LOW );
      digitalWrite( 14 , LOW );
      Serial.print(" = Rot");
      Serial.println();
    }
  }
  delay( 2000 );
}
