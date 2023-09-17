#pragma once

#include "WiFi.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/Picopixel.h>

#include <alfalogCommon.h>
#include <alfaBackends.h>

/*
 *
 *    This backend is a bit more advanced than the OledLogger backend. It supports
 *    multiple lines of logs, and it splits long log messages into multiple lines.
 *
 *    It also supports a static top bar, which can be used to display some static
 *    information, like the IP address of the device.
 * 
 *    It also uses a font made for ants, so yeah. 5 Lines of text fit on 128x32 OLED.
 * 
 */

// Split lines nicely when log is longer than the display
static std::vector<std::string> splitString(const std::string& input_string, int maxlen) {
    std::vector<std::string> words;
    std::string word;

    // iterate over chars
    for (const auto& c : input_string) {
        if (std::isspace(c)) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
            if (word.length() > maxlen) {
                words.push_back(word.substr(0, maxlen));
                word = word.substr(maxlen);
            }
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

typedef enum {
    BAR_WIFI_IP,
    BAR_CUSTOM,
} topBarMode_t;

class AdvancedOledLogger : public AlfaBackend {
public:

AdvancedOledLogger(
    TwoWire& i2c, alogLevel_t targetLevel, 
    oledType_t oledType = OLED_128x64,
    oledRot_t oledRot = OLED_NORMAL
){
    if (oledType == OLED_128x64) {
        display = Adafruit_SSD1306(128, 64, &i2c);
        lineCount = 9;
    } else {
        display = Adafruit_SSD1306(128, 32, &i2c);
        lineCount = 4;
    }
    _level = targetLevel;
    this->oledRot = oledRot;
}

void begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setRotation(oledRot);

    display.setTextSize(1);
    display.setFont(&Picopixel);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    _started = true;
}

void insertLogLine(std::string& line){
    lines.push_back(line);
    if (lines.size() > lineCount) {
        lines.erase(lines.begin());
    }
}

void renderTopBar(){
    display.fillRect(0, 0, 128, 7, SSD1306_WHITE);
    display.setCursor(1, 5);
    display.setTextColor(SSD1306_BLACK);

    display.print(staticTopBarText.c_str());

    if (barMode == BAR_WIFI_IP) {
        std::string ipText(
            (WiFi.status() == WL_CONNECTED)?
            WiFi.localIP().toString().c_str():
            "[No internet]"
        );
        int16_t x,y; uint16_t dx,dy;
        display.getTextBounds(ipText.c_str(), 0, 0, &x, &y, &dx, &dy);
        int16_t cursorOffs = display.width() - dx - 1;
        display.setCursor(cursorOffs, 5);
        display.print(ipText.c_str());
    }
}

void renderLogs(){
    int start_y = 13;
    display.setTextColor(SSD1306_WHITE);
    for (auto& line : lines) {
        display.setCursor(left_offs,start_y);
        display.print(line.c_str());
        start_y += font_height;
    }
}

void renderStatusIcon(){
    display.setTextColor(SSD1306_WHITE);
    static int counter = 0;
    char chars[] = {'|', '/', '-', '\\'};
    display.setCursor(
        display.width()-5,
        display.height()-5 
    );
    display.print(chars[counter%4]);
    counter++;
}

bool shouldDebounce(){
    static int lastDebounce = 0;
    static int debounceCount = 0;

    if (millis() - lastDebounce > 100) {
        if (debounceCount < 4){
            lastDebounce = millis();
            debounceCount++;
            return true;
        }        
    }
    debounceCount = 0;
    return false;
}

void redraw(bool forceRedraw = true){
    if (!_started) { return; }
    display.clearDisplay();

    if ((shouldDebounce())&&(forceRedraw == false)){
        return;
    }
    
    renderLogs();
    renderTopBar();
    renderStatusIcon();

    display.display();
}

void log(alogLevel_t level, 
    const char* file, int line, 
    const std::string& msg) {

    (void) file;
    (void) line;

    if (!_started) { return; }
    if (level < _level) { return; }
    
    auto words = splitString((getDefaultPrefix(level) + " " + msg), log_line_length);

    std::string logLine;

    for (auto& word : words) {
        if (logLine.length() + word.length() > log_line_length) {
            insertLogLine(logLine);
            logLine.clear();
        }
        logLine += word + " ";
    }

    insertLogLine(logLine);
    redraw(false);
}

void setTopBarText(topBarMode_t mode, const std::string& text){
    barMode = mode;
    staticTopBarText = text;
    redraw(true);
}

private:
    const int font_height = 6;
    const int font_width = 3;
    const int left_offs = 0;
    const int log_line_length = 36;

    Adafruit_SSD1306 display;
    std::vector<std::string> lines;
    std::string staticTopBarText;

    int lineCount;
    oledRot_t oledRot;
    topBarMode_t barMode;
};