/**
 * @file    log.h
 * @brief   Logging abstraction layer over printf (UART output)
 *
 * @details Provides macros for leveled logging: INFO, WARN, ERROR, DEBUG, VERBOSE.
 *          Each level can be individually enabled/disabled at compile time.
 *          Usage is identical to printf.
 *
 * @example
 *   LOG_INFO("System started");
 *   LOG_DEBUG("Counter = %d", counter);
 *   LOG_ERROR("Init failed, code: 0x%02X", err_code);
 */

#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <stdio.h>

/* =========================================================================
 * CONFIGURATION — override these in your build system or before #include
 * ========================================================================= */

/** Master switch — set to 0 to silence ALL log output in one shot */
#ifndef LOG_ENABLE
#define LOG_ENABLE 0 /* Off by default — set to 1 to enable logging */
#endif

/** Per-level enable flags (1 = on, 0 = off) */
#ifndef LOG_LEVEL_INFO_ENABLE
#define LOG_LEVEL_INFO_ENABLE 0 /* Off by default — enable for important runtime events */
#endif

#ifndef LOG_LEVEL_WARN_ENABLE
#define LOG_LEVEL_WARN_ENABLE 0 /* Off by default — enable when you want to see warnings */
#endif

#ifndef LOG_LEVEL_ERROR_ENABLE
#define LOG_LEVEL_ERROR_ENABLE 0 /* Off by default — enable when you want to see errors */
#endif

#ifndef LOG_LEVEL_DEBUG_ENABLE
#define LOG_LEVEL_DEBUG_ENABLE 0 /* Off by default — enable for development builds */
#endif

#ifndef LOG_LEVEL_VERBOSE_ENABLE
#define LOG_LEVEL_VERBOSE_ENABLE 0 /* Off by default — very chatty */
#endif

/**
 * Show file + line stamp on every log line?
 *   Format appended: [file.c:123]
 */
#ifndef LOG_SHOW_LOCATION
#define LOG_SHOW_LOCATION 0 /* Off by default — enable if you want file/line info */
#endif

/**
 * Show function name on every log line?
 *   Format appended: [funcName]
 */
#ifndef LOG_SHOW_FUNC
#define LOG_SHOW_FUNC 0 /* Off by default — enable if you want function name info */
#endif

/* =========================================================================
 * ANSI COLOR CODES (disabled automatically if your terminal doesn't support)
 * ========================================================================= */

#ifndef LOG_USE_COLOR
#define LOG_USE_COLOR 0 /* Off by default — set to 1 to enable colored output (if supported) */
#endif

#if LOG_USE_COLOR
#define _LOG_COLOR_RESET "\033[0m"
#define _LOG_COLOR_INFO "\033[32m"    /* Green  */
#define _LOG_COLOR_WARN "\033[33m"    /* Yellow */
#define _LOG_COLOR_ERROR "\033[31m"   /* Red    */
#define _LOG_COLOR_DEBUG "\033[36m"   /* Cyan   */
#define _LOG_COLOR_VERBOSE "\033[37m" /* White  */
#else
#define _LOG_COLOR_RESET ""
#define _LOG_COLOR_INFO ""
#define _LOG_COLOR_WARN ""
#define _LOG_COLOR_ERROR ""
#define _LOG_COLOR_DEBUG ""
#define _LOG_COLOR_VERBOSE ""
#endif

/* =========================================================================
 * INTERNAL HELPERS — do not use directly
 * ========================================================================= */

/** Location stamp: appended when LOG_SHOW_LOCATION == 1 */
#if LOG_SHOW_LOCATION
#define _LOG_LOCATION_FMT " [%s:%d]"
#define _LOG_LOCATION_ARGS , __FILE__, __LINE__
#else
#define _LOG_LOCATION_FMT ""
#define _LOG_LOCATION_ARGS
#endif

/** Function stamp: appended when LOG_SHOW_FUNC == 1 */
#if LOG_SHOW_FUNC
#define _LOG_FUNC_FMT " [%s]"
#define _LOG_FUNC_ARGS , __func__
#else
#define _LOG_FUNC_FMT ""
#define _LOG_FUNC_ARGS
#endif

/**
 * Core logging primitive.
 *   _color   — ANSI escape string
 *   _tag     — level label shown in brackets, e.g. "INFO"
 *   _fmt     — user's format string
 *   __VA_ARGS__ — user's variadic arguments
 */
#define _LOG_WRITE(_color, _tag, _fmt, ...)                                           \
    printf(_color "[" _tag "]" _LOG_LOCATION_FMT _LOG_FUNC_FMT                        \
                  " " _fmt _LOG_COLOR_RESET "\r\n" _LOG_LOCATION_ARGS _LOG_FUNC_ARGS, \
           ##__VA_ARGS__)

/* =========================================================================
 * PUBLIC MACROS
 * ========================================================================= */

#if LOG_ENABLE

/**
 * @brief Normal informational message.
 *        Use for startup banners, state transitions, milestone events.
 */
#if LOG_LEVEL_INFO_ENABLE
#define LOG_INFO(_fmt, ...) \
    _LOG_WRITE(_LOG_COLOR_INFO, "INFO ", _fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(_fmt, ...) \
    do                      \
    {                       \
    } while (0)
#endif

/**
 * @brief Warning — something unexpected but recoverable.
 */
#if LOG_LEVEL_WARN_ENABLE
#define LOG_WARN(_fmt, ...) \
    _LOG_WRITE(_LOG_COLOR_WARN, "WARN ", _fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(_fmt, ...) \
    do                      \
    {                       \
    } while (0)
#endif

/**
 * @brief Error — something went wrong; action may be required.
 */
#if LOG_LEVEL_ERROR_ENABLE
#define LOG_ERROR(_fmt, ...) \
    _LOG_WRITE(_LOG_COLOR_ERROR, "ERROR", _fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(_fmt, ...) \
    do                       \
    {                        \
    } while (0)
#endif

/**
 * @brief Debug — internal values, flow tracing.
 *        Strip from release builds by setting LOG_LEVEL_DEBUG_ENABLE=0.
 */
#if LOG_LEVEL_DEBUG_ENABLE
#define LOG_DEBUG(_fmt, ...) \
    _LOG_WRITE(_LOG_COLOR_DEBUG, "DEBUG", _fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(_fmt, ...) \
    do                       \
    {                        \
    } while (0)
#endif

/**
 * @brief Verbose — very fine-grained tracing (loop internals, ISR counts…).
 *        Off by default; enable only when hunting hard bugs.
 */
#if LOG_LEVEL_VERBOSE_ENABLE
#define LOG_VERBOSE(_fmt, ...) \
    _LOG_WRITE(_LOG_COLOR_VERBOSE, "VERB ", _fmt, ##__VA_ARGS__)
#else
#define LOG_VERBOSE(_fmt, ...) \
    do                         \
    {                          \
    } while (0)
#endif

/**
 * @brief Unconditional raw print — bypasses level filtering.
 *        Useful for separators, banners, or test output.
 */
#define LOG_RAW(_fmt, ...) printf(_fmt, ##__VA_ARGS__)

#else /* LOG_ENABLE == 0 — silence everything */

#define LOG_INFO(_fmt, ...) \
    do                      \
    {                       \
    } while (0)
#define LOG_WARN(_fmt, ...) \
    do                      \
    {                       \
    } while (0)
#define LOG_ERROR(_fmt, ...) \
    do                       \
    {                        \
    } while (0)
#define LOG_DEBUG(_fmt, ...) \
    do                       \
    {                        \
    } while (0)
#define LOG_VERBOSE(_fmt, ...) \
    do                         \
    {                          \
    } while (0)
#define LOG_RAW(_fmt, ...) \
    do                     \
    {                      \
    } while (0)

#endif /* LOG_ENABLE */

/* =========================================================================
 * UTILITY MACROS
 * ========================================================================= */

/** Print a divider line — useful between test sections */
#define LOG_DIVIDER() LOG_RAW("----------------------------------------\r\n")

/** Assert-style check: logs an error and returns retval on failure */
#define LOG_ASSERT_RET(_cond, _retval, _fmt, ...)             \
    do                                                        \
    {                                                         \
        if (!(_cond))                                         \
        {                                                     \
            LOG_ERROR("ASSERT FAILED: " _fmt, ##__VA_ARGS__); \
            return (_retval);                                 \
        }                                                     \
    } while (0)

/** Assert-style check: logs an error and returns void on failure */
#define LOG_ASSERT_RET_VOID(_cond, _fmt, ...)                 \
    do                                                        \
    {                                                         \
        if (!(_cond))                                         \
        {                                                     \
            LOG_ERROR("ASSERT FAILED: " _fmt, ##__VA_ARGS__); \
            return;                                           \
        }                                                     \
    } while (0)

#endif /* LOG_H */