#pragma once

#define ALOGT(...) AlfaLogger.log(LOG_TRACE, fmt::format(__VA_ARGS__));;
#define ALOGD(...) AlfaLogger.log(LOG_DEBUG, fmt::format(__VA_ARGS__));
#define ALOGI(...) AlfaLogger.log(LOG_INFO, fmt::format(__VA_ARGS__));
#define ALOGW(...) AlfaLogger.log(LOG_WARN, fmt::format(__VA_ARGS__));
#define ALOGE(...) AlfaLogger.log(LOG_ERROR, fmt::format(__VA_ARGS__));
#define ALOGV(...) AlfaLogger.log(LOG_VIP, fmt::format(__VA_ARGS__));

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_VIP,
    LOG_DISABLED
} alog_level_t;