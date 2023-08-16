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

static std::vector<std::string> split_string(const std::string& input_string, int maxlen) {
    std::vector<std::string> words;
    std::string word;

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


class AdvancedOledLogger : public AlfaBackend {
public:

AdvancedOledLogger(TwoWire& i2c, oled_type_t oled_type, alog_level_t target_level) {
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
    display.setRotation(2);

    display.setTextSize(1);
    display.setFont(&Picopixel);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    _started = true;
}

void insert_log_line(std::string& line){
    lines.push_back(line);
    if (lines.size() > line_count) {
        lines.erase(lines.begin());
    }
}

void render_top_bar(){
    display.fillRect(0, 0, 128, 7, SSD1306_WHITE);
    display.setCursor(1, 5);
    display.setTextColor(SSD1306_BLACK);

    std::string text = fmt::format(
        "{} {:>16}",
        staticTopBarText.c_str(),
        (WiFi.status() == WL_CONNECTED)?
            WiFi.localIP().toString().c_str():
            "[No internet]"
    );
    display.print(text.c_str());
}

void render_logs(){
    int start_y = 13;
    display.setTextColor(SSD1306_WHITE);
    for (auto& line : lines) {
        display.setCursor(left_offs,start_y);
        display.print(line.c_str());
        start_y += font_height;
    }
}

void render_status_icon(){
    // display.drawBitmap(80, 10, test, 16, 13, 1);
}

void redraw(){    
    display.clearDisplay();
    
    render_logs();
    render_top_bar();
    render_status_icon();

    display.display();
}

void log(alog_level_t level, const std::string& msg) {
    if (!_started) { return; }
    if (level < _level) { return; }
    
    auto words = split_string((getDefaultPrefix(level) + " " + msg), log_line_length);

    std::string line;

    for (auto& word : words) {
        if (line.length() + word.length() > log_line_length) {
            insert_log_line(line);
            line.clear();
        }
        line += word + " ";
    }
    insert_log_line(line);
    redraw();
}

std::string staticTopBarText;

private:
    const int font_height = 6;
    const int font_width = 3;
    const int left_offs = 0;
    const int log_line_length = 36;

    Adafruit_SSD1306 display;
    std::vector<std::string> lines;
    int line_count;
};