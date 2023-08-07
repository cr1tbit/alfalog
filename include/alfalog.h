#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>

#undef B1 //this is very critical - fmtlib has an conflict with esp32 arduino framework
#include <fmt/core.h>

#include <alfalogCommon.h>
#include <alfaBackends.h>


class AlfaLogger_ {
    public:
    AlfaLogger_() = default;

    static AlfaLogger_ &getInstance(){
        static AlfaLogger_ instance;
        return instance;
    }

    public:
    void addBackend(AlfaBackend* backend) {
        _backends.push_back(backend);
    }

    void begin() {
        for (auto& backend : _backends) {
            backend->begin();
        }
    }

    const std::string getPrefix(alog_level_t level) {
        switch (level) {
            case LOG_DEBUG:
                return std::string("D:");
            case LOG_INFO:
                return std::string("I:");
            case LOG_WARN:
                return std::string("W:");
            case LOG_ERROR:
                return std::string("E:");
            default:
                return std::string("??");
        }
    }

    void log(alog_level_t level, const std::string& msg) {
        for (auto& backend : _backends) {
            backend->log(level, getPrefix(level) + msg);
        }
    }

    private:
        std::vector<AlfaBackend*> _backends;
};

AlfaLogger_ &AlfaLogger = AlfaLogger.getInstance();

