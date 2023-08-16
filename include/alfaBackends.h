#pragma once 

#include <alfalogCommon.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

#define ALOG_FANCY true

class AlfaBackend {
    public:
    virtual ~AlfaBackend() {};
    virtual void log(alog_level_t level, const std::string& msg) = 0;
    virtual void begin() = 0;

    alog_level_t _level = LOG_DISABLED;

    const std::string getDefaultPrefix(alog_level_t level) {
        switch (level) {
            case LOG_TRACE:
                return std::string("T:");
            case LOG_DEBUG:
                return std::string("D:");
            case LOG_INFO:
                return std::string("I:");
            case LOG_WARN:
                return std::string("W:");
            case LOG_ERROR:
                return std::string("E:");
            case LOG_VIP:
                return std::string("V:");
            default:
                return std::string("??");
        }
    }

    const std::string getFancyPrefix(alog_level_t level) {
        switch (level) {
            case LOG_TRACE:
                return std::string(cyan + "TRC:" + reset);
            case LOG_DEBUG:
                return std::string(green + "DBG:" + reset);
            case LOG_INFO:
                return std::string(magenta + "INF:" + reset);
            case LOG_WARN:
                return std::string(yellow + "WRN:" + reset);
            case LOG_ERROR:
                return std::string(red + "ERR:" + reset);
            case LOG_VIP:
                return std::string(cyan + "VIP:" + reset);
            default:
                return std::string("???:");
        }
    }

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

    logs.insert(logs.begin(),getDefaultPrefix(level) + msg);
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

SerialLogger(std::function<void(const char*)> alogPrintHandle, alog_level_t target_level, bool isFancy = false){
    this->_ser_handle = alogPrintHandle;
    _level = target_level;
    _fancy = isFancy;
}
void begin() {
    _started = true;
}

void log(alog_level_t level, const std::string& msg) {
    if (level < _level) { return; }
    if (_fancy) {
        _ser_handle((getFancyPrefix(level) + " " + msg).c_str());
    } else {
        _ser_handle((getDefaultPrefix(level) + " " + msg).c_str());
    }
}

private:
    std::function<void(const char*)> _ser_handle;
    bool _fancy;
};