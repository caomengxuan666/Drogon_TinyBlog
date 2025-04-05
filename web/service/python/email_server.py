import os
import yaml
import asyncio
from email.mime.text import MIMEText
from email.header import Header
import aiosmtplib
import sys
from collections import deque
from pathlib import Path

# Windows/Linux兼容性处理
if sys.platform == 'win32':
    asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
else:
    try:
        import uvloop
        uvloop.install()
    except ImportError:
        pass


class EmailServer:
    def __init__(self, config_path=None, pool_size=5):
        """
        初始化邮件服务器类。
        :param config_path: 配置文件路径，默认为None。如果为None，则尝试从默认位置加载。
        :param pool_size: 连接池大小，默认为5。
        """
        # 默认配置值
        self.from_addr = None
        self.password = None
        self.smtp_server = None
        self.smtp_port = None

        # 动态解析配置文件路径
        resolved_config_path = self._resolve_config_path(config_path)
        if resolved_config_path:
            try:
                with open(resolved_config_path, 'r') as f:
                    config = yaml.safe_load(f)
                self.from_addr = config['email-server']['sender']['address']
                self.password = config['email-server']['sender']['password']
                self.smtp_server = config['email-server']['server']
                self.smtp_port = config['email-server']['port']
                print(f"成功加载配置文件: {resolved_config_path}")
            except Exception as e:
                raise ValueError(f"加载配置文件失败: {e}")
        else:
            raise FileNotFoundError("无法找到配置文件。请确保 email.yaml 存在于默认路径或通过参数指定路径。")

        # 初始化连接池
        self.pool_size = pool_size
        self.pool = deque(maxlen=pool_size)
        self.lock = asyncio.Lock()
        
    def _resolve_config_path(self, config_path):
        """
        动态解析配置文件路径。
        :param config_path: 用户提供的配置文件路径。
        :return: 返回有效的配置文件路径，如果找不到则返回 None。
        """
        if config_path and os.path.isfile(config_path):
            return config_path  # 使用用户提供的路径

        # 尝试从当前工作目录加载
        current_dir_config = os.path.join(os.getcwd(), "email.yaml")
        if os.path.isfile(current_dir_config):
            return current_dir_config

        # 尝试从脚本所在目录加载
        script_dir = Path(__file__).parent
        script_dir_config = script_dir / "email.yaml"
        if script_dir_config.is_file():
            return str(script_dir_config)

        # 尝试从可执行文件所在目录加载（适用于打包场景）
        executable_dir = Path(os.path.dirname(os.path.abspath(sys.argv[0])))
        executable_dir_config = executable_dir / "email.yaml"
        if executable_dir_config.is_file():
            return str(executable_dir_config)

        # 所有路径均无效
        return None


    async def _create_connection(self):
        """
        创建一个新的SMTP连接。
        :return: 返回一个已登录的SMTP客户端实例。
        """
        client = aiosmtplib.SMTP(
            hostname=self.smtp_server,
            port=self.smtp_port,
            use_tls=True
        )
        await client.connect()
        await client.login(self.from_addr, self.password)
        return client

    async def get_connection(self):
        """
        从连接池中获取一个连接。如果池为空，则创建新的连接。
        :return: 返回一个SMTP客户端实例。
        """
        async with self.lock:
            if self.pool:
                return self.pool.pop()  # 从池中取出一个连接
            return await self._create_connection()  # 创建新连接

    async def release_connection(self, client):
        """
        将连接释放回连接池。如果池已满，则关闭连接。
        :param client: SMTP客户端实例。
        """
        async with self.lock:
            if len(self.pool) < self.pool_size:
                self.pool.append(client)  # 将连接放回池中
            else:
                await client.quit()  # 关闭多余连接

    def set_email_content(self, content, msg_type='html'):
        """
        设置邮件内容。
        :param content: 邮件正文内容。
        :param msg_type: 内容类型（'plain' 或 'html'）。
        """
        self.email_content = MIMEText(content, msg_type, 'utf-8')

    def set_email_title(self, title):
        """
        设置邮件标题。
        :param title: 邮件标题。
        """
        self.email_title = title

    async def send_email(self, recv, content, title):
        """
        异步发送邮件。
        :param recv: 收件人邮箱地址。
        :param content: 邮件正文内容。
        :param title: 邮件标题。
        """
        # 检查是否设置了必要的字段
        if not all([self.from_addr, self.password, self.smtp_server, self.smtp_port]):
            raise ValueError("缺少必要配置字段 (from_addr, password, smtp_server, smtp_port)。")

        # 构建邮件内容
        self.set_email_content(content)
        self.set_email_title(title)

        msg = self.email_content
        msg['From'] = self.from_addr  # 发件人地址
        msg['To'] = recv  # 收件人地址
        msg['Subject'] = Header(title, 'utf-8')  # 添加编码参数

        smtp_client = None  # 初始化变量，防止未定义错误
        try:
            # 从连接池中获取一个连接
            smtp_client = await self.get_connection()
            await smtp_client.sendmail(self.from_addr, [recv], msg.as_string())
            print("邮件发送成功！")
        except aiosmtplib.SMTPAuthenticationError as e:
            print(f"认证失败错误: {e}")
            raise RuntimeError("SMTP 认证失败，请检查用户名、密码和服务器配置。")  # 抛出异常终止流程
        except Exception as e:
            print(f"其他错误: {str(e)}")
            raise RuntimeError(f"邮件发送失败: {str(e)}")  # 抛出异常终止流程
        finally:
            # 确保在任何情况下都能正确释放连接
            if smtp_client:
                await self.release_connection(smtp_client)


# 示例用法
if __name__ == "__main__":
    async def main():
        # 自定义配置文件路径
        config_path = "../email.yaml"
        email_server = EmailServer(config_path=config_path, pool_size=5)

        # 发送测试邮件
        await email_server.send_email(
            recv="2507560089@qq.com",
            content="测试邮件发送功能。",
            title="测试邮件"
        )

    asyncio.run(main())