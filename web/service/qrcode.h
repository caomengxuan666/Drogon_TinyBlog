#pragma once
#include <pybind11/embed.h> // Pybind11 嵌入 Python 解释器
#include <pybind11/stl.h>   // 支持 STL 容器与 Python 的转换
#include <string>
#include <iostream>

namespace py = pybind11;

class QRCodeGenerator {
public:
    void generate_qr_code(const std::string& save_path, const std::string& data,
                          int version, int box_size, int border) {
        // 初始化 Python 解释器
        py::scoped_interpreter guard{};

        try {
            // 导入所需的 Python 模块
            py::module os = py::module::import("os");
            py::module time = py::module::import("time");
            py::module qrcode = py::module::import("qrcode");

            // 创建目录（如果不存在）
            os.attr("makedirs")(save_path, py::arg("exist_ok") = true);

            // 创建 QRCode 对象
            py::object qr_class = qrcode.attr("QRCode");
            py::object qr = qr_class(
                py::arg("version") = version,
                py::arg("error_correction") = qrcode.attr("constants").attr("ERROR_CORRECT_H"),
                py::arg("box_size") = box_size,
                py::arg("border") = border
            );
            qr.attr("add_data")(data);
            py::object img = qr.attr("make_image")(py::arg("fill_color") = "green", py::arg("back_color") = "white");

            // 使用时间戳命名文件
            long timestamp = time.attr("time_ns")().cast<long>();
            std::string filename = "qr_" + std::to_string(timestamp) + ".png";
            std::string file_path = (py::str(os.attr("path").attr("join")(save_path, filename))).cast<std::string>();

            // 保存图像
            img.attr("save")(file_path, py::arg("optimize") = true, py::arg("compress_level") = 1);

            std::cout << "QR code saved to: " << file_path << std::endl;
        } catch (const py::error_already_set& e) {
            // 捕获 Python 异常并打印
            std::cerr << "Python error: " << e.what() << std::endl;
        }
    }
};

// Pybind11 绑定
PYBIND11_MODULE(qrcode_cpp, m) {
    py::class_<QRCodeGenerator>(m, "QRCodeGenerator")
        .def(py::init<>())
        .def("generate_qr_code", &QRCodeGenerator::generate_qr_code,
             py::arg("save_path"), py::arg("data"), py::arg("version"),
             py::arg("box_size"), py::arg("border"),
             "Generate a QR code using Python's qrcode library and save it as a PNG file.");
}