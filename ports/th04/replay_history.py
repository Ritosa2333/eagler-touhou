"""Bounded browser-authored replay archive. Read-only watch tokens never grant a seat.

No emulator runs on the service. The existing room owns publisher authentication.
"""
import copy
import base64
import hashlib
import json
import re
import secrets
import threading
import time
from urllib.parse import urlsplit, parse_qs

MAX_FRAMES = 216000
MAX_BYTES = 48 * 1024 * 1024
TOTAL_BYTES = 128 * 1024 * 1024
TTL = 900
MAX_POINTS = 128
POINT_BYTES = 16 * 1024 * 1024
POINT_TOTAL = 128 * 1024 * 1024


def integer(n, lo, hi):
    return type(n) is int and lo <= n <= hi


def scene_valid(scene):
    if scene is None:
        return True
    if not isinstance(scene, dict) or scene.get('kind') not in ('stage', 'round', 'selection', 'ending'):
        return False
    if scene['kind'] in ('stage', 'round') and not integer(scene.get(scene['kind']), 0, 65535):
        return False
    return all(k == 'kind' or k in ('stage', 'route', 'round') and integer(v, 0, 65535) for k, v in scene.items())


def header_valid(h, game):
    return (isinstance(h, dict) and h.get('version') == 'pc98-confirmed-tape/1'
            and h.get('adapter') == 'confirmed-worker/1' and h.get('game') == game
            and integer(h.get('players'), 1, 3) and integer(h.get('hostSlot'), 0, h['players'] - 1)
            and isinstance(h.get('identity'), str) and re.fullmatch('[a-f0-9]{64}', h['identity'])
            and isinstance(h.get('settings'), dict) and 'opening' in h
            and type(h.get('tickMs')) in (int, float) and 10 <= h['tickMs'] <= 30)


def row_valid(row, header, frame):
    if not isinstance(row, dict) or set(row) != {'inputs', 'offlineMask', 'proof', 'hash'}:
        return False
    if len(json.dumps(row)) > 12000:
        return False
    inputs, game = row['inputs'], header['game']
    if not isinstance(inputs, list) or len(inputs) != header['players'] or not integer(row['offlineMask'], 0, 2 ** header['players'] - 1):
        return False
    if not isinstance(row['proof'], str) or len(row['proof']) > 8192 or ((frame + 1) % 120 == 0) != (row['hash'] is not None):
        return False
    if game != '03':
        return all(integer(n, 0, 2 ** 45 - 1) and not (row['offlineMask'] & (1 << i) and n != 0) for i, n in enumerate(inputs))
    if row['offlineMask']:
        return False
    actions = {'up', 'down', 'left', 'right', 'shot', 'rapid', 'attack', 'focus'}
    commands = {'pause', 'resume', 'round', 'match', 'up', 'down', 'confirm', 'escape', 'hidden', 'visible'}
    return all(isinstance(n, dict) and isinstance(n.get('actions'), list) and len(n['actions']) <= 16
               and all(isinstance(a, str) and a in actions for a in n['actions'])
               and isinstance(n.get('commands'), list) and len(n['commands']) <= 16
               and all(isinstance(a, str) and a in commands for a in n['commands'])
               and integer(n.get('touch'), 0, 2 ** 45 - 1) and type(n.get('focusEnabled')) is bool
               and type(n.get('points')) is bool for n in inputs)


class ReplayHistory:
    def __init__(self, clock=time.monotonic):
        self.clock, self.lock, self.archives = clock, threading.RLock(), {}

    def prune(self):
        for key, a in list(self.archives.items()):
            if self.clock() - a['updated'] > TTL:
                del self.archives[key]

    def open(self, owner, header, game):
        if not header_valid(header, game) or len(json.dumps(header)) > 32768:
            raise ValueError('观战开局身份无效')
        with self.lock:
            self.prune()
            for watch, a in self.archives.items():
                if a['owner'] == owner:
                    if a['header'] != header:
                        raise ValueError('本局观战身份已固定')
                    return {'watch': watch, 'frames': len(a['rows'])}
            if len(self.archives) >= 16:
                raise ValueError('观战服务容量已满')
            watch = secrets.token_urlsafe(32)
            self.archives[watch] = dict(owner=owner, header=copy.deepcopy(header), rows=[], end=None, updated=self.clock(), size=0,
                                       points={}, pointBytes=0, pointsDone=False)
            return {'watch': watch, 'frames': 0}

    def push(self, owner, watch, first, rows):
        with self.lock:
            self.prune()
            a = self.archives.get(watch)
            if not a or a['owner'] != owner:
                raise PermissionError('观战发布身份无效或已过期')
            if not integer(first, 0, len(a['rows'])) or not isinstance(rows, list) or not 1 <= len(rows) <= 60 or first + len(rows) > MAX_FRAMES:
                raise ValueError('观战批次范围无效')
            if not all(row_valid(r, a['header'], first + i) for i, r in enumerate(rows)):
                raise ValueError('观战输入无效')
            if first < len(a['rows']):
                if first + len(rows) > len(a['rows']) or a['rows'][first:first + len(rows)] != rows:
                    raise ValueError('观战重传内容冲突')
                return {'frames': len(a['rows'])}
            if a['end']:
                raise ValueError('已结束的观战不能追加输入')
            size = len(json.dumps(rows, ensure_ascii=False).encode('utf-8'))
            if a['size'] + size > MAX_BYTES or sum(v['size'] for v in self.archives.values()) + size > TOTAL_BYTES:
                raise ValueError('观战历史达到容量上限')
            a['rows'].extend(copy.deepcopy(rows)); a['size'] += size; a['updated'] = self.clock()
            return {'frames': len(a['rows'])}

    def end(self, owner, watch, frame, reason):
        with self.lock:
            self.prune(); a = self.archives.get(watch)
            if not a or a['owner'] != owner:
                raise PermissionError('观战发布身份无效或已过期')
            if not integer(frame, 0, MAX_FRAMES) or frame != len(a['rows']) or not isinstance(reason, str):
                raise ValueError('观战结束边界无效')
            ending = {'frame': frame, 'reason': reason[:250]}
            if a['end'] and a['end'] != ending:
                raise ValueError('观战结束内容冲突')
            a['end'] = ending; a['updated'] = self.clock()
            return {'frames': frame}

    def read(self, watch, first):
        with self.lock:
            self.prune(); a = self.archives.get(watch)
            if not a:
                raise PermissionError('观战链接无效或已过期')
            if not integer(first, 0, len(a['rows'])):
                raise ValueError('观战读取范围无效')
            index = [dict(frame=f, sha=p['sha'], size=p['size'], **({'scene': copy.deepcopy(p['scene'])} if p['scene'] is not None else {})) for f, p in a['points'].items() if p['complete']]
            budget = max(12000, min(40000, 60000 - len(json.dumps(a['header'])) - len(json.dumps(index)) - 1024))
            rows, size = [], 0
            for row in a['rows'][first:first + 60]:
                n = len(json.dumps(row))
                if rows and size + n > budget:
                    break
                rows.append(copy.deepcopy(row)); size += n
            return dict(type='slice', header=copy.deepcopy(a['header']), first=first, rows=rows, available=len(a['rows']), end=copy.deepcopy(a['end']),
                        points=index, pointsDone=a['pointsDone'])

    def point(self, owner, watch, frame, sha, size, offset, payload, scene=None):
        with self.lock:
            self.prune(); a = self.archives.get(watch)
            if not a or a['owner'] != owner:
                raise PermissionError('检查点发布身份无效或已过期')
            if (not integer(frame, 0, len(a['rows'])) or not integer(size, 1, POINT_BYTES)
                    or not integer(offset, 0, size - 1) or not isinstance(sha, str) or not re.fullmatch('[a-f0-9]{64}', sha)
                    or not isinstance(payload, str) or len(payload) > 43692 or not scene_valid(scene)):
                raise ValueError('检查点分块元数据无效')
            try:
                chunk = base64.b64decode(payload, validate=True)
            except (ValueError, TypeError):
                raise ValueError('检查点分块编码无效')
            if not 0 < len(chunk) <= 32768 or offset + len(chunk) > size:
                raise ValueError('检查点分块范围无效')
            p = a['points'].get(frame)
            if p is None:
                if a['pointsDone'] or offset or len(a['points']) >= MAX_POINTS or any(not v['complete'] for v in a['points'].values()) or (not a['points'] and frame != 0) or (a['points'] and frame <= max(a['points'])):
                    raise ValueError('检查点顺序无效')
                if a['pointBytes'] + size > POINT_TOTAL or sum(v['pointBytes'] for v in self.archives.values()) + size > POINT_TOTAL:
                    raise ValueError('检查点归档容量已满')
                p = dict(sha=sha, size=size, data=bytearray(), complete=False, scene=copy.deepcopy(scene))
                a['points'][frame] = p; a['pointBytes'] += size
            if p['sha'] != sha or p['size'] != size or p['scene'] != scene:
                raise ValueError('检查点身份已固定')
            if offset < len(p['data']):
                if p['data'][offset:offset + len(chunk)] != chunk:
                    raise ValueError('检查点重传内容冲突')
            elif offset == len(p['data']):
                p['data'].extend(chunk)
                if len(p['data']) == size:
                    if hashlib.sha256(p['data']).hexdigest() != sha:
                        del a['points'][frame]; a['pointBytes'] -= size
                        raise ValueError('检查点摘要不一致')
                    p['complete'] = True
            else:
                raise ValueError('检查点分块缺失')
            a['updated'] = self.clock()
            return dict(offset=offset + len(chunk))

    def points_end(self, owner, watch):
        with self.lock:
            self.prune(); a = self.archives.get(watch)
            if not a or a['owner'] != owner:
                raise PermissionError('检查点发布身份无效或已过期')
            if not a['end'] or any(not p['complete'] for p in a['points'].values()):
                raise ValueError('输入或检查点尚未封存')
            a['pointsDone'] = True; a['updated'] = self.clock()
            return dict(done=True)

    def read_point(self, watch, frame, offset):
        with self.lock:
            self.prune(); a = self.archives.get(watch)
            if not a:
                raise PermissionError('观战链接无效或已过期')
            if not integer(frame, 0, MAX_FRAMES) or not integer(offset, 0, POINT_BYTES):
                raise ValueError('检查点读取范围无效')
            p = a['points'].get(frame)
            if not p or not p['complete'] or offset >= p['size']:
                raise ValueError('检查点尚未完整发布或范围无效')
            return dict(frame=frame, sha=p['sha'], size=p['size'], offset=offset,
                        payload=base64.b64encode(p['data'][offset:offset + 32768]).decode('ascii'))


HISTORY = ReplayHistory()


def handle_get(handler, rooms=None, room_lock=None, game=None, is_expired=None):
    url = urlsplit(handler.path)
    if url.path not in ('/api/replay', '/api/replay-point', '/api/replay-room'):
        return False
    try:
        q = parse_qs(url.query)
        if url.path == '/api/replay-room':
            code = q.get('room', [''])[0].upper()
            if rooms is None or room_lock is None or not re.fullmatch('[0-9]{4}', code):
                raise ValueError('请输入四位房间号')
            with room_lock:
                room = rooms.get(code)
                if not room or is_expired and is_expired(room):
                    raise PermissionError('房间不存在或已过期')
                owner = (game, code, room.get('host'), room.get('generation'), room.get('runId', 0))
                with HISTORY.lock:
                    HISTORY.prune()
                    watch = next((w for w, a in HISTORY.archives.items() if a['owner'] == owner), None)
                if not watch:
                    raise ValueError('本局尚未发布观战，请在游戏开始后重试')
                result = {'watch': watch}
        else:
            result = (HISTORY.read(q.get('watch', [''])[0], int(q.get('from', ['0'])[0])) if url.path == '/api/replay'
                  else HISTORY.read_point(q.get('watch', [''])[0], int(q.get('frame', ['-1'])[0]), int(q.get('offset', ['-1'])[0])))
        handler.reply(result)
    except PermissionError as e:
        handler.reply({'error': str(e)}, 403)
    except (ValueError, TypeError) as e:
        handler.reply({'error': str(e)}, 400)
    return True


def handle_post(handler, rooms, room_lock, game, is_expired=None):
    action = urlsplit(handler.path).path
    if not action.startswith('/api/replay/'):
        return False
    try:
        origin = handler.headers.get('Origin')
        if origin and urlsplit(origin).netloc != handler.headers.get('Host'):
            raise ValueError('来源不匹配')
        size = int(handler.headers.get('Content-Length', '0'))
        if not 0 < size <= 65536:
            raise ValueError('观战消息大小无效')
        data = json.loads(handler.rfile.read(size))
        if not isinstance(data, dict):
            raise ValueError('观战消息格式无效')
        with room_lock:
            code = str(data.get('room', '')).upper(); room = rooms.get(code)
            if not room or not room.get('host') or room['host'] != data.get('token'):
                raise PermissionError('只有本局房主可以发布观战')
            if is_expired and is_expired(room):
                raise PermissionError('房间已过期')
            if action == '/api/replay/open' and (room.get('ended') or (room.get('phase') != 'loading' and not room.get('started', False))):
                raise PermissionError('本局尚未开始或已结束')
            generation = room.get('generation')
            if generation is not None and data.get('generation') != generation:
                raise PermissionError('观战房间代次不匹配')
            run_id = data.get('runId')
            owner = (game, code, room['host'], generation, run_id)
            if type(run_id) is not int or not 1 <= run_id <= room.get('runId', 0) or (action == '/api/replay/open' and run_id != room.get('runId', 0)):
                raise PermissionError('观战本局编号不匹配')
            if action == '/api/replay/open':
                header = data.get('header')
                if not header_valid(header, game) or header['players'] != room['settings'].get('players', 2):
                    raise ValueError('观战人数或作品与本局不一致')
                host_slot = room.get('slots', room.get('seats', {})).get('host', room['settings'].get('hostSeat', 0))
                if header['hostSlot'] != host_slot:
                    raise ValueError('观战房主座位与本局不一致')
                result = HISTORY.open(owner, header, game)
            elif action == '/api/replay/push':
                result = HISTORY.push(owner, data.get('watch'), data.get('first'), data.get('rows'))
            elif action == '/api/replay/end':
                result = HISTORY.end(owner, data.get('watch'), data.get('frame'), data.get('reason'))
            elif action == '/api/replay/point':
                result = HISTORY.point(owner, data.get('watch'), data.get('frame'), data.get('sha'), data.get('size'), data.get('offset'), data.get('payload'), data.get('scene'))
            elif action == '/api/replay/points-end':
                result = HISTORY.points_end(owner, data.get('watch'))
            else:
                raise ValueError('未知观战接口')
            # Accepted live uploads are host activity too. A stalled lobby poll
            # must not expire a room whose confirmed history is still arriving.
            # Sealing an earlier run never renews the current room's lease.
            if run_id == room.get('runId') and not room.get('ended') and (room.get('phase') == 'loading' or room.get('started')):
                room['seen']['host'] = time.monotonic()
        handler.reply(result)
    except PermissionError as e:
        handler.reply({'error': str(e)}, 403)
    except (ValueError, TypeError, KeyError) as e:
        handler.reply({'error': str(e)}, 400)
    return True
