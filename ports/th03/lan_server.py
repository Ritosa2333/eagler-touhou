import replay_history
"""TH03 LAN room/signaling server. Game input is exchanged by the selected transport."""
import argparse, json, secrets, socket, threading, time
from urllib.request import urlopen
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, unquote, urlparse
from turn_service import rtc_configuration, TurnError

ROOT = Path(__file__).resolve().parent / 'web'
PROTOCOL = 'th03-lan/1'
ROOMS, LOCK, TTL = {}, threading.RLock(), 180
DEFAULT_SETTINGS = {'language': 'jp', 'difficulty': 3, 'clock': 32, 'rollback': False,
                    'focusEnabled': True, 'touchUnlimitedAllowed': True, 'hostSeat': 0}


def validated_settings(value, host_seat=0):
    if not isinstance(value, dict): return None
    value = {**value, 'hostSeat': value.get('hostSeat', host_seat)}
    if set(value) != set(DEFAULT_SETTINGS) \
      or value['language'] not in ('jp', 'cn') \
      or type(value['hostSeat']) is not int or value['hostSeat'] not in (0, 1) \
      or type(value['difficulty']) is not int or value['difficulty'] not in range(4) \
      or type(value['clock']) is not int or value['clock'] != 32 \
      or any(type(value[key]) is not bool for key in ('rollback', 'focusEnabled', 'touchUnlimitedAllowed')):
        return None
    return value

def snapshot(room):
    return {'protocol': PROTOCOL, 'room': room['code'], 'mode': room['mode'],
            'network': room['network'], 'ice': room['ice'],
            'revision': room['revision'], 'started': room['started'], 'runId': room.get('runId', 0),
            'present': {'host': bool(room['host']), 'guest': bool(room['guest'])},
            'ready': dict(room['ready']), 'settings': dict(room['settings']),
            'startup': dict(room['startup'])}

class Handler(SimpleHTTPRequestHandler):
    protocol_version = 'HTTP/1.1'
    extensions_map = {**SimpleHTTPRequestHandler.extensions_map, '.js': 'text/javascript', '.mjs': 'text/javascript', '.wasm': 'application/wasm', '.gz': 'application/octet-stream'}
    def log_message(self, *args): pass
    def end_headers(self):
        self.send_header('Cache-Control', 'no-store')
        self.send_header('X-Content-Type-Options', 'nosniff')
        self.send_header('Referrer-Policy', 'no-referrer')
        super().end_headers()
    def list_directory(self, path): self.send_error(404)
    def reply(self, value, status=200):
        body = json.dumps(value, ensure_ascii=False).encode()
        self.send_response(status); self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Content-Length', str(len(body))); self.end_headers(); self.wfile.write(body)
    def do_GET(self):
        if replay_history.handle_get(self, ROOMS, LOCK, '03', lambda r: time.monotonic()-r['seen']['host']>TTL): return
        url = urlparse(self.path)
        if url.path == '/relay':
            from relay_server import handle
            handle(self, ROOMS, LOCK, TTL)
            return
        if url.path in ('/api/state', '/api/signals', '/api/relay-auth'):
            q = {k: v[0] for k, v in parse_qs(url.query).items()}; self.api('state', q); return
        if url.path == '/': self.path = '/lan.html'
        path = unquote(url.path)
        if '\\' in path or any(p.startswith('.') for p in path.split('/') if p): self.send_error(404); return
        super().do_GET()
    def do_POST(self):
        if replay_history.handle_post(self, ROOMS, LOCK, '03', lambda r: time.monotonic()-r['seen']['host']>TTL): return
        try:
            origin = self.headers.get('Origin')
            if origin and urlparse(origin).netloc != self.headers.get('Host'): raise ValueError('来源不匹配')
            size = int(self.headers.get('Content-Length', '0'))
            if not 0 < size <= 65536: raise ValueError('消息大小无效')
            data = json.loads(self.rfile.read(size))
            if not isinstance(data, dict): raise ValueError('消息格式无效')
            self.api(urlparse(self.path).path.removeprefix('/api/'), data)
        except (ValueError, TypeError, json.JSONDecodeError): self.reply({'error': '请求格式无效'}, 400)
    def api(self, action, data):
        if action == 'ice':
            return self.ice_configuration(data)
        with LOCK:
            now = time.monotonic()
            for code, room in list(ROOMS.items()):
                if now - room['seen']['host'] > TTL: ROOMS.pop(code, None)
            if action == 'create':
                mode = data.get('mode', 'rtc')
                if mode not in ('rtc', 'relay') or data.get('protocol') != PROTOCOL: return self.reply({'error': '联机版本或模式无效'}, 400)
                network, ice = data.get('network', 'lan'), data.get('ice', 'all')
                if network not in ('lan', 'public') or ice not in ('all', 'relay'):
                    return self.reply({'error': '公网连接设置无效'}, 400)
                settings = validated_settings(data['settings']) if 'settings' in data else dict(DEFAULT_SETTINGS)
                if settings is None: return self.reply({'error': '开局设置无效'}, 400)
                if len(ROOMS) >= 256: return self.reply({'error': '房间数量已满，请稍后再试'}, 429)
                code = f'{secrets.randbelow(10000):04d}'
                while code in ROOMS: code = f'{secrets.randbelow(10000):04d}'
                token = secrets.token_urlsafe(18)
                room = {'code': code, 'mode': mode, 'network': network, 'ice': ice,
                        'host': token, 'guest': None, 'revision': 1, 'started': False, 'runId': 0,
                        'seen': {'host': now, 'guest': now}, 'signals': {'host': [], 'guest': []},
                        'ready': {'host': False, 'guest': False}, 'startup': {},
                        'settings': settings}
                ROOMS[code] = room
                return self.reply({'protocol': PROTOCOL, 'room': code, 'token': token, 'role': 'host', 'state': snapshot(room)})
            code = str(data.get('room', '')).upper(); room = ROOMS.get(code)
            if not room or len(code) != 4: return self.reply({'error': '房间不存在或已结束'}, 404)
            role = next((r for r in ('host', 'guest') if room[r] and room[r] == data.get('token')), None)
            if action == 'join':
                if data.get('protocol') != PROTOCOL: return self.reply({'error': '请刷新联机页面'}, 409)
                if room['guest'] or room['started']: return self.reply({'error': '房间已满或已经开始'}, 409)
                room['guest'] = secrets.token_urlsafe(18); room['revision'] += 1
                room['seen']['guest'] = now; room['ready'] = {'host': False, 'guest': False}
                return self.reply({'protocol': PROTOCOL, 'room': code, 'token': room['guest'], 'role': 'guest', 'state': snapshot(room)})
            if not role: return self.reply({'error': '房间身份无效'}, 403)
            if data.get('protocol') != PROTOCOL: return self.reply({'error': '请刷新联机页面'}, 409)
            room['seen'][role] = now
            if action in ('end', 'signal', 'progress', 'signals', 'ready', 'start') and str(data.get('runId')) != str(room['runId']):
                return self.reply({'error': '本局编号已失效'}, 409)
            if action == 'end':
                if room['started']:
                    room['started'] = False
                    room['ready'] = {'host': False, 'guest': False}
                    room['startup'] = {}
                    room['signals'] = {'host': [], 'guest': []}
                    room['ice_requested'] = {}
                    room['revision'] += 1
                return self.reply(snapshot(room))
            if action == 'state':
                if self.path.startswith('/api/relay-auth'):
                    if data.get('protocol') != PROTOCOL or data.get('role') != role or room['mode'] != 'relay' or not room['started']:
                        return self.reply({'ok': False}, 403)
                    return self.reply({'ok': True, 'mode': room['mode'], 'role': role})
                if self.path.startswith('/api/signals'):
                    if str(data.get('runId')) != str(room['runId']):
                        return self.reply({'error': '本局编号已失效'}, 409)
                    messages, room['signals'][role] = room['signals'][role], []
                    return self.reply({'messages': messages, 'state': snapshot(room)})
                return self.reply(snapshot(room))
            if action == 'settings':
                if role != 'host' or room['started']: return self.reply({'error': '仅房主可在开局前修改设置'}, 403)
                value = validated_settings(data.get('settings'), room['settings']['hostSeat'])
                if value is None:
                    return self.reply({'error': '开局设置无效'}, 400)
                room['settings'] = value; room['ready'] = {'host': False, 'guest': False}; room['revision'] += 1
                return self.reply(snapshot(room))
            if action == 'ready':
                if room['started']: return self.reply({'error': '本局已开始'}, 409)
                room['ready'][role] = not room['ready'][role]; room['revision'] += 1
                return self.reply(snapshot(room))
            if action == 'progress':
                if not room['started']: return self.reply({'error': '本局尚未开始'}, 409)
                loaded = data.get('loaded') is True or room['startup'].get(role, {}).get('loaded', False)
                room['startup'][role] = {'message': str(data.get('message', ''))[:200],
                                         'loaded': loaded, 'error': data.get('error') is True}
                return self.reply(snapshot(room))
            if action == 'signal':
                target = data.get('to'); message = data.get('message')
                if not room['started'] or target not in ('host', 'guest') or target == role or not room[target] \
                  or not isinstance(message, dict) or len(room['signals'][target]) >= 256: return self.reply({'error': '信令无效'}, 400)
                room['signals'][target].append({'from': role, 'message': message})
                return self.reply({'ok': True})
            if action == 'signals':
                messages, room['signals'][role] = room['signals'][role], []
                return self.reply({'messages': messages, 'state': snapshot(room)})
            if action == 'start':
                if role != 'host' or room['started'] or not room['guest'] or not all(room['ready'].values()):
                    return self.reply({'error': '需要双方加入并准备后，由房主开始'}, 409)
                room['started'] = True; room['revision'] += 1; room['runId'] += 1
                return self.reply(snapshot(room))
            if action == 'leave':
                if role == 'host': ROOMS.pop(code, None)
                else:
                    room['guest'] = None; room['started'] = False; room['revision'] += 1
                    room['ready'] = {'host': False, 'guest': False}; room['startup'] = {}
                    room['signals'] = {'host': [], 'guest': []}; room['ice_requested'] = {}
                return self.reply({'ok': True})
            return self.reply({'error': '未知接口'}, 404)

    def ice_configuration(self, data):
        # Provider I/O must not hold the room lock: polling, signaling and TCP
        # input forwarding must keep working during a slow credentials request.
        code, role, token = data.get('room'), data.get('role'), data.get('token')
        if not isinstance(code, str) or role not in ('host', 'guest') or not isinstance(token, str):
            return self.reply({'error': '房间身份无效'}, 403)
        with LOCK:
            now = time.monotonic(); room = ROOMS.get(code)
            if not room or now - room['seen']['host'] > TTL:
                return self.reply({'error': '房间不存在或已结束'}, 404)
            if not token or room[role] != token or data.get('protocol') != PROTOCOL:
                return self.reply({'error': '房间身份无效'}, 403)
            if not room['started'] or room['mode'] != 'rtc' or room['network'] != 'public':
                return self.reply({'error': '请先开始公网 UDP 房间'}, 409)
            requests = room.setdefault('ice_requested', {})
            if now - requests.get(role, -float('inf')) < 5:
                return self.reply({'error': '连接配置请求过于频繁'}, 429)
            requests[role] = now; room['seen'][role] = now
            identity = (code, room['host'], role, token); policy = room['ice']
        try:
            config = rtc_configuration(identity, policy)
        except TurnError as error:
            return self.reply({'error': str(error)}, 503)
        with LOCK:
            current = ROOMS.get(code)
            if current is not room or current[role] != token or not current['started'] \
              or time.monotonic() - current['seen']['host'] > TTL:
                return self.reply({'error': '房间已结束或身份已失效'}, 403)
        return self.reply(config)

def main():
    parser = argparse.ArgumentParser(); parser.add_argument('--port', type=int, default=9877)
    parser.add_argument('--bind', default='0.0.0.0'); args = parser.parse_args()
    try:
        class Server(ThreadingHTTPServer):
            allow_reuse_address = False
            def server_bind(self):
                if hasattr(socket, 'SO_EXCLUSIVEADDRUSE'):
                    self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
                super().server_bind()
        server = Server((args.bind, args.port), partial(Handler, directory=str(ROOT)))
    except OSError as error:
        print(f'当前启动目录：{ROOT.parent}', flush=True)
        try:
            with urlopen(f'http://127.0.0.1:{args.port}/lockstep-runtime.json', timeout=2) as response:
                active_version = json.load(response).get('protocol', 'unknown')
            print(f'端口 {args.port} 已由另一服务占用，正在提供：{active_version}', flush=True)
        except (OSError, ValueError):
            print(f'端口 {args.port} 已被占用。', flush=True)
        raise SystemExit(f'新版服务没有启动。请关闭旧服务，再运行此目录的 start-lan.bat；刷新网页仍会访问旧服务。({error})')
    print('TH03 netplay - lobby, STUN/TURN credentials and TCP WebSocket relay', flush=True)
    with (ROOT / 'lockstep-runtime.json').open(encoding='utf-8') as runtime_file:
        runtime_version = json.load(runtime_file)['protocol']
    print(f'服务目录：{ROOT.parent}\n联机版本：{runtime_version}', flush=True)
    print(f'A computer: http://localhost:{args.port}/lan.html', flush=True)
    addresses = sorted({item[4][0] for item in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET)
                        if not item[4][0].startswith('127.')})
    for address in addresses: print(f'B computer: http://{address}:{args.port}/lan.html', flush=True)
    print('Public UDP: https://YOUR-HOST/lan.html?network=public', flush=True)
    print('Public TCP: https://YOUR-HOST/lan.html?network=public&transport=ws', flush=True)
    print('Keep this window open. Ctrl+C to stop.', flush=True)
    try: server.serve_forever()
    except KeyboardInterrupt: pass
    finally: server.server_close()
if __name__ == '__main__': main()
