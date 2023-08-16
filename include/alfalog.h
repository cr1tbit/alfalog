#pragma once

#include <Arduino.h>
#include <Wire.h>

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


    void log(alog_level_t level, const std::string& msg) {
        for (auto& backend : _backends) {
            backend->log(level, msg);
        }
    }

    private:
        std::vector<AlfaBackend*> _backends;
};

AlfaLogger_ &AlfaLogger = AlfaLogger.getInstance();

