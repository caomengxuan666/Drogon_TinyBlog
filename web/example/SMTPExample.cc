#include "../service/smtpsender.h"
#include <iostream>

int main() {
    SMTPSender<RegisSender> email_sender;
    email_sender.sendEmail("2507560089@qq.com", "114514", [](const std::string &str) {
        std::cout << "Callback received: " << str << std::endl; // 明确输出内容
    });
}