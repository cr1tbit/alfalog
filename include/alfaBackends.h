#pragma once 

#include <alfalogCommon.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const std::string alogColorRed("\033[0;31m");
const std::string alogColorGreen("\033[1;32m");
const std::string alogColorYellow("\033[1;33m");
const std::string alogColorCyan("\033[0;36m");
const std::string alogColorMagenta("\033[0;35m");
const std::string alogColorGrey("\033[0;37m");
const std::string alogColorReset("\033[0m");

#define ALOG_FANCY true
#define ALOG_BASIC true

#define ALOG_FILELINE true
#define ALOG_NOFILELINE false

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
                return std::string(alogColorCyan    + "TRC: " + alogColorReset);
            case LOG_DEBUG:
                return std::string(alogColorGreen   + "DBG: " + alogColorReset);
            case LOG_INFO:
                return std::string(alogColorMagenta + "INF: " + alogColorReset);
            case LOG_WARN:
                return std::string(alogColorYellow  + "WRN: " + alogColorReset);
            case LOG_ERROR:
                return std::string(alogColorRed     + "ERR: " + alogColorReset);
            case LOG_VIP:
                return std::string(alogColorCyan    + "VIP: " + alogColorReset);
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

SerialLogger(
    std::function<void(const char*)> alogPrintHandle,
    alogLevel_t targetLevel,
    bool isFancy = false,
    bool printFileLine = true
){
    this->_ser_handle = alogPrintHandle;
    _level = targetLevel;
    _fancy = isFancy;
    _printFileLine = printFileLine;
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

    if ((_printFileLine) && (line > 0)) {
        if (_fancy) {
            fileLine += fmt::format("{}{}:{}{} ",alogColorGrey, file, line, alogColorReset);
        } else {
            fileLine += fmt::format("{}:{} ",file, line);
        }
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