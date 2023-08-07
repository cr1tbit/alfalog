#pragma once 

#include <alfalogCommon.h>

class AlfaBackend {
    public:
    virtual ~AlfaBackend() {};
    virtual void log(alog_level_t level, const std::string& msg) = 0;
    virtual void begin() = 0;

    alog_level_t _level = LOG_DISABLED;

    protected:
        bool _started = false;
};

typedef enum {
    OLED_128x64,
    OLED_128x32
} oled_type_t;


class OledLogger : public AlfaBackend {
public:
OledLogger(TwoWire& i2c, oled_type_t oled_type, alog_level_t target_level) {
    if (oled_type == OLED_128x64) {
        display = Adafruit_SSD1306(128, 64, &i2c);
        line_count = 8;
    } else {
        display = Adafruit_SSD1306(128, 32, &i2c);
        line_count = 4;
    }
    _level = target_level;
}

void begin() {

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.invertDisplay(true);

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("hello world");
    display.display();

    _started = true;
}

void log(alog_level_t level, const std::string& msg) {
    if (!_started) { return; }
    if (level < _level) { return; }

    logs.insert(logs.begin(), msg);
    if (logs.size() > line_count) {
        logs.pop_back();
    }

    display.clearDisplay();
    display.setCursor(0,0);
    for (auto& log : logs) {
        display.println(log.c_str());
    }
    display.display();
}

private:
    Adafruit_SSD1306 display;
    std::vector<std::string> logs;
    int line_count;
};


class SerialLogger : public AlfaBackend {
public:

SerialLogger(std::function<void(const char*)> alogPrintHandle, alog_level_t target_level){
    this->_ser_handle = alogPrintHandle;
    _level = target_level;
}
void begin() {
    _started = true;
}

void log(alog_level_t level, const std::string& msg) {
    if (level < _level) { return; }
    _ser_handle(msg.c_str());
}

private:
    std::function<void(const char*)> _ser_handle;
};