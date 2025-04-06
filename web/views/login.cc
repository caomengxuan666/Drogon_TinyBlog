//this file is generated by program(drogon_ctl) automatically,don't modify it!
#include "login.h"
#include <drogon/utils/OStringStream.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <list>
#include <deque>
#include <queue>
using namespace drogon;
std::string login::genText(const DrTemplateData& login_view_data)
{
	drogon::OStringStream login_tmp_stream;
	std::string layoutName{""};
	login_tmp_stream << "<style>\n";
	login_tmp_stream << "    * {\n";
	login_tmp_stream << "        font-family: \"Poppins\", sans-serif;\n";
	login_tmp_stream << "        margin: 0;\n";
	login_tmp_stream << "        padding: 0;\n";
	login_tmp_stream << "        box-sizing: border-box;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    body {\n";
	login_tmp_stream << "        min-height: 100vh;\n";
	login_tmp_stream << "        display: flex;\n";
	login_tmp_stream << "        align-items: center;\n";
	login_tmp_stream << "        justify-content: center;\n";
	login_tmp_stream << "        background: #282a37;\n";
	login_tmp_stream << "        background-size: cover;\n";
	login_tmp_stream << "        background-position: center;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper {\n";
	login_tmp_stream << "        position: relative;\n";
	login_tmp_stream << "        width: 400px;\n";
	login_tmp_stream << "        height: 450px;\n";
	login_tmp_stream << "        background: #3e404d;\n";
	login_tmp_stream << "        border: 2px solid rgba(255, 255, 255, 0.5);\n";
	login_tmp_stream << "        border-radius: 20px;\n";
	login_tmp_stream << "        display: flex;\n";
	login_tmp_stream << "        align-items: center;\n";
	login_tmp_stream << "        justify-content: center;\n";
	login_tmp_stream << "        backdrop-filter: blur(15px);\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper:hover {\n";
	login_tmp_stream << "        box-shadow: 0 0 40px rgba(255, 255, 255, 0.5);\n";
	login_tmp_stream << "        background: #46474e;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper h1 {\n";
	login_tmp_stream << "        font-size: 2em;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        text-align: center;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .input-box {\n";
	login_tmp_stream << "        position: relative;\n";
	login_tmp_stream << "        width: 310px;\n";
	login_tmp_stream << "        margin: 30px 0;\n";
	login_tmp_stream << "        border-bottom: 2px solid #fff;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .input-box input {\n";
	login_tmp_stream << "        width: 100%;\n";
	login_tmp_stream << "        height: 50px;\n";
	login_tmp_stream << "        background: transparent;\n";
	login_tmp_stream << "        outline: none;\n";
	login_tmp_stream << "        border: none;\n";
	login_tmp_stream << "        font-size: 1em;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        padding: 0 40px 0 5px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .input-box label {\n";
	login_tmp_stream << "        position: absolute;\n";
	login_tmp_stream << "        top: 50%;\n";
	login_tmp_stream << "        left: 5px;\n";
	login_tmp_stream << "        transform: translateY(-50%);\n";
	login_tmp_stream << "        font-size: 1em;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        pointer-events: none;\n";
	login_tmp_stream << "        transition: 0.5s;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .input-box input:focus~label,\n";
	login_tmp_stream << "    .wrapper .input-box input:valid~label {\n";
	login_tmp_stream << "        top: -5px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .input-box .icon {\n";
	login_tmp_stream << "        position: absolute;\n";
	login_tmp_stream << "        right: 8px;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        font-size: 1.2em;\n";
	login_tmp_stream << "        line-height: 57px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .row {\n";
	login_tmp_stream << "        margin: -15px 0 15px;\n";
	login_tmp_stream << "        font-size: 0.9em;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        display: flex;\n";
	login_tmp_stream << "        justify-content: space-between;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .row label {\n";
	login_tmp_stream << "        display: flex;\n";
	login_tmp_stream << "        align-items: center;\n";
	login_tmp_stream << "        gap: 5px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .row a {\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        text-decoration: none;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .options a:hover {\n";
	login_tmp_stream << "        text-decoration: underline;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .btn {\n";
	login_tmp_stream << "        width: 100%;\n";
	login_tmp_stream << "        height: 40px;\n";
	login_tmp_stream << "        background: #fff;\n";
	login_tmp_stream << "        outline: none;\n";
	login_tmp_stream << "        border: none;\n";
	login_tmp_stream << "        border-radius: 40px;\n";
	login_tmp_stream << "        cursor: pointer;\n";
	login_tmp_stream << "        font-size: 1em;\n";
	login_tmp_stream << "        font-weight: 500;\n";
	login_tmp_stream << "        color: #000;\n";
	login_tmp_stream << "        margin-top: 10px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .btn:hover {\n";
	login_tmp_stream << "        background: #ffffea;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .signup-link {\n";
	login_tmp_stream << "        font-size: 0.9em;\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        text-align: center;\n";
	login_tmp_stream << "        margin: 25px 0 10px;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .signup-link a {\n";
	login_tmp_stream << "        color: #fff;\n";
	login_tmp_stream << "        text-decoration: none;\n";
	login_tmp_stream << "        font-weight: 600;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    .wrapper .signup-link a:hover {\n";
	login_tmp_stream << "        text-decoration: underline;\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    @media (max-width: 360px) {\n";
	login_tmp_stream << "        .wrapper {\n";
	login_tmp_stream << "            width: 100%;\n";
	login_tmp_stream << "            height: 100vh;\n";
	login_tmp_stream << "            border: none;\n";
	login_tmp_stream << "            border-radius: 0px;\n";
	login_tmp_stream << "        }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "        .wrapper .input-box {\n";
	login_tmp_stream << "            width: 290px;\n";
	login_tmp_stream << "        }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "        #img {\n";
	login_tmp_stream << "            z-index: -90;\n";
	login_tmp_stream << "        }\n";
	login_tmp_stream << "    }\n";
	login_tmp_stream << "</style>\n";
	login_tmp_stream << "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css\" />\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "<!--css file-->\n";
	login_tmp_stream << "<link rel=\"stylesheet\" href=\"styles.css\" />\n";
	login_tmp_stream << "</head>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "<body>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    <div class=\"wrapper\">\n";
	login_tmp_stream << "        <form action=\"#\">\n";
	login_tmp_stream << "            <h1>Login</h1>\n";
	login_tmp_stream << "            <div class=\"input-box\">\n";
	login_tmp_stream << "                <i class=\"fas fa-envelope icon\"></i>\n";
	login_tmp_stream << "                <input id=\"usernameInput\" type=\"username\" required />\n";
	login_tmp_stream << "                <label>Username</label>\n";
	login_tmp_stream << "            </div>\n";
	login_tmp_stream << "            <div class=\"input-box\">\n";
	login_tmp_stream << "                <i class=\"fas fa-lock icon\"></i>\n";
	login_tmp_stream << "                <input id=\"passwordInput\" type=\"password\" required />\n";
	login_tmp_stream << "                <label>Password</label>\n";
	login_tmp_stream << "            </div>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "            <div class=\"row\">\n";
login_tmp_stream<<"\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "                <a onclick=\"forgotman()\">Forgot password?</a>\n";
	login_tmp_stream << "            </div>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "            <button onclick=\"loginBrungle()\" class=\"btn\">Login</button>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "            <div class=\"signup-link\">\n";
	login_tmp_stream << "                <p>Don't have an account? <a href=\"regis\">Create one.</a></p>\n";
	login_tmp_stream << "            </div>\n";
	login_tmp_stream << "        </form>\n";
	login_tmp_stream << "    </div>\n";
	login_tmp_stream << "</body>\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "</html>\n";
	login_tmp_stream << "<script>\n";
	login_tmp_stream << "    function forgotman() {\n";
	login_tmp_stream << "        alert('Please email ssnigdhasiraz22@sirhenryfloyd.co.uk to request a password reset');\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    // user passwords and stuff\n";
login_tmp_stream<<"\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    // Admin\n";
	login_tmp_stream << "    const adminUser = \"Admin\";\n";
	login_tmp_stream << "    const adminPass = \"Pass\"\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "    function loginBrungle() {\n";
	login_tmp_stream << "        var pass = document.querySelector('#passwordInput');\n";
	login_tmp_stream << "        var name = document.querySelector('#usernameInput');\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "        var passValue = pass.value.toLowerCase();\n";
	login_tmp_stream << "        var nameValue = name.value.toLowerCase();\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "        if (passValue === adminUser) {\n";
	login_tmp_stream << "            if (nameValue === \"a\") {\n";
	login_tmp_stream << "                alert('logging');\n";
	login_tmp_stream << "                // add logic here for savig using indexededdb lol porbablcy gonna be lkike function save()\n";
	login_tmp_stream << "            } else {\n";
	login_tmp_stream << "                alert('wrong username');\n";
	login_tmp_stream << "            }\n";
	login_tmp_stream << "        }\n";
	login_tmp_stream << "        // user 2\n";
	login_tmp_stream << "        else if (passValue === \"b\") {\n";
	login_tmp_stream << "            if (nameValue === \"b\") {\n";
	login_tmp_stream << "                alert('logging');\n";
	login_tmp_stream << "            } else {\n";
	login_tmp_stream << "                alert('wrong username');\n";
	login_tmp_stream << "            }\n";
	login_tmp_stream << "        }\n";
	login_tmp_stream << "    }\n";
login_tmp_stream<<"\n";
	login_tmp_stream << "</script>\n";
if(layoutName.empty())
{
std::string ret{std::move(login_tmp_stream.str())};
return ret;
}else
{
auto templ = DrTemplateBase::newTemplate(layoutName);
if(!templ) return "";
HttpViewData data = login_view_data;
auto str = std::move(login_tmp_stream.str());
if(!str.empty() && str[str.length()-1] == '\n') str.resize(str.length()-1);
data[""] = std::move(str);
return templ->genText(data);
}
}
