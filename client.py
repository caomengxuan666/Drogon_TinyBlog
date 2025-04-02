import requests
import sys
from colorama import Fore, Style, init

# 初始化 colorama
init(autoreset=True)

SERVER_URL = "http://47.120.50.122:8888/execute"

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
        print("Actions: configure, build, run, close,update")
        sys.exit(1)

    action = sys.argv[1]
    if action not in ["configure", "build", "run", "close","update"]:
        print("Invalid action. Choose from 'configure', 'build', 'run','update' or 'close'.")
        sys.exit(1)

    send_request(action)