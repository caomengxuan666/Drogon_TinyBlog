#include "../service/urlencode.h"
#include "qrcodepool.h"
int muilti_process_generate_qrcode() {
    try {
        QRCodeMultiProcessGenerator generator(4);// 初始化进程池，大小为 4

        // 模拟多次生成 QR 码
        for (int i = 0; i < 10; ++i) {
            std::string data = "https://example.com/" + std::to_string(i);
            std::string save_path = "code_images";
            std::string file_name = "qr_code_" + std::to_string(i) + ".png";

            std::string file_path = generator.generate_qr_code_with_filename(data, save_path, file_name, 3, 15, 2);
            std::cout << "QR code saved to: " << file_path << std::endl;
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
        std::string save_path = "code_images";
        std::string file_name = "github_page.png";

        std::string file_path = generator.generate_qr_code_with_filename("https://github.com/caomengxuan666", save_path, file_name, 3, 15, 2);
        std::cout << "QR code saved to: " << file_path << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void single_code() {
    QRCodeGenerator qr_gen;
    std::string data = "http://47.120.50.122:5000";
    std::string save_path = "code_images";
    std::string file_name = "single_code.png";

    qr_gen.generate_qr_code_with_filename(save_path, file_name, data, 3, 15, 2);
}

void generateQRcodeWithFunc() {
    std::string filepath = "code_images";
    std::string filename = "UrlEncode.png";
    std::string url_data = "https://47.120.50.122/";
    UrlEncode2QRCode(filepath, filename, url_data);
}

int main() {
    //single_process_example();
    //single_code();
    generateQRcodeWithFunc();
    return 0;
}