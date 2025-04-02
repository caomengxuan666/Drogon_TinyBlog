from flask import Flask, request, jsonify
import subprocess
from threading import Thread
import os
import signal
import git  # 导入 GitPython

app = Flask(__name__)

# 定义命令路径和工作目录
WORK_DIR = "/home/cmx/codespace/tiny_blog/web"
GIT_DIR = "/home/cmx/codespace/tiny_blog"
CONFIGURE_CMD = [
    "/usr/bin/cmake",
    "-DCMAKE_BUILD_TYPE:STRING=Release",
    "-DCMAKE_BUILD_PARALLEL_LEVEL:STRING=2",
    "-DCMAKE_TOOLCHAIN_FILE:STRING=/home/cmx/vcpkg/scripts/buildsystems/vcpkg.cmake",
    "-DVCPKG_TARGET_TRIPLET:STRING=x64-linux",
    "-DCMAKE_PREFIX_PATH:STRING=/home/cmx/vcpkg/installed/x64-linux",
    "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE",
    "-DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc",
    "-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++",
    "--no-warn-unused-cli",
    "-S/home/cmx/codespace/tiny_blog/web",
    "-B/home/cmx/codespace/tiny_blog/web/build",
    "-G", "Ninja"
]

BUILD_CMD = [
    "/usr/bin/cmake",
    "--build", "/home/cmx/codespace/tiny_blog/web/build",
    "--config", "Release",
    "--target", "all",
    "-j2",
    "--"
]

# 全局变量存储服务进程的 PID
SERVICE_PID = None

def start_service():
    """在后台启动服务"""
    global SERVICE_PID
    try:
        # 切换到工作目录
        os.chdir(WORK_DIR)

        # 启动目标程序并重定向输出
        with open("log.out", "w") as stdout_file, open("error.log", "w") as stderr_file:
            process = subprocess.Popen(
                ["/home/cmx/codespace/tiny_blog/web/build/web"],
                stdout=stdout_file,
                stderr=stderr_file,
                env=os.environ.copy()  # 确保传递完整的环境变量
            )
            SERVICE_PID = process.pid
            print(f"Started service with PID: {SERVICE_PID}")

    except Exception as e:
        print(f"Failed to start service: {str(e)}")


def stop_service():
    """停止正在运行的服务"""
    global SERVICE_PID
    if SERVICE_PID is not None:
        try:
            os.kill(SERVICE_PID, signal.SIGTERM)
            print(f"Service with PID {SERVICE_PID} has been terminated.")
            SERVICE_PID = None
            return True
        except ProcessLookupError:
            print(f"Process with PID {SERVICE_PID} not found.")
            return False
    else:
        print("No service is currently running.")
        return False


def update_git():
    """使用 GitPython 更新仓库"""
    try:
        # 打开 Git 仓库
        repo = git.Repo(GIT_DIR)

        # 拉取最新代码
        pull_result = repo.remotes.origin.pull()

        # 构造返回结果
        messages = []
        for info in pull_result:
            messages.append(f"Updated branch: {info.ref.name}, updated to commit: {info.commit.hexsha}")

        return {
            "message": "Git pull completed successfully",
            "details": messages
        }

    except git.exc.GitCommandError as e:
        return {
            "message": "Git pull failed",
            "error": str(e)
        }


@app.route('/execute', methods=['POST'])
def execute_command():
    """处理 POST 请求并执行对应的操作"""
    data = request.json
    action = data.get("action")

    try:
        if not action:
            raise ValueError("Action is required in the request body.")

        print(f"Received request: {request.json}")
        print(f"Executing action: {action}")

        if action == "configure":
            print("Running configure command...")
            result = subprocess.run(CONFIGURE_CMD, cwd=WORK_DIR, capture_output=True, text=True)
            message = f"Configure completed with return code: {result.returncode}"
        elif action == "build":
            print("Running build command...")
            result = subprocess.run(BUILD_CMD, cwd=WORK_DIR, capture_output=True, text=True)
            message = f"Build completed with return code: {result.returncode}"
        elif action == "run":
            print("Starting service in background...")
            # 启动后台线程运行服务
            Thread(target=start_service).start()
            return jsonify({"message": "Service started in background"}), 200
        elif action == "close":
            print("Stopping service...")
            success = stop_service()
            if success:
                return jsonify({"message": "Service stopped successfully"}), 200
            else:
                return jsonify({"message": "No service running to stop"}), 400
        elif action == "update":
            print("Updating git...")
            result = update_git()
            return jsonify(result), 200
        else:
            return jsonify({"error": "Invalid action"}), 400

        # 打印命令执行结果
        print(f"Command executed with return code: {result.returncode}")
        print(f"STDOUT:\n{result.stdout}")
        print(f"STDERR:\n{result.stderr}")

        # 返回执行结果
        return jsonify({
            "stdout": result.stdout,
            "stderr": result.stderr,
            "returncode": result.returncode,
            "message": message  # 添加 message 字段
        }), 200

    except Exception as e:
        error_message = f"Error occurred: {str(e)}"
        print(error_message)
        return jsonify({"error": error_message}), 500


if __name__ == '__main__':
    print("Starting server on 0.0.0.0:8888...")
    app.run(host='0.0.0.0', port=8888)