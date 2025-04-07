/**
 * @FilePath     : /tiny_blog/web/service/urlencode.h
 * @Description  :  将url encode成qrcode
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-04-07 22:23:19
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/

#include "qrcode.h"

using refstring=const std::string&;

/**
 * @author       : caomengxuan
 * @brief        : 传入一个url，将其编码为qrcode
 * @param         {refstring} filepath:文件夹输出路径
 * @param         {refstring} filename:文件名
 * @param         {refstring} url_data:要编码的url数据
 * @note         : 这里目前只有单个QRcode url的编码需求，即一个url做一个编码，只用到了QRCodeGenerator,没有使用Pool,这里不需要QRCodePool
 * @return        {*}
**/
inline static const void UrlEncode2QRCode(refstring filepath,refstring filename,refstring url_data) {
    QRCodeGenerator qrcodegenerator;
    qrcodegenerator.generate_qr_code_with_filename(filepath,filename,url_data,10,10,1);
}

