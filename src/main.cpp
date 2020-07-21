#include <Arduino.h>
#include <NCD32Relay.h>
#include <BlynkSimpleEsp32.h>
#include <RGBLED.h>
#include <WiFi.h>
#include <WiFiClient.h>

//Objects
NCD32Relay relayController;
RGBLED rgbLED;
TaskHandle_t backgroundTask;

//Functions
void backgroundTasks(void* pvParameters);

//Variables
char* auth = "QpIT6zfPE3L682Wl9-C7ANCDahlLbDep";
char* ssid = "Travis-WiFi";
char* pass = "2Washington99";

void setup() {
  // put your setup code here, to run once:
  //Initialize Serial, Relay Controller, Blynk and RGBLED
  Serial.begin(115200);
  relayController.setAddress(0, 0);
  rgbLED.init(2,15,13,COMMON_ANODE, false);
	rgbLED.setMode(rgbLED.MODE_BOOT);
  Blynk.begin(auth, ssid, pass);

  //Create background thread that will handle the status of the RGB LED.  This keeps it running smooth even if there are communication delays on the main thread
  xTaskCreatePinnedToCore(backgroundTasks, "BackGround Tasks", 20000, NULL, 1, &backgroundTask, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
}

//Handle writes to virtual pins which will be used to control relays.
BLYNK_WRITE_DEFAULT(){
  int pin = request.pin;
  int value = param.asInt();

  Serial.printf("%i written to pin %i\n", value, pin);

  switch(value){
    case(0):{
      relayController.turnOffRelay(pin);
      break;
    }
    case(1):{
      relayController.turnOnRelay(pin);
      break;
    }
  }
}

BLYNK_CONNECTED() {
  Serial.println("Connected to Server");
  rgbLED.setMode(rgbLED.SERVER_CONNECTED);
}

BLYNK_APP_CONNECTED() {
  Serial.println("App connected");
  rgbLED.setMode(rgbLED.APP_CONNECTED);
}

BLYNK_APP_DISCONNECTED() {
  Serial.println("App disconnected");
  rgbLED.setMode(rgbLED.SERVER_CONNECTED);
}

void backgroundTasks(void* pvParameters){
  for(;;){
    rgbLED.loop();
    vTaskDelay(10);
  }
  vTaskDelete( NULL );
}
