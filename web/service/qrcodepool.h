#pragma once

#include "../service/qrcode.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h> // Windows 多进程 API
#else
#include <unistd.h>  // POSIX fork, pipe
#include <sys/wait.h>// POSIX wait
#endif

class QRCodeMultiProcessGenerator {
public:
    // 构造函数：初始化进程池
    QRCodeMultiProcessGenerator(int pool_size);

    // 析构函数：清理进程池
    ~QRCodeMultiProcessGenerator();

    // 分配任务并生成 QR 码
    std::string generate_qr_code(const std::string &data, const std::string &save_path,
                                 int version, int box_size, int border);

private:
#ifdef _WIN32
    std::vector<HANDLE> processes; // Windows 子进程句柄
    std::vector<HANDLE> read_handles; // 读取管道句柄
    std::vector<HANDLE> write_handles; // 写入管道句柄
#else
    std::vector<pid_t> processes; // POSIX 子进程 ID
    std::vector<int> read_fds;    // 读取管道文件描述符
    std::vector<int> write_fds;   // 写入管道文件描述符
#endif
    int pool_size;                // 进程池大小
    std::mutex pool_mutex;        // 保护进程池的互斥锁
    std::condition_variable pool_cv; // 条件变量
    std::queue<int> available_processes; // 可用进程队列

    // 启动子进程
    void start_process();
};

inline QRCodeMultiProcessGenerator::QRCodeMultiProcessGenerator(int pool_size)
    : pool_size(pool_size) {
    for (int i = 0; i < pool_size; ++i) {
        start_process();
    }
}

inline QRCodeMultiProcessGenerator::~QRCodeMultiProcessGenerator() {
#ifdef _WIN32
    for (size_t i = 0; i < processes.size(); ++i) {
        TerminateProcess(processes[i], 0); // 强制终止子进程
        CloseHandle(processes[i]);
        CloseHandle(read_handles[i]);
        CloseHandle(write_handles[i]);
    }
#else
    for (size_t i = 0; i < processes.size(); ++i) {
        kill(processes[i], SIGTERM); // 发送终止信号给子进程
        close(read_fds[i]);
        close(write_fds[i]);
    }
#endif
}

inline void QRCodeMultiProcessGenerator::start_process() {
#ifdef _WIN32
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStd_IN_Rd, hChildStd_IN_Wr;
    HANDLE hChildStd_OUT_Rd, hChildStd_OUT_Wr;

    // 创建匿名管道
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) {
        std::cerr << "Error creating input pipe!" << std::endl;
        return;
    }
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        std::cerr << "Error creating output pipe!" << std::endl;
        return;
    }

    // 设置子进程的输入输出
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = hChildStd_IN_Rd;
    si.hStdOutput = hChildStd_OUT_Wr;

    // 创建子进程
    if (!CreateProcess(
            NULL,                              // 不指定模块名
            const_cast<char*>("QRCodeWorker"), // 假设有一个独立的可执行文件
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to create process!" << std::endl;
        return;
    }

    processes.push_back(pi.hProcess);
    read_handles.push_back(hChildStd_OUT_Rd);
    write_handles.push_back(hChildStd_IN_Wr);

    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);
#else
    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        std::cerr << "Error creating pipes!" << std::endl;
        return;
    }

    pid_t pid = fork(); // 创建子进程

    if (pid == 0) { // 子进程
        close(pipe_in[1]);  // 关闭写端
        close(pipe_out[0]); // 关闭读端

        // 从管道读取任务
        char buffer[1024];
        while (true) {
            ssize_t bytes_read = read(pipe_in[0], buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) break;

            buffer[bytes_read] = '\0';
            std::string task(buffer);

            // 解析任务参数
            std::istringstream iss(task);
            std::string data, save_path;
            int version, box_size, border;
            iss >> data >> save_path >> version >> box_size >> border;

            // 生成 QR 码
            QRCodeGenerator qr_gen;
            std::ostringstream oss;
            oss << save_path << "/qr_" << time(nullptr) << ".png";
            std::string file_path = oss.str();

            qr_gen.generate_qr_code(save_path, data, version, box_size, border);

            // 返回结果
            std::string result = file_path + "\n";
            write(pipe_out[1], result.c_str(), result.size());
        }

        close(pipe_in[0]);
        close(pipe_out[1]);
        return;
    } else if (pid < 0) { // fork 失败
        std::cerr << "Failed to fork process!" << std::endl;
        return;
    }

    processes.push_back(pid);
    read_fds.push_back(pipe_out[0]);
    write_fds.push_back(pipe_in[1]);

    close(pipe_in[0]);
    close(pipe_out[1]);
#endif

    available_processes.push(processes.size() - 1); // 将新进程加入可用队列
}

inline std::string QRCodeMultiProcessGenerator::generate_qr_code(const std::string &data, const std::string &save_path,
                                                          int version, int box_size, int border) {
    std::unique_lock<std::mutex> lock(pool_mutex);
    pool_cv.wait(lock, [this]() { return !available_processes.empty(); });

    int process_index = available_processes.front();
    available_processes.pop();

    lock.unlock();

    // 构造任务字符串
    std::ostringstream task_stream;
    task_stream << data << " " << save_path << " " << version << " " << box_size << " " << border << "\n";
    std::string task = task_stream.str();

#ifdef _WIN32
    // 向子进程发送任务
    DWORD bytes_written;
    WriteFile(write_handles[process_index], task.c_str(), task.size(), &bytes_written, NULL);

    // 从子进程读取结果
    char buffer[1024];
    DWORD bytes_read;
    ReadFile(read_handles[process_index], buffer, sizeof(buffer) - 1, &bytes_read, NULL);
    buffer[bytes_read] = '\0';
    std::string result(buffer);
#else
    // 向子进程发送任务
    write(write_fds[process_index], task.c_str(), task.size());

    // 从子进程读取结果
    char buffer[1024];
    ssize_t bytes_read = read(read_fds[process_index], buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';
    std::string result(buffer);
#endif

    lock.lock();
    available_processes.push(process_index); // 释放子进程回池
    pool_cv.notify_one();
    lock.unlock();

    return result;
}