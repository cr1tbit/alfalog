#include <Arduino.h>

#include <Wire.h>
#include <alfalog.h>

// This might not compile, I haven't checked it actually yet lmao

TwoWire i2c = TwoWire(0);

void uartPrintAlogHandle(const char* str){
  Serial.println(str);
}

OledLogger display = OledLogger(i2c, OLED_128x32, LOG_INFO);
SerialLogger serialLogger = SerialLogger(uartPrintAlogHandle, LOG_DEBUG);
Button2 userButton = Button2();

void setup() {  
  Serial.begin(115200);
  i2c.begin(SDA, SCL, 100000);

  AlfaLogger.addBackend(&display);
  AlfaLogger.addBackend(&serialLogger);
  AlfaLogger.begin();
  ALOGI("display started");
}

int counter = 0;
void loop() {
    ALOGD("counter: {}", counter++);
    if (counter >=10){
        ALOGE(
            "counter reached {}, that's too much, performing {}", 
            counter,
            "counter reset");
        counter = 0;
    }
    delay(1000);
}
