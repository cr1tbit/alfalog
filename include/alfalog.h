#pragma once

#include <Arduino.h>
#undef B1 //this is very critical - fmtlib has an conflict with esp32 arduino framework
#include <Wire.h>

#include <vector>
#include <fmt/core.h>

#include <alfalogCommon.h>
#include <alfaBackends.h>

template <> struct fmt::formatter<String> {  
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.begin();
    }
    auto format(const String& s, format_context& ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "{}", s.c_str());
    }
};

template <> struct fmt::formatter<IPAddress> {  
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.begin();
    }
    auto format(const IPAddress& s, format_context& ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "{}", s.toString().c_str());
    }
};

class AlfaLogger_ {
    public:
    AlfaLogger_() = default;

    static AlfaLogger_ &getInstance() {
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

extern AlfaLogger_ &AlfaLogger;
