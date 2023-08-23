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
    virtual void log(alogLevel_t level, 
        const char* file, int line, 
        const std::string& msg) = 0;
    virtual void begin() = 0;

    alogLevel_t _level = LOG_DISABLED;

    const std::string getDefaultPrefix(alogLevel_t level) {
        switch (level) {
            case LOG_TRACE:
                return std::string("T: ");
            case LOG_DEBUG:
                return std::string("D: ");
            case LOG_INFO:
                return std::string("I: ");
            case LOG_WARN:
                return std::string("W: ");
            case LOG_ERROR:
                return std::string("E: ");
            case LOG_VIP:
                return std::string("V: ");
            case LOG_RAW:
                return std::string("");
            default:
                return std::string("??");
        }
    }

    const std::string getFancyPrefix(alogLevel_t level) {
        switch (level) {
            case LOG_TRACE:
                return std::string(cyan + "TRC: " + reset);
            case LOG_DEBUG:
                return std::string(green + "DBG: " + reset);
            case LOG_INFO:
                return std::string(magenta + "INF: " + reset);
            case LOG_WARN:
                return std::string(yellow + "WRN: " + reset);
            case LOG_ERROR:
                return std::string(red + "ERR: " + reset);
            case LOG_VIP:
                return std::string(cyan + "VIP: " + reset);
            case LOG_RAW:
                return std::string("");
            default:
                return std::string("???: ");
        }
    }

    protected:
        bool _started = false;
};

typedef enum {
    OLED_128x64,
    OLED_128x32
} oledType_t;

typedef enum {
    OLED_NORMAL = 0,
    OLED_UPSIDE_DOWN = 2,
} oledRot_t;

class OledLogger : public AlfaBackend {
public:
OledLogger(TwoWire& i2c, alogLevel_t targetLevel,
    oledType_t oledType = OLED_128x64,
    oledRot_t oledRot = OLED_NORMAL
){
    if (oledType == OLED_128x64) {
        display = Adafruit_SSD1306(128, 64, &i2c);
        lineCount = 8;
    } else {
        display = Adafruit_SSD1306(128, 32, &i2c);
        lineCount = 4;
    }
    _level = targetLevel;
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

void log(alogLevel_t level, 
        const char* file, int line, 
        const std::string& msg) {

    (void) file;
    (void) line;
    
    if (!_started) { return; }
    if (level < _level) { return; }

    logs.insert(logs.begin(),getDefaultPrefix(level) + msg);
    if (logs.size() > lineCount) {
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
    int lineCount;
};


class SerialLogger : public AlfaBackend {
public:

SerialLogger(std::function<void(const char*)> alogPrintHandle, alogLevel_t targetLevel, bool isFancy = false){
    this->_ser_handle = alogPrintHandle;
    _level = targetLevel;
    _fancy = isFancy;
}
void begin() {
    _started = true;
}

void log(alogLevel_t level, 
    const char* file, int line, 
    const std::string& msg) {

    (void) file;
    (void) line;

    if (level < _level) { return; }

    std::string fileLine = "";

    if ((_printFileLine) && (line != 0)) {
        fileLine += fmt::format("{}:{} ",file, line);
    }

    std::string logString = fmt::format(
        "{}{}{}",
        _fancy ? getFancyPrefix(level) : getDefaultPrefix(level),
        fileLine,
        msg
    );
    _ser_handle(logString.c_str());
}

private:
    std::function<void(const char*)> _ser_handle;
    bool _fancy;
    bool _printFileLine = true;
};