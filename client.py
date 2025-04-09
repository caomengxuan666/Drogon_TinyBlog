import requests
import sys
from colorama import Fore, Style, init
import os

# 初始化 colorama
init(autoreset=True)

SERVER_URL = "http://47.120.50.122:8888/execute"

def download_db():
    """从服务器下载数据库文件"""
    download_url = "http://47.120.50.122:8888/download_db"
    save_path = "./TinyBlog.db"  # 保存到当前目录下的 TinyBlog.db 文件

    try:
        print(f"{Fore.CYAN}Downloading database file from server...{Style.RESET_ALL}")
        response = requests.get(download_url, stream=True)
        
        if response.status_code == 200:
            # 确保目标路径的父目录存在
            os.makedirs(os.path.dirname(save_path), exist_ok=True)

            # 将文件写入本地
            with open(save_path, 'wb') as file:
                for chunk in response.iter_content(chunk_size=8192):
                    file.write(chunk)
            
            print(f"{Fore.GREEN}Database file downloaded successfully: {save_path}{Style.RESET_ALL}")
        else:
            print(f"{Fore.RED}Error downloading file: {response.status_code}, {response.text}{Style.RESET_ALL}")
    
    except Exception as e:
        print(f"{Fore.RED}Error occurred while downloading file: {str(e)}{Style.RESET_ALL}")

def send_request(action):
    payload = {"action": action}
    response = requests.post(SERVER_URL, json=payload)
    if response.status_code == 200:
        data = response.json()
        
        # 打印 Message
        print(f"{Fore.CYAN}Message:{Style.RESET_ALL} {data.get('message', '')}")
        
        # 打印 STDOUT（绿色标题和内容）
        stdout = data.get('stdout', '')
        if stdout:
            print(f"{Fore.GREEN}=== STDOUT ==={Style.RESET_ALL}")
            # 将每一行的 stdout 内容设置为绿色
            for line in stdout.splitlines():
                print(f"{Fore.GREEN}{line}{Style.RESET_ALL}")
        
        # 打印 STDERR（红色）
        stderr = data.get('stderr', '')
        if stderr:
            print(f"{Fore.RED}=== STDERR ==={Style.RESET_ALL}")
            print(stderr)
        
        # 打印 Return Code
        print(f"{Fore.BLUE}Return Code:{Style.RESET_ALL} {data.get('returncode', 'N/A')}")
    else:
        print(f"{Fore.RED}Error:{Style.RESET_ALL} {response.status_code}, {response.text}")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python client.py <action>")
        print("Actions: configure, build, run, close, update, download_db")
        sys.exit(1)

    action = sys.argv[1]
    if action not in ["configure", "build", "run", "close", "update", "download_db"]:
        print("Invalid action. Choose from 'configure', 'build', 'run', 'update', 'close', or 'download_db'.")
        sys.exit(1)

    if action == "download_db":
        download_db()
    else:
        send_request(action)