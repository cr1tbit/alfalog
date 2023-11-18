#pragma once

#include <Arduino.h>
#undef B1 //this is very critical - fmtlib has an conflict with esp32 arduino framework
#include <Wire.h>

#include <vector>
#include <fmt/core.h>
#include <fmt/printf.h>

#include <alfalogCommon.h>
#include <alfaBackends.h>

std::vector<byte> alogScanI2C(TwoWire &i2c);

const char* alogGetInitString(int line = 0);

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

    void log(alogLevel_t level, 
        const char* file, int line, 
        const std::string& msg
    ) {
        for (auto& backend : _backends) {
            backend->log(level, file, line, msg);
        }
    }

    // https://stackoverflow.com/questions/29242/off-the-shelf-c-hex-dump-code
    // thanks @epatel, that's like 3rd time I'm using this snippet :)
    void hexdump(const void *ptr, int buflen) {
        unsigned char *buf = (unsigned char *)ptr;
        int i, j;

        for (i = 0; i < buflen; i += 16) {
            std::string _hexline = fmt::sprintf("%06x: ", i);
            for (j = 0; j < 16; j++)
                if (i + j < buflen)
                    _hexline += fmt::sprintf("%02x ", buf[i + j]);
                else
                    _hexline += fmt::sprintf("   ");
            _hexline += fmt::sprintf(" ");
            for (j = 0; j < 16; j++)
                if (i + j < buflen)
                    _hexline += fmt::sprintf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
            log(LOG_RAW, "", 0, _hexline);
        }
    }

    private:
        std::vector<AlfaBackend*> _backends;
};

extern AlfaLogger_ &AlfaLogger;
