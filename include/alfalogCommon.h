#pragma once

#define ALOGT(...) AlfaLogger.log(LOG_TRACE,  __FILE__, __LINE__, fmt::format(__VA_ARGS__));
#define ALOGD(...) AlfaLogger.log(LOG_DEBUG,  __FILE__, __LINE__, fmt::format(__VA_ARGS__));
#define ALOGI(...) AlfaLogger.log(LOG_INFO,   __FILE__, __LINE__, fmt::format(__VA_ARGS__));
#define ALOGW(...) AlfaLogger.log(LOG_WARN,   __FILE__, __LINE__, fmt::format(__VA_ARGS__));
#define ALOGE(...) AlfaLogger.log(LOG_ERROR,  __FILE__, __LINE__, fmt::format(__VA_ARGS__));
#define ALOGV(...) AlfaLogger.log(LOG_VIP,    __FILE__, __LINE__, fmt::format(__VA_ARGS__));

#define ALOGHD(ptr,len) AlfaLogger.hexdump(ptr,len);

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_VIP,
    LOG_RAW,
    LOG_DISABLED
} alog_level_t;