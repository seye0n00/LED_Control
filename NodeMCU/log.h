#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

/* Log Configuration */
#define ENABLE_LOG 0        // 0: OFF, 1: ON
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 1

/* Log Macros */
#if ENABLE_LOG

  #if LOG_LEVEL_INFO
    #define LOG_I(tag, msg) \
      Serial.println(String("[INFO] ") + tag + " : " + msg)
  #else
    #define LOG_I(tag, msg)
  #endif

  #if LOG_LEVEL_ERROR
    #define LOG_E(tag, msg) \
      Serial.println(String("[ERROR] ") + tag + " : " + msg)
  #else
    #define LOG_E(tag, msg)
  #endif

#else
  #define LOG_I(tag, msg)
  #define LOG_E(tag, msg)
#endif

#endif