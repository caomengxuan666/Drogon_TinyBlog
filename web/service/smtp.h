#pragma once
#include <fstream>
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <stdexcept> // 标准异常库

#ifdef _WIN32
#include <windows.h> // Windows API
#else
#include <unistd.h>  // readlink
#include <limits.h>  // PATH_MAX
#endif

namespace py = pybind11;

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

// 自定义异常类
class EmailException : public std::runtime_error {
public:
    explicit EmailException(const std::string &message) : std::runtime_error(message) {}
};

// 全局管理 Python 解释器的初始化和销毁
class PyInterpreterManager {
public:
    PyInterpreterManager() {
        py::initialize_interpreter();
    }

    ~PyInterpreterManager() {
        py::finalize_interpreter();
    }
};

// 获取可执行文件所在目录（跨平台实现）
inline std::string get_executable_directory() {
#ifdef _WIN32
    // Windows: 使用 GetModuleFileName 获取可执行文件路径
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);

    // 提取目录部分
    size_t last_slash = path.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        return path.substr(0, last_slash);
    }
    return "";
#else
    // Linux: 使用 /proc/self/exe 和 readlink 获取可执行文件路径
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        std::string path(buffer);

        // 提取目录部分
        size_t last_slash = path.find_last_of('/');
        if (last_slash != std::string::npos) {
            return path.substr(0, last_slash);
        }
    }
    return "";
#endif
}

// 封装 EmailServer 类的功能
class DLL_EXPORT EmailWrapper {
public:
    /**
     * 构造函数。
     * @param config_path 配置文件路径（可选）。
     * @param pool_size 连接池大小（可选）。
     */
    EmailWrapper(const std::string &config_path = "", int pool_size = 5) {
        try {
            // 导入 Python 模块
            email_module = py::module_::import("email_server");

            // 动态解析配置文件路径
            std::string resolved_config_path = !config_path.empty() ? config_path : get_executable_directory() + "/email.yaml";

            // 创建 EmailServer 实例，支持传递 config_path 和 pool_size 参数
            email_server = email_module.attr("EmailServer")(py::arg("config_path") = resolved_config_path, py::arg("pool_size") = pool_size);
        } catch (const py::error_already_set &e) {
            throw EmailException("Python 初始化失败: " + std::string(e.what()));
        } catch (const std::exception &e) {
            throw EmailException("C++ 初始化失败: " + std::string(e.what()));
        }
    }

    // 调用 send_email 方法
    void send_email(const std::string &recv, const std::string &content, const std::string &title) {
        try {
            py::object asyncio = py::module_::import("asyncio");
            py::object send_email_coroutine = email_server.attr("send_email")(recv, content, title);

            asyncio.attr("run")(send_email_coroutine);
        } catch (const py::error_already_set &e) {
            throw EmailException("Python 错误: " + std::string(e.what()));
        } catch (const std::exception &e) {
            throw EmailException("C++ 错误: " + std::string(e.what()));
        }
    }

    // 加载 HTML 文件内容
    std::string load_html_content(const std::string &html_file_path) {
        std::ifstream file(html_file_path);
        if (!file.is_open()) {
            throw EmailException("无法打开 HTML 文件: " + html_file_path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // 设置硬编码的 HTML 内容
    std::string set_html_content(const std::string &html_content) {
        return html_content; // 直接返回硬编码的 HTML 内容
    }

private:
    py::object email_module; // 保存对 Python 模块的引用
    py::object email_server; // 存储 Python 对象
};

PYBIND11_MODULE(email_wrapper, m) {
    py::class_<EmailWrapper>(m, "EmailWrapper")
            .def(py::init<const std::string &, int>(), py::arg("config_path") = "", py::arg("pool_size") = 5) // 构造函数
            .def("send_email", &EmailWrapper::send_email)              // 暴露 send_email 方法
            .def("load_html_content", &EmailWrapper::load_html_content)// 暴露 load_html_content 方法
            .def("set_html_content", &EmailWrapper::set_html_content); // 暴露 set_html_content 方法
}

// 全局管理 Python 解释器
static PyInterpreterManager interpreter_manager;