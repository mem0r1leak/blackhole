#pragma once

#include <iostream>
#include <string>
#include <string_view>

class Terminal {
public:
    static constexpr std::string_view RESET   = "\x1b[0m";
    static constexpr std::string_view BOLD    = "\x1b[1m";
    static constexpr std::string_view RED     = "\x1b[31m";
    static constexpr std::string_view GREEN   = "\x1b[32m";
    static constexpr std::string_view CYAN    = "\x1b[36m";
    static constexpr std::string_view PURPLE  = "\x1b[35m";

    static void clear() {
        std::cout << "\x1b[2J\x1b[H";
    }

    static void log(const std::string_view msg, const std::string_view color = RESET) {
        std::cout << color << "[*] " << RESET << msg << std::endl;
    }

    static void error(const std::string_view msg) {
        std::cerr << BOLD << RED << "[!] ERROR: " << RESET << msg << std::endl;
    }

    static void draw_progress(const float percent) {
        constexpr int width = 20;
        const int pos = static_cast<int>(width * percent);

        std::cout << "\r" << PURPLE << "[" << RESET;
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << "■";
            else if (i == pos) std::cout << "○";
            else std::cout << " ";
        }
        std::cout << PURPLE << "]" << RESET << " " << static_cast<int>(percent * 100.0) << "%" << std::flush;
    }
};