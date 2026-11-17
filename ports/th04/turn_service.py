"""Cloudflare managed TURN credentials. No game or rollback dependencies."""
import http.client
import json
import re
import threading
import time
from pathlib import Path

CONFIG_PATH=Path(__file__).resolve().parent/'turn-config.local.json'
TTL=14400  # Four-hour test sessions; restart the room before expiry.
LOCK=threading.BoundedSemaphore(3)
CACHE_LOCK=threading.Lock()
CACHE={}


class TurnError(Exception):
    pass


def read_config():
    try:
        config=json.loads(CONFIG_PATH.read_text(encoding='utf-8-sig'))
        key=config['keyId']
        token=config['apiToken']
        if not isinstance(key,str) or not re.fullmatch(r'[A-Za-z0-9_-]{1,128}',key):
            raise ValueError()
        if not isinstance(token,str) or not token or len(token)>4096 or any(ord(c)<33 or ord(c)>126 for c in token):
            raise ValueError()
        return key,token
    except FileNotFoundError:
        raise TurnError('尚未配置 TURN。请在电脑运行 configure-turn.bat，填入 Cloudflare TURN Key ID 和专用 API Token，然后重新建房。') from None
    except (OSError,ValueError,TypeError,KeyError):
        raise TurnError('本机 TURN 配置无效，请重新运行 configure-turn.bat；不要将密钥放入网页目录。') from None


def credentials(identity):
    # Separate lock from ROOMS: an upstream request must never block room polling.
    # Avoid waiting for another request past the browser's configuration deadline.
    if not LOCK.acquire(timeout=1):
        raise TurnError('TURN 凭据服务正忙，请稍后重新建房。')
    try:
        key,token=read_config()
        now=time.monotonic()
        cache_key=(identity,key,token)
        with CACHE_LOCK:
            for item,value in list(CACHE.items()):
                if value['until']<=now:
                    del CACHE[item]
            if cache_key in CACHE:
                return CACHE[cache_key]['result']
        connection=http.client.HTTPSConnection('rtc.live.cloudflare.com',timeout=6)
        try:
            connection.request('POST',f'/v1/turn/keys/{key}/credentials/generate-ice-servers',
                body=json.dumps({'ttl':TTL}),headers={'Authorization':f'Bearer {token}','Content-Type':'application/json'})
            response=connection.getresponse()
            if response.status not in (200,201):
                if response.status in (401,403):
                    raise TurnError('TURN 服务拒绝凭据，请检查 TURN Key ID 与配套 API Token。')
                raise TurnError(f'TURN 服务返回 HTTP {response.status}，请检查账号开通状态或稍后重试。')
            raw=response.read(65537)
            if len(raw)>65536:
                raise ValueError()
            servers=json.loads(raw)['iceServers']
            if not isinstance(servers,list):
                raise ValueError()
            cleaned=[]
            has_turn=False
            for server in servers:
                urls=server['urls']
                if isinstance(urls,str):urls=[urls]
                if not isinstance(urls,list) or not urls or any(not isinstance(url,str) or not re.match(r'^(stun|stuns|turn|turns):\S+$',url) for url in urls):
                    raise ValueError()
                entry={'urls':urls}
                if any(url.startswith(('turn:','turns:')) for url in urls):
                    for field in ('username','credential'):
                        if not isinstance(server.get(field),str) or not server[field]:raise ValueError()
                        entry[field]=server[field]
                    has_turn=True
                cleaned.append(entry)
            if not has_turn:raise ValueError()
            result={'iceServers':cleaned,'iceTransportPolicy':'all'}
            with CACHE_LOCK:
                if len(CACHE)>=192:CACHE.pop(next(iter(CACHE)))
                CACHE[cache_key]={'until':time.monotonic()+TTL-600,'result':result}
            return result
        except TurnError:
            raise
        except (OSError,http.client.HTTPException):
            raise TurnError('电脑无法访问 TURN 凭据服务，请检查网络、DNS 或代理；这不是 NP2 启动错误。') from None
        except (ValueError,KeyError,TypeError):
            raise TurnError('TURN 服务返回的凭据格式无效。') from None
        finally:
            connection.close()
    finally:
        LOCK.release()
