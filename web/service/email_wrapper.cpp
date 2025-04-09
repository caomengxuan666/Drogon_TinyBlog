#include "smtp.h"

PYBIND11_MODULE(email_wrapper, m) {
    py::class_<EmailWrapper>(m, "EmailWrapper")
            .def(py::init<const std::string &, int>(), py::arg("config_path") = "", py::arg("pool_size") = 5) // 构造函数
            .def("send_email", &EmailWrapper::send_email)              // 暴露 send_email 方法
            .def("load_html_content", &EmailWrapper::load_html_content)// 暴露 load_html_content 方法
            .def("set_html_content", &EmailWrapper::set_html_content); // 暴露 set_html_content 方法
}