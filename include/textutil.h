#ifndef TEXTUTIL_H
#define TEXTUTIL_H

#include <cctype>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

inline std::string trim(const std::string& text) {
    const char* spaces = " \t\r\n";
    std::size_t start = text.find_first_not_of(spaces);
    if (start == std::string::npos) {
        return "";
    }
    std::size_t end = text.find_last_not_of(spaces);
    return text.substr(start, end - start + 1);
}

inline std::string toLowerCopy(std::string text) {
    for (char& ch : text) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return text;
}

inline bool containsIgnoreCase(const std::string& text, const std::string& needle) {
    if (needle.empty()) {
        return false;
    }
    return toLowerCopy(text).find(toLowerCopy(needle)) != std::string::npos;
}

inline std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(text);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

inline bool parseInt(const std::string& text, int& value) {
    try {
        const std::string clean = trim(text);
        std::size_t used = 0;
        const int parsed = std::stoi(clean, &used);
        if (clean.empty() || used != clean.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

inline bool parseDouble(const std::string& text, double& value) {
    try {
        const std::string clean = trim(text);
        std::size_t used = 0;
        const double parsed = std::stod(clean, &used);
        if (clean.empty() || used != clean.size() || !std::isfinite(parsed)) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

inline std::string clip(const std::string& text, std::size_t width) {
    if (text.size() <= width) {
        return text;
    }
    if (width < 4) {
        return text.substr(0, width);
    }
    return text.substr(0, width - 3) + "...";
}

#endif
