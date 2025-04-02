import sys
import requests
import threading
import time
import os
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QTextEdit, QPushButton, QVBoxLayout, QHBoxLayout,
    QWidget, QLabel, QScrollArea, QMessageBox
)
from PyQt5.QtCore import Qt, pyqtSignal
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from git import Repo

# 服务器 URL
SERVER_URL = "http://47.120.50.122:8888/execute"

# 文件监控目录
LOCAL_DIR = "/home/cmx/codespace/tiny_blog"


class FileChangeHandler(FileSystemEventHandler):
    """处理本地文件变化"""
    def __init__(self, update_callback):
        self.update_callback = update_callback

    def on_modified(self, event):
        if not event.is_directory:
            self.update_callback(f"本地文件已修改: {event.src_path}")

    def on_created(self, event):
        if not event.is_directory:
            self.update_callback(f"本地文件已创建: {event.src_path}")

    def on_deleted(self, event):
        if not event.is_directory:
            self.update_callback(f"本地文件已删除: {event.src_path}")


class ClientApp(QMainWindow):
    # 定义信号
    log_update_signal = pyqtSignal(str)
    update_status_signal = pyqtSignal(str)

    def __init__(self):
        super().__init__()
        self.setWindowTitle("服务器与客户端同步工具")
        self.setGeometry(100, 100, 1000, 700)  # 调整窗口大小

        # 初始化 Git 仓库
        self.repo = Repo(LOCAL_DIR)

        # 初始化界面
        self.init_ui()

        # 连接信号到槽函数
        self.log_update_signal.connect(self.update_log_text)
        self.update_status_signal.connect(self.update_status_label)

        # 启动文件监控
        self.start_file_monitoring()

        # 启动后台线程以轮询服务器状态
        threading.Thread(target=self.poll_server_status, daemon=True).start()

    def init_ui(self):
        """初始化 PyQt 界面"""
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        main_layout = QVBoxLayout(main_widget)

        # 标题
        title_label = QLabel("服务器与客户端同步工具", self)
        title_label.setAlignment(Qt.AlignCenter)
        title_label.setStyleSheet("""
            font-size: 24px;
            font-weight: bold;
            color: #333;
            margin-bottom: 20px;
        """)
        main_layout.addWidget(title_label)

        # 状态栏
        self.status_label = QLabel("等待操作...", self)
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("""
            font-size: 16px;
            color: #007BFF;
            margin-bottom: 10px;
        """)
        main_layout.addWidget(self.status_label)

        # 日志显示区域
        log_scroll_area = QScrollArea()
        log_scroll_area.setWidgetResizable(True)
        log_container = QWidget()
        self.log_text = QTextEdit(log_container)
        self.log_text.setReadOnly(True)
        self.log_text.setStyleSheet("""
            background-color: #f9f9f9;
            border: 1px solid #ddd;
            font-family: 'Courier New', monospace;
            font-size: 14px;
            padding: 10px;
        """)
        log_layout = QVBoxLayout(log_container)
        log_layout.addWidget(self.log_text)
        log_scroll_area.setWidget(log_container)
        main_layout.addWidget(log_scroll_area)

        # 按钮区域
        button_layout = QHBoxLayout()
        actions = ["配置", "构建", "运行", "停止", "更新", "提交差异"]
        for action in actions:
            btn = QPushButton(action, self)
            btn.clicked.connect(lambda _, a=action: self.send_request(a))
            btn.setStyleSheet("""
                QPushButton {
                    background-color: #007BFF;
                    color: white;
                    font-size: 14px;
                    padding: 10px 20px;
                    border: none;
                    border-radius: 5px;
                }
                QPushButton:hover {
                    background-color: #0056b3;
                }
            """)
            button_layout.addWidget(btn)
        main_layout.addLayout(button_layout)

    def send_request(self, action):
        """发送请求到服务器"""
        # 将中文操作名映射为英文
        action_map = {
            "配置": "configure",
            "构建": "build",
            "运行": "run",
            "停止": "close",
            "更新": "update",
            "提交差异": "compare"
        }
        # 检查是否支持该操作
        if action not in action_map:
            self.log_update_signal.emit(f"无效的操作: {action}\n")
            return

        # 获取对应的英文操作名
        english_action = action_map[action]

        try:
            if english_action == "compare":
                # 获取暂存区差异并发送到服务器
                self.send_staged_changes_to_server()
            else:
                # 发送普通请求
                payload = {"action": english_action}
                headers = {"Content-Type": "application/json"}  # 显式设置请求头
                response = requests.post(SERVER_URL, json=payload, headers=headers, timeout=10)
                print(f"Server response: {response.status_code}, {response.text}")

                if response.status_code == 200:
                    data = response.json()
                    self.log_update_signal.emit(f"服务器响应 ({action}):\n")
                    for key, value in data.items():
                        self.log_update_signal.emit(f"{key}: {value}")
                    self.log_update_signal.emit("\n")

                    # 如果包含 diff 信息，则高亮显示
                    if "diff" in data:
                        self.log_update_signal.emit("文件差异:\n")
                        self.log_update_signal.emit(data["diff"])
                else:
                    self.log_update_signal.emit(f"服务器错误: {response.status_code}, {response.text}\n\n")
        except Exception as e:
            self.log_update_signal.emit(f"请求失败: {str(e)}\n\n")

    def send_staged_changes_to_server(self):
        """获取暂存区的差异并发送到服务器"""
        try:
            staged_files = self.get_staged_changes()
            files_data = {}

            for file_path in staged_files:
                full_path = os.path.join(LOCAL_DIR, file_path)
                if os.path.exists(full_path):  # 如果文件存在（未被删除）
                    with open(full_path, "r") as f:
                        files_data[file_path] = f.read()
                else:
                    files_data[file_path] = None  # 标记为已删除

            payload = {"action": "compare", "files": files_data}
            response = requests.post(SERVER_URL, json=payload, timeout=10)

            if response.status_code == 200:
                data = response.json()
                self.log_update_signal.emit("文件差异:\n")
                for file, diff in data.get("diffs", {}).items():
                    self.log_update_signal.emit(f"{file}:\n{diff}\n")
            else:
                self.log_update_signal.emit(f"服务器错误: {response.status_code}, {response.text}\n\n")
        except Exception as e:
            self.log_update_signal.emit(f"请求失败: {str(e)}\n\n")

    def get_staged_changes(self):
        """获取 Git 暂存区的差异文件"""
        staged_files = []
        for diff in self.repo.index.diff("HEAD"):
            if diff.change_type in ("A", "M", "D"):  # 添加、修改、删除
                staged_files.append(diff.a_path)
        return staged_files

    def start_file_monitoring(self):
        """启动本地文件监控"""
        event_handler = FileChangeHandler(self.update_log)
        observer = Observer()
        observer.schedule(event_handler, path=LOCAL_DIR, recursive=True)
        observer.start()
        threading.Thread(target=observer.join, daemon=True).start()
        self.update_log("本地文件监控已启动。\n")

    def poll_server_status(self):
        """轮询服务器状态"""
        while True:
            try:
                response = requests.post(SERVER_URL, json={"action": "update"}, timeout=10)  # 设置超时时间为 10 秒
                if response.status_code == 200:
                    data = response.json()
                    commit_info = data.get("details", ["未知提交"])[0].split("commit: ")[-1]
                    self.update_status_signal.emit(f"远程仓库已更新至提交: {commit_info}")
                else:
                    self.update_status_signal.emit("无法连接到服务器")
            except Exception as e:
                self.update_status_signal.emit(f"未连接到服务器: {str(e)}")
            time.sleep(10)  # 每 10 秒轮询一次

    def update_log(self, message):
        """更新日志显示"""
        self.log_update_signal.emit(message.strip())

    def update_log_text(self, message):
        """更新日志文本框"""
        self.log_text.append(message)
        self.log_text.verticalScrollBar().setValue(self.log_text.verticalScrollBar().maximum())

    def update_status_label(self, message):
        """更新状态栏标签"""
        self.status_label.setText(message)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = ClientApp()
    window.show()
    sys.exit(app.exec_())