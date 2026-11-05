import replay_history
"""TH01 two/three-seat rooms, trickle ICE signaling and authenticated WS relay."""
import argparse
import json
import secrets
import socket
import threading
import time
import webbrowser
from functools import partial
from http.server import ThreadingHTTPServer
from urllib.parse import parse_qs, urlsplit
from server import Handler as AssetHandler, ROOT
import relay_server
from turn_service import rtc_configuration, TurnError, CONFIG_PATH as TURN_CONFIG_PATH, STUN_SERVERS

ROOMS = {}
LOCK = threading.RLock()
ROLES = ('host', 'guest', 'guest2')
TTL = 60
ALLOW_QA = False
ICE_SERVERS = []
PROTOCOL = 'th01-native-lockstep/1'

def settings(value):
    if not isinstance(value, dict): raise ValueError('设置格式无效')
    rank, count = value.get('rank', 1), value.get('count', 2)
    transport, delay = value.get('transport', 'rtc'), value.get('delay', 0)
    players = value.get('players', 2)
    start_stage = value.get('startStage', 'normal')
    valid_stages = {'normal'} | {f's{n}' for n in range(1, 6)} | {f'{r}{n}' for r in ('m', 'j') for n in range(6, 21)}
    if type(start_stage) is not str or start_stage not in valid_stages: raise ValueError('起始关卡无效')
    life_compensation = value.get('lifeCompensation', True)
    if type(life_compensation) is not bool: raise ValueError('生命补偿设置无效')
    network, ice = value.get('network', 'lan'), value.get('ice', 'all')
    if type(players) is not int or players not in (2, 3): raise ValueError('人数无效')
    if type(rank) is not int or rank not in range(4): raise ValueError('难度无效')
    if type(count) is not int or count not in (1, 2, 3, 4, 6, 8): raise ValueError('玉数无效')
    if transport not in ('rtc', 'ws'): raise ValueError('连接方式无效')
    if network not in ('lan', 'public'): raise ValueError('网络模式无效')
    if ice not in ('all', 'relay'): raise ValueError('ICE 策略无效')
    if ice == 'relay' and (network != 'public' or transport != 'rtc'):
        raise ValueError('强制 TURN 仅适用于公网 UDP 房间')
    if type(delay) is not int or (delay != 0 and delay not in range(1, 13)): raise ValueError('输入缓冲无效')
    return dict(startStage=start_stage, rank=rank, count=count, players=players, transport=transport,
                network=network, ice=ice, delay=delay, qa=ALLOW_QA,
                lifeCompensation=life_compensation)

def active_roles(room):
    return ROLES[:room['settings']['players']]

def cleanup():
    now = time.monotonic()
    for code, room in list(ROOMS.items()):
        if now-room['seen']['host'] > TTL:
            del ROOMS[code]
        else:
            for role in active_roles(room)[1:]:
                if room[role] and now-room['seen'][role] > TTL:
                    if room['phase'] == 'loading': room['ended'] = '队友离线，请重建房间'
                    room[role] = None
                    room['ready'] = dict.fromkeys(ROLES, False)
                    room['signals'][role].clear()

def public(code, room, role):
    return dict(room=code, role=role, settings=room['settings'], phase=room['phase'],
                generation=room['generation'], transport=room['transport'],
                seats=room['seats'], ready=room['ready'], runId=room.get('runId', 0),
                present={r: bool(room[r]) for r in ROLES}, ended=room['ended'])

def network_capabilities():
    return {'turnConfigured': TURN_CONFIG_PATH.exists(),
            'publicStunCount': len(ICE_SERVERS or STUN_SERVERS)}

def ice_configuration(data):
    """Authenticate before provider I/O, then recheck after releasing the lock."""
    code, role, token = data.get('room'), data.get('role'), data.get('token')
    if (data.get('protocol') != PROTOCOL or not isinstance(code, str) or
        role not in ROLES or not isinstance(token, str) or not token):
        return {'error': '房间身份无效'}, 403
    with LOCK:
        cleanup()
        room = ROOMS.get(code)
        if not room: return {'error': '房间不存在或已结束'}, 404
        if role not in active_roles(room) or not room[role] or not secrets.compare_digest(room[role], token):
            return {'error': '房间身份无效'}, 403
        if room['phase'] != 'loading' or room['transport'] != 'rtc' or room['ended']:
            return {'error': '请在全员准备并开始 UDP 房间后获取连接配置'}, 409
        now = time.monotonic()
        requested = room.setdefault('ice_requested', {})
        if now - requested.get(role, -float('inf')) < 5:
            return {'error': '连接配置请求过于频繁，请稍后重试'}, 429
        requested[role] = now; room['seen'][role] = now
        config = room['settings']
        network, policy = config['network'], config['ice']
        identity = (code, room['generation'], role, token)
    try:
        result = (rtc_configuration(identity, policy, ICE_SERVERS or None)
                  if network == 'public' else
                  {'iceServers': ICE_SERVERS, 'iceTransportPolicy': 'all'})
    except TurnError as error:
        return {'error': str(error)}, 503
    with LOCK:
        current = ROOMS.get(code)
        if (current is not room or current[role] != token or current['ended'] or
            current['phase'] != 'loading' or current['generation'] != identity[1] or
            any(time.monotonic() - current['seen'][r] > TTL for r in ('host', role))):
            return {'error': '房间已结束或身份已失效'}, 403
    return result, 200

class LanHTTPServer(ThreadingHTTPServer):
    # On Windows SO_REUSEADDR can let two services answer one port inconsistently.
    allow_reuse_address = False
    def server_bind(self):
        if hasattr(socket, 'SO_EXCLUSIVEADDRUSE'):
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_EXCLUSIVEADDRUSE, 1)
        super().server_bind()

class Handler(AssetHandler):
    protocol_version = 'HTTP/1.1'
    def log_message(self, fmt, *args):
        # Signaling bodies/relay URLs carry secrets; keep them out of logs.
        if urlsplit(self.path).path.startswith(('/api/', '/relay')): return
        super().log_message(fmt, *args)

    def reply(self, value, status=200):
        body = json.dumps(value, ensure_ascii=False).encode()
        self.send_response(status)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Content-Length', str(len(body)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(body)

    def identity(self, data):
        code, token = str(data.get('room', '')).upper(), data.get('token')
        room = ROOMS.get(code)
        if not room: raise ValueError('房间不存在或已经关闭')
        role = next((r for r in ROLES if room[r] and secrets.compare_digest(room[r], str(token))), None)
        if role is None: raise ValueError('房间身份无效')
        room['seen'][role] = time.monotonic()
        return code, room, role

    def do_GET(self):
        if replay_history.handle_get(self, ROOMS, LOCK, '01', lambda r: time.monotonic()-r['seen']['host']>TTL): return
        url = urlsplit(self.path)
        if url.path == '/relay':
            relay_server.handle(self, ROOMS, LOCK, ROLES, TTL)
        elif url.path == '/api/ice':
            self.reply({'iceServers': ICE_SERVERS, 'iceTransportPolicy': 'all'})
        elif url.path == '/api/network':
            self.reply(network_capabilities())
        elif url.path == '/api/events':
            try:
                with LOCK:
                    cleanup()
                    code, room, role = self.identity({k: v[0] for k, v in parse_qs(url.query).items()})
                    value = public(code, room, role)
                    value['signals'] = room['signals'][role][:]
                    room['signals'][role].clear()
                self.reply(value)
            except ValueError as error: self.reply({'error': str(error)}, 400)
        elif url.path.startswith('/api/'):
            self.reply({'error': '接口不存在'}, 404)
        else:
            if url.path == '/': self.path = '/lan.html'
            super().do_GET()

    def do_POST(self):
        if replay_history.handle_post(self, ROOMS, LOCK, '01', lambda r: time.monotonic()-r['seen']['host']>TTL): return
        origin = urlsplit(self.headers.get('Origin', ''))
        if origin.scheme not in ('http', 'https') or origin.netloc != self.headers.get('Host'):
            self.close_connection = True
            self.reply({'error': '请求来源不匹配'}, 403); return
        try:
            size = int(self.headers.get('Content-Length', '0'))
            if size <= 0 or size > 65536:
                self.close_connection = True
                raise ValueError('请求过大或为空')
            data = json.loads(self.rfile.read(size))
            if not isinstance(data, dict): raise ValueError('请求格式无效')
            if urlsplit(self.path).path == '/api/ice':
                value, status = ice_configuration(data)
                self.reply(value, status); return
            with LOCK:
                cleanup()
                value = self.action(urlsplit(self.path).path, data)
            self.reply(value)
        except (ValueError, TypeError, KeyError) as error:
            self.reply({'error': str(error)}, 400)

    def action(self, path, data):
        if path == '/api/create':
            if len(ROOMS) >= 64: raise ValueError('房间数已满')
            config = settings(data.get('settings', {}))
            code = str(secrets.randbelow(9000)+1000)
            while code in ROOMS: code = str(secrets.randbelow(9000)+1000)
            token = secrets.token_urlsafe(24)
            room = dict(host=token, guest=None, guest2=None, settings=config, transport=config['transport'],
                        phase='lobby', generation=secrets.token_hex(12), runId=0,
                        seats={'host': 0, 'guest': 1, 'guest2': 2}, ready=dict.fromkeys(ROLES, False),
                        seen={'host': time.monotonic(), 'guest': 0, 'guest2': 0},
                        signals={r: [] for r in ROLES}, ice_requested={}, ended='')
            ROOMS[code] = room
            return {**public(code, room, 'host'), 'token': token}
        if path == '/api/join':
            code = str(data.get('room', '')).upper()
            room = ROOMS.get(code)
            if not room: raise ValueError('房间不存在')
            role = next((r for r in active_roles(room)[1:] if not room[r]), None)
            if room['phase'] != 'lobby' or role is None: raise ValueError('房间已开始或已满')
            room[role] = secrets.token_urlsafe(24)
            room['seen'][role] = time.monotonic()
            room['ready'] = dict.fromkeys(ROLES, False)
            return {**public(code, room, role), 'token': room[role]}
        code, room, role = self.identity(data)
        if path == '/api/end':
            if data.get('runId') != room['runId']: raise ValueError('本局编号已失效')
            if room['phase'] != 'lobby':
                room['phase'] = 'lobby'
                room['ended'] = ''
                room['ready'] = dict.fromkeys(ROLES, False)
                room['signals'] = {r: [] for r in ROLES}
                room['ice_requested'] = {}
            return public(code, room, role)
        if path in ('/api/signal', '/api/ready', '/api/start') and data.get('runId') != room['runId']:
            raise ValueError('本局编号已失效')
        if path == '/api/leave':
            if role == 'host': del ROOMS[code]
            else:
                room[role] = None
                room['signals'][role].clear()
                room['ready'] = dict.fromkeys(ROLES, False)
                if room['phase'] == 'loading': room['ended'] = '队友离开，请重建房间'
            return {'ok': True}
        if path == '/api/signal':
            if room['phase'] != 'loading' or room['ended']: raise ValueError('本局尚未开始或已经结束')
            message = data.get('message', {})
            if not isinstance(message, dict) or message.get('type') not in ('offer', 'answer', 'ice', 'ended'):
                raise ValueError('信令无效')
            if data.get('generation') != room['generation']: raise ValueError('运行代次无效')
            if message['type'] == 'offer' and role != 'host': raise ValueError('仅房主发起连接')
            if message['type'] == 'answer' and role == 'host': raise ValueError('仅客机回复连接')
            if message['type'] == 'ended':
                room['ended'] = '本局已停止，请重建房间'
                targets = [r for r in active_roles(room) if r != role]
            else:
                target = data.get('target')
                if target not in active_roles(room) or target == role or (role != 'host' and target != 'host'):
                    raise ValueError('信令目标无效')
                targets = [target]
            for target in targets:
                if len(room['signals'][target]) >= 128: raise ValueError('信令积压')
                room['signals'][target].append({**message, 'from': role})
            return {'ok': True}
        if room['phase'] != 'lobby': raise ValueError('开局后不能更改房间')
        if path == '/api/settings':
            if role != 'host': raise ValueError('仅房主更改设置')
            config = settings(data['settings'])
            if config['players'] == 2 and room['guest2']: raise ValueError('第三位队友需先离开才能改为双人')
            if config['players'] != room['settings']['players']:
                room['seats'] = dict(zip(ROLES, range(3)))
            room['settings'] = config
            room['transport'] = room['settings']['transport']
            room['ready'] = dict.fromkeys(ROLES, False)
        elif path == '/api/seat':
            if role != 'host': raise ValueError('仅房主交换座位')
            count = room['settings']['players']
            room['seats'].update({r: (room['seats'][r]+1)%count for r in active_roles(room)})
            room['ready'] = dict.fromkeys(ROLES, False)
        elif path == '/api/ready':
            if type(data.get('ready')) is not bool: raise ValueError('准备状态无效')
            room['ready'][role] = data['ready']
        elif path == '/api/start':
            if role != 'host': raise ValueError('仅房主开始')
            if not all(room[r] and room['ready'][r] for r in active_roles(room)): raise ValueError('需要全员加入并准备')
            room['phase'] = 'loading'
            room['runId'] += 1
        else: raise ValueError('接口不存在')
        return public(code, room, role)

def main():
    global ALLOW_QA, ICE_SERVERS
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=9874)
    parser.add_argument('--bind', default='0.0.0.0')
    parser.add_argument('--no-browser', action='store_true')
    parser.add_argument('--qa', action='store_true', help='Enable synchronized host test commands')
    parser.add_argument('--stun', action='append', default=[], help='STUN URL for cross-NAT ICE; may repeat')
    args = parser.parse_args(); ALLOW_QA = args.qa
    if any(not url.lower().startswith(('stun:', 'stuns:')) or any(c.isspace() for c in url) for url in args.stun):
        parser.error('--stun requires a stun: or stuns: URL')
    ICE_SERVERS = [{'urls': url} for url in args.stun]
    server = LanHTTPServer((args.bind, args.port), partial(Handler, directory=str(ROOT)))
    print(f'TH01 LAN · http://127.0.0.1:{args.port}/lan.html', flush=True)
    print(f'Engine: native C++ / WASM; protocol: {PROTOCOL}', flush=True)
    if ALLOW_QA: print('QA mode: synchronized host test commands enabled.', flush=True)
    print(f'Public UDP: default STUN enabled; TURN {"configured" if TURN_CONFIG_PATH.exists() else "not configured"}.', flush=True)
    print('Public UDP URL: https://YOUR-HOST/lan.html?network=public', flush=True)
    print('Public TCP URL: https://YOUR-HOST/lan.html?network=public&transport=ws', flush=True)
    print('Use start-public-tunnel.bat for a temporary HTTPS entry. The tunnel is not a TURN server.', flush=True)
    addresses = sorted({a[4][0] for a in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET)})
    for address in addresses:
        if not address.startswith('127.'): print(f'LAN: http://{address}:{args.port}/lan.html', flush=True)
    print('Keep this window open. Ctrl+C to stop.', flush=True)
    if not args.no_browser: threading.Timer(.5, lambda: webbrowser.open(f'http://127.0.0.1:{args.port}/lan.html')).start()
    try: server.serve_forever()
    except KeyboardInterrupt: pass
    finally: server.server_close()

if __name__ == '__main__': main()
