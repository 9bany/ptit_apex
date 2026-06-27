#pragma once

// Ma ANSI cho terminal color — hoat dong tren macOS, Linux, Windows 10+.
namespace Color {
    constexpr const char* reset  = "\033[0m";
    constexpr const char* bold   = "\033[1m";
    constexpr const char* dim    = "\033[2m";
    constexpr const char* red    = "\033[31m";
    constexpr const char* green  = "\033[32m";
    constexpr const char* yellow = "\033[33m";
    constexpr const char* cyan   = "\033[36m";
}
