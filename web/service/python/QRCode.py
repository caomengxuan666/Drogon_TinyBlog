import os
import time
import qrcode

class QRCodeGenerator:
    def generate_qr_code(self, save_path: str, data: str, version: int, box_size: int, border: int):
        # 创建目录（如果不存在）
        os.makedirs(save_path, exist_ok=True)

        # 创建 QRCode 对象
        qr = qrcode.QRCode(
            version=version,
            error_correction=qrcode.constants.ERROR_CORRECT_H,
            box_size=box_size,
            border=border
        )
        qr.add_data(data)
        img = qr.make_image(fill_color='green', back_color='white')

        # 使用时间戳命名文件
        filename = f"qr_{int(time.time_ns())}.png"
        file_path = os.path.join(save_path, filename)
        img.save(file_path, optimize=True, compress_level=1)
