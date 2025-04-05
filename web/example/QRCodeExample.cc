#include "qrcodepool.h"

int muilti_process_generate_qrcode() {
    try {
        QRCodeMultiProcessGenerator generator(4);// 初始化进程池，大小为 4

        // 模拟多次生成 QR 码
        for (int i = 0; i < 10; ++i) {
            std::string data = "https://example.com/" + std::to_string(i);
            std::string save_path = "code_images";

            std::string file_path = generator.generate_qr_code(data, save_path, 3, 15, 2);
            std::cout << "QR code saved to: " << file_path;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int single_process_example() {
    try {
        QRCodeMultiProcessGenerator generator(1);
        generator.generate_qr_code("https://github.com/caomengxuan666", "github_page", 3, 15, 2);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void single_code() {
    QRCodeGenerator qr_gen;
    std::string data = "https://www.baidu.com/";
    qr_gen.generate_qr_code(
            "code_images",// 保存路径
            data,         // 数据内容
            3,            // 版本
            15,           // 模块大小
            2             // 边框宽度
    );
}

int main() {
    single_process_example();
    single_code();
    return 0;
}