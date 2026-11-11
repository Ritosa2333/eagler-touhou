"""Same-port WebSocket upgrade using TH04's websockets Sans-I/O pattern."""
import json
import select
import socket
import threading
import time
from collections import deque
from urllib.parse import parse_qs, urlparse

HUBS, HUB_LOCK = {}, threading.RLock()
ACTIONS = {'up', 'down', 'left', 'right', 'shot', 'rapid', 'attack', 'focus'}

def valid_input_policy(value):
    if not isinstance(value, dict) or type(value.get('rollbackEnabled')) is not bool:
        return False
    if any(type(value.get(key)) is not int for key in ('schema', 'rttMs', 'transitSteps', 'inputDelay', 'predictionSteps', 'predictionLead')):
        return False
    rtt, enabled = value['rttMs'], value['rollbackEnabled']
    if not 0 <= rtt <= 60000:
        return False
    transit = (rtt*3+99)//100 if enabled and rtt > 20 else 0
    prediction = min(2, max(0, transit-1))
    return value == {'schema': 2, 'rollbackEnabled': enabled, 'rttMs': rtt, 'transitSteps': transit,
                     'inputDelay': min(6, transit-prediction), 'predictionSteps': prediction,
                     'predictionLead': prediction+2 if enabled else 0}

def valid_message(message):
    if not isinstance(message, dict):
        return False
    if message.get('sync') == 'th03-rollback/7':
        kind = message.get('type')
        if kind == 'sync-ready':
            return isinstance(message.get('identity'), dict)
        if kind in ('buffer-ping', 'buffer-pong'):
            return type(message.get('id')) is int and 1 <= message['id'] < 2**53
        if kind == 'buffer-report':
            value = message.get('report')
            return isinstance(value, dict) and type(value.get('rollbackEnabled')) is bool \
                and type(value.get('rttMs')) is int and type(value.get('maxRttMs')) is int \
                and 0 <= value['rttMs'] <= value['maxRttMs'] <= 60000
        if kind in ('buffer-policy', 'buffer-ack'):
            return valid_input_policy(message.get('policy'))
        if kind == 'rollback':
            return type(message.get('frame')) is int and 0 <= message['frame'] < 2**53
        if kind == 'check':
            return type(message.get('frame')) is int and message['frame'] >= 0 and isinstance(message.get('hash'), str) and len(message['hash']) <= 128
        value = message.get('input')
        return kind == 'frame' and type(message.get('frame')) is int and message['frame'] >= 0 \
            and isinstance(value, dict) and isinstance(value.get('actions'), list) \
            and all(isinstance(a, str) and a in ACTIONS for a in value['actions']) \
            and type(value.get('touch')) is int and 0 <= value['touch'] < 2**45 \
            and type(value.get('focusEnabled')) is bool and type(value.get('points')) is bool \
            and isinstance(value.get('commands'), list) and len(value['commands']) <= 16 \
            and all(isinstance(c, str) and c in ('pause', 'resume', 'round', 'match', 'up', 'down', 'confirm', 'escape', 'hidden', 'visible') for c in value['commands'])
    if 'sync' in message:
        return False
    return message.get('type') == 'input' and type(message.get('seq')) is int \
        and isinstance(message.get('actions'), list) and all(isinstance(a, str) and a in ACTIONS for a in message['actions']) \
        and type(message.get('touch', 0)) is int and 0 <= message.get('touch', 0) < 2**45

class OutboundWriter:
    def __init__(self, sock):
        self.sock = sock; self.condition = threading.Condition()
        self.chunks = deque(); self.bytes = 0; self.stopping = False
        self.thread = threading.Thread(target=self.run, daemon=True); self.thread.start()
    def abort(self):
        with self.condition:
            self.stopping = True; self.chunks.clear(); self.bytes = 0; self.condition.notify()
        try: self.sock.shutdown(socket.SHUT_RDWR)
        except OSError: pass
    def put(self, chunk):
        with self.condition:
            if self.stopping: raise OSError('Relay closed')
            if self.bytes + len(chunk) <= 524288 and len(self.chunks) < 256:
                self.chunks.append(chunk); self.bytes += len(chunk); self.condition.notify(); return
        self.abort(); raise OSError('Relay destination congested')
    def run(self):
        try:
            while True:
                with self.condition:
                    self.condition.wait_for(lambda: self.chunks or self.stopping)
                    if not self.chunks: return
                    chunk = self.chunks.popleft(); self.bytes -= len(chunk)
                self.sock.sendall(chunk)
        except OSError: self.abort()
    def close(self):
        with self.condition: self.stopping = True; self.condition.notify()
        self.thread.join(timeout=0.25); self.abort()

def handle(handler, rooms, room_lock, ttl):
    try:
        from websockets.server import ServerProtocol
        from websockets.protocol import OPEN
        from websockets.frames import OP_TEXT, OP_CONT, OP_CLOSE
    except ImportError:
        handler.reply({'error': '请运行 install-relay.bat 安装转发依赖后重启服务'}, 503); return
    origin = urlparse(handler.headers.get('Origin', ''))
    if origin.scheme not in ('http', 'https') or origin.netloc != handler.headers.get('Host'):
        handler.reply({'error': '中继来源不匹配'}, 403); return
    data = {k: v[0] for k, v in parse_qs(urlparse(handler.path).query).items()}
    code, token, role = data.get('room'), data.get('token'), data.get('role')
    with room_lock:
        room = rooms.get(code)
        if not room or time.monotonic() - room['seen']['host'] > ttl:
            handler.reply({'error': '房间已结束'}, 404); return
        if role not in ('host', 'guest') or room[role] != token or not token \
          or room['mode'] != 'relay' or not room['started'] or data.get('protocol') != 'th03-lan/1':
            handler.reply({'error': '中继身份或房间模式无效'}, 403); return
        identity = room['host']
    run_id=room.get('runId',0)
    if str(data.get('runId'))!=str(run_id):
        handler.reply({'error':'本局编号已失效'},409);return
    key = (code, identity, run_id); sock = handler.connection
    sock.settimeout(5); sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    handler.close_connection = True
    protocol = ServerProtocol(max_size=65536); send_lock = threading.Lock(); writer = OutboundWriter(sock)
    def flush():
        for chunk in protocol.data_to_send():
            if chunk: writer.put(chunk)
    def send(value):
        with send_lock:
            if protocol.state is not OPEN: raise OSError('Relay closed')
            protocol.send_text(json.dumps(value).encode()); flush()
    entry = {'send': send}
    try:
        request = (handler.requestline + '\r\n' + ''.join(f'{k}: {v}\r\n' for k, v in handler.headers.items()) + '\r\n').encode('iso-8859-1')
        protocol.receive_data(request); events = protocol.events_received()
        if len(events) != 1: return
        response = protocol.accept(events[0]); protocol.send_response(response); flush()
        if response.status_code != 101: return
        with HUB_LOCK:
            hub = HUBS.setdefault(key, {})
            if role in hub: send({'type': 'error', 'message': '重复连接'}); return
            hub[role] = entry
            if 'host' in hub and 'guest' in hub:
                # Clients send sync-ready as soon as they receive ready. Keep
                # registration and both notifications atomic with forwarding.
                for peer_role, peer in hub.items():
                    peer['send']({'type': 'ready', 'role': peer_role})
        fragments = bytearray(); last_data = time.monotonic()
        while True:
            with room_lock:
                current = rooms.get(code)
                if current is not room or current.get('runId',0)!=run_id or current[role] != token or not current['started'] \
                  or time.monotonic() - current['seen']['host'] > ttl: break
            if time.monotonic() - last_data > 40: break
            if not select.select([sock], [], [], 1)[0]: continue
            chunk = sock.recv(65536)
            if not chunk: break
            with send_lock:
                protocol.receive_data(chunk); events = protocol.events_received(); flush()
                opened = protocol.state is OPEN
            if not opened: break
            for event in events:
                if event.opcode == OP_CLOSE: return
                if event.opcode not in (OP_TEXT, OP_CONT): continue
                fragments.extend(event.data)
                if len(fragments) > 65536: raise ValueError('message too large')
                if not event.fin: continue
                message = json.loads(fragments); fragments.clear(); last_data = time.monotonic()
                if not valid_message(message):
                    raise ValueError('invalid input')
                with room_lock: room['seen'][role] = last_data
                with HUB_LOCK: other = HUBS.get(key, {}).get('guest' if role == 'host' else 'host')
                if other:
                    try: other['send'](message)
                    except OSError: pass
    except (OSError, ValueError, TypeError): pass
    finally:
        with HUB_LOCK:
            hub = HUBS.get(key, {})
            if hub.get(role) is entry:
                hub.pop(role)
                for other in hub.values():
                    try: other['send']({'type': 'peer-left'})
                    except OSError: pass
                if not hub: HUBS.pop(key, None)
        with send_lock:
            if protocol.state is OPEN:
                try: protocol.send_close(1000, 'relay ended'); flush()
                except OSError: pass
        writer.close()
