#pragma once

#define ALOGD(...) AlfaLogger.log(LOG_DEBUG, fmt::format(__VA_ARGS__));
#define ALOGI(...) AlfaLogger.log(LOG_INFO, fmt::format(__VA_ARGS__));
#define ALOGW(...) AlfaLogger.log(LOG_WARN, fmt::format(__VA_ARGS__));
#define ALOGE(...) AlfaLogger.log(LOG_ERROR, fmt::format(__VA_ARGS__));

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DISABLED
} alog_level_t;