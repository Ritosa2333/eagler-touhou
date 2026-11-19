"""Interactive local-only configuration. Does not contact the TURN provider."""
import getpass
import json
import re
from turn_service import CONFIG_PATH


def main():
    print('Cloudflare TURN setup / Cloudflare TURN 配置')
    print('请填写 TURN 服务创建的 Key ID 与配套 API Token，不是 Cloudflare 全局 API Key。')
    print('密钥只保存在本机项目根目录，不在 web 目录中，不要发到聊天。')
    if CONFIG_PATH.exists() and input('已有配置，覆盖吗？输入 YES 确认：').strip()!='YES':
        return
    key=input('TURN Key ID: ').strip()
    token=getpass.getpass('TURN API Token（输入不显示）: ').strip()
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,128}',key) or not token or len(token)>4096 or any(ord(c)<33 or ord(c)>126 for c in token):
        raise SystemExit('格式无效，未保存。')
    CONFIG_PATH.write_text(json.dumps({'keyId':key,'apiToken':token},indent=2)+'\n',encoding='utf-8')
    print('已保存。尚未验证账号或网络；重启 start-lan.bat 后用 TURN 测试链接建房。')


if __name__=='__main__':
    try:main()
    except (KeyboardInterrupt,EOFError):print('\n已取消。')
