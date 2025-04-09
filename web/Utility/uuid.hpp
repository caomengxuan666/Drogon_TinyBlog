/**
 * @FilePath     : /tiny_blog/web/Utility/uuid.hpp
 * @Description  : UUID 生成类,比起Drogon轻量级一些，用在需要生成自定义长度的情况下
                  以及当前模块不包含Drogon库链接的情况下
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-04-08 18:19:15
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/

#pragma once

#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


class UUID {
public:
    // 生成指定长度的UUID字符串
    static std::string generate(size_t length = 32) noexcept {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');

        for (size_t i = 0; i < length; ++i) {
            int digit = dis(gen);
            oss << std::setw(1) << digit;
        }

        return std::move(oss.str());// 使用std::move返回字符串
    }

    //生成纯数字字符串
    static std::string generate_num(size_t length = 32) noexcept {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        std::ostringstream oss;
        for (size_t i = 0; i < length; ++i) {
            int digit = dis(gen);
            oss << std::setw(1) << digit;
        }
        return std::move(oss.str());
    }

    // 生成符合RFC 4122标准的UUID字符串
    static std::string generate_rfc4122() noexcept {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::vector<char> uuid(16);
        for (int i = 0; i < 16; ++i) {
            uuid[i] = static_cast<char>(dis(gen));
        }

        // 设置版本号为4 (随机生成)
        uuid[6] = (uuid[6] & 0x0f) | 0x40;
        // 设置变体为1 (RFC 4122)
        uuid[8] = (uuid[8] & 0x3f) | 0x80;

        std::ostringstream oss;
        oss << std::hex << std::setfill('0')
            << std::setw(2) << static_cast<int>(uuid[0])
            << std::setw(2) << static_cast<int>(uuid[1])
            << std::setw(2) << static_cast<int>(uuid[2])
            << std::setw(2) << static_cast<int>(uuid[3])
            << "-"
            << std::setw(2) << static_cast<int>(uuid[4])
            << std::setw(2) << static_cast<int>(uuid[5])
            << "-"
            << std::setw(2) << static_cast<int>(uuid[6])
            << std::setw(2) << static_cast<int>(uuid[7])
            << "-"
            << std::setw(2) << static_cast<int>(uuid[8])
            << std::setw(2) << static_cast<int>(uuid[9])
            << "-"
            << std::setw(2) << static_cast<int>(uuid[10])
            << std::setw(2) << static_cast<int>(uuid[11])
            << std::setw(2) << static_cast<int>(uuid[12])
            << std::setw(2) << static_cast<int>(uuid[13])
            << std::setw(2) << static_cast<int>(uuid[14])
            << std::setw(2) << static_cast<int>(uuid[15]);

        return std::move(oss.str());
    }
};
