import subprocess
import sys

def run_command(command):
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    stdout, stderr = process.communicate()
    if process.returncode != 0:
        print(f"Error: {stderr.decode('utf-8')}")
    else:
        print(stdout.decode('utf-8'))

def main():
    isProxy = True  # 这里可以设置为False不使用代理的
    update_server = True  # 设置为True来更新服务器

    if isProxy:
        run_command("gitproxy set")

    run_command("git add .")
    run_command('git commit -m "update: Automated commit"')
    run_command("git push")

    if update_server:
        run_command("python client.py update")

if __name__ == "__main__":
    main()