import replay_history
"""LAN lobby/signaling only. Each browser runs its own deterministic DOSBox-X."""
import argparse
import json
import secrets
import socket
import threading
import time
from functools import partial
from http.server import ThreadingHTTPServer
from web_server import AssetHandler
from pathlib import Path
from urllib.parse import urlparse, parse_qs
from turn_service import credentials as turn_credentials, TurnError

PROTOCOL='th04-rollback/4'
ROLES=('host','guest','guest2')
ROOMS={}
LOCK=threading.Lock()
TTL=120
LOADING_TTL=600


class RoomHTTPServer(ThreadingHTTPServer):
    # A tunnel multiplexes simultaneous imports/assets/polls into a burst of
    # origin connections. The default backlog of 5 rejects part of a 3P boot
    # even though handler threads are available to serve those requests.
    request_queue_size=128


def room_expired(room,now):
    lease=LOADING_TTL if room['phase']=='loading' else TTL
    return now-room['seen']['host']>lease


def snapshot(room):
    return {'protocol':PROTOCOL,'revision':room['revision'],'phase':room['phase'],'slots':room['slots'],
            'present':{r:bool(room[r]) for r in ROLES},
            'ready':room['ready'],'settings':room['settings'],'generation':room['generation'],'runId':room.get('runId',0)}


class Handler(AssetHandler):
    extensions_map={**AssetHandler.extensions_map,'.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.opus':'audio/ogg'}

    def log_message(self,*args):
        pass

    def end_headers(self):
        self.send_header('X-Content-Type-Options','nosniff')
        super().end_headers()

    def list_directory(self,path):
        self.send_error(404)

    def reply(self,value,status=200):
        body=json.dumps(value).encode()
        self.send_response(status)
        self.send_header('Content-Type','application/json')
        self.send_header('Content-Length',str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if replay_history.handle_get(self, ROOMS, LOCK, '04', lambda r: room_expired(r,time.monotonic())): return
        url=urlparse(self.path)
        if url.path=='/relay':
            from relay_server import handle
            handle(self,ROOMS,LOCK,ROLES,TTL,room_expired)
        elif url.path=='/api/events':
            self.api('events',{k:v[0] for k,v in parse_qs(url.query).items()})
        elif url.path.startswith('/api/'):
            self.reply({'error':'未知接口'},404)
        else:
            if url.path=='/':
                self.path='/lan.html'
            super().do_GET()

    def do_POST(self):
        if replay_history.handle_post(self, ROOMS, LOCK, '04', lambda r: room_expired(r,time.monotonic())): return
        try:
            origin=self.headers.get('Origin')
            if origin and urlparse(origin).netloc!=self.headers.get('Host'):
                raise ValueError('来源不匹配')
            size=int(self.headers.get('Content-Length','0'))
            if not 0<size<=65536:
                raise ValueError('消息大小无效')
            data=json.loads(self.rfile.read(size))
            if not isinstance(data,dict):
                raise ValueError('消息格式无效')
            self.api(urlparse(self.path).path.removeprefix('/api/'),data)
        except (ValueError,TypeError):
            self.reply({'error':'请求格式或来源不合法'},400)

    def api(self,action,data):
        if action=='ice':
            # Authenticate under the room lock; upstream HTTPS runs outside it.
            code=str(data.get('room','')).upper()
            with LOCK:
                room=ROOMS.get(code)
                now=time.monotonic()
                if not room or room_expired(room,now):
                    return self.reply({'error':'房间不存在或已结束，请重新建房'},404)
                role=next((r for r in ROLES if room[r] and room[r]==data.get('token')),None)
                if not role:
                    return self.reply({'error':'房间身份无效'},403)
                if room['phase']!='loading':
                    return self.reply({'error':'请在全员准备并开始后获取连接配置'},409)
                previous=room.setdefault('ice_requested',{}).get(role,0)
                if now-previous<5:
                    return self.reply({'error':'连接配置请求过于频繁，请稍后重试'},429)
                room['ice_requested'][role]=now
                identity=(code,room['generation'],role)
            try:
                config=turn_credentials(identity)
            except TurnError as error:
                return self.reply({'error':str(error)},503)
            with LOCK:
                current=ROOMS.get(code)
                if not current or current['generation']!=identity[1] or current[role]!=data.get('token'):
                    return self.reply({'error':'房间已结束或身份已失效'},403)
            return self.reply(config)
        with LOCK:
            now=time.monotonic()
            for code,room in list(ROOMS.items()):
                if room_expired(room,now):
                    del ROOMS[code]
            if action=='create':
                transport=data.get('transport','rtc')
                if transport not in ('rtc','ws'):
                    return self.reply({'error':'未知传输模式'},400)
                if data.get('protocol')!=PROTOCOL:
                    return self.reply({'error':'请刷新帧同步版网页'},400)
                if len(ROOMS)>=64:
                    return self.reply({'error':'房间数量已满'},429)
                code=f'{secrets.randbelow(10000):04d}'
                while code in ROOMS:
                    code=f'{secrets.randbelow(10000):04d}'
                rollback=data.get('rollback',False)
                if type(rollback) is not bool:
                    return self.reply({'error':'回滚设置必须为开或关'},400)
                token=secrets.token_urlsafe(24)
                ROOMS[code]={'host':token,'guest':None,'guest2':None,'queues':{r:[] for r in ROLES},
                    'transport':transport,
                    'seen':{r:now for r in ROLES},'revision':1,'phase':'lobby','slots':{r:None for r in ROLES},
                    'ready':{r:False for r in ROLES},'generation':secrets.token_hex(8),'runId':0,
                    'settings':{'p1':0,'p2':2,'p3':0,'players':2,'difficulty':1,'lives':3,'bombs':2,'language':'cn','rollback':rollback}}
                return self.reply({'room':code,'token':token,'role':'host','protocol':PROTOCOL,'state':snapshot(ROOMS[code])})
            code=str(data.get('room','')).upper()
            if len(code)!=4 or any(c not in '0123456789' for c in code):
                return self.reply({'error':'请输入四位数字房间号'},400)
            room=ROOMS.get(code)
            if not room:
                return self.reply({'error':'房间不存在或已结束，请重新建房'},404)
            if action=='join':
                if data.get('transport','rtc')!=room.get('transport','rtc'):
                    return self.reply({'error':'双方传输模式不同，请使用同一个完整联机链接'},409)
                if data.get('protocol')!=PROTOCOL:
                    return self.reply({'error':'请刷新帧同步版网页'},400)
                available=next((r for r in ROLES[1:room['settings']['players']] if not room[r]),None)
                if available is None or room['phase']!='lobby':
                    return self.reply({'error':'房间已满或本局已经开始'},409)
                room[available]=secrets.token_urlsafe(24)
                room['revision']+=1
                room['seen'][available]=now
                return self.reply({'room':code,'token':room[available],'role':available,'protocol':PROTOCOL,'state':snapshot(room)})
            token=data.get('token')
            role=next((r for r in ROLES if room[r] and room[r]==token),None)
            if not role:
                return self.reply({'error':'房间身份无效'},403)
            room['seen'][role]=now
            active=ROLES[:room['settings']['players']]
            if action in ('end','signal','drop','ready','start') and data.get('runId')!=room['runId']:
                return self.reply({'error':'本局编号已失效'},409)
            if action=='end':
                if room['phase']!='lobby':
                    room['phase']='lobby'
                    room['ready']={r:False for r in ROLES}
                    room['queues']={r:[] for r in ROLES}
                    room['startup']={}
                    room['ice_requested']={}
                    for member in ROLES:
                        if not room[member]:room['slots'][member]=None
                    room['revision']+=1
                return self.reply({'state':snapshot(room)})
            if action=='events':
                # HTTP diagnostics work before the WebRTC channel opens.
                if room['phase']=='loading' and 'boot_stage' in data:
                    step=str(data.get('boot_step','0'))
                    if step.isdigit() and 0<=int(step)<=7:
                        room.setdefault('startup',{})[role]={'stage':str(data.get('boot_stage',''))[:300],'step':int(step),'seen':now}
                events=room['queues'][role]
                room['queues'][role]=[]
                startup={r:{'stage':v['stage'],'step':v['step'],'age':round(now-v['seen'],1)} for r,v in room.get('startup',{}).items()}
                return self.reply({'events':events,'state':snapshot(room),'startup':startup})
            if action in ('seat','loadout','settings','ready'):
                if room['phase']!='lobby':
                    return self.reply({'error':'本局已经锁定，请结束后重新建房'},409)
                if action=='seat':
                    slot=data.get('slot')
                    if type(slot) is not int or slot not in range(room['settings']['players']):
                        return self.reply({'error':'请选择已开放的座位'},400)
                    if any(room['slots'][r]==slot for r in active if r!=role):
                        return self.reply({'error':'该座位已有人'},409)
                    room['slots'][role]=slot
                    room['ready']={r:False for r in ROLES}
                elif action=='loadout':
                    slot=room['slots'][role]
                    value=data.get('loadout')
                    if slot is None:
                        return self.reply({'error':'请先选择座位，再选择自机'},409)
                    if type(value) is not int or value not in range(4):
                        return self.reply({'error':'自机选择无效'},400)
                    room['settings'][f'p{slot+1}']=value
                    room['ready']={r:False for r in ROLES}
                elif action=='settings':
                    if role!='host':
                        return self.reply({'error':'只有房主可以修改本局设置'},403)
                    value=data.get('settings',{})
                    bounds={'players':(2,3),'difficulty':(0,4),'lives':(1,6),'bombs':(0,2)}
                    if not isinstance(value,dict) or set(value)!=(set(bounds)|{'rollback','language'}) or value.get('language') not in ('cn','jp') or type(value.get('rollback')) is not bool or any(type(value[k]) is not int or not lo<=value[k]<=hi for k,(lo,hi) in bounds.items()):
                        return self.reply({'error':'开局设置无效'},400)
                    if value['players']==2 and (room['guest2'] or 2 in room['slots'].values()):
                        return self.reply({'error':'请先让第三位加入者离开，并腾出 P3 座位，再关闭 3P'},409)
                    room['settings'].update(value)
                    room['ready']={r:False for r in ROLES}
                else:
                    if room['slots'][role] is None:
                        return self.reply({'error':'请先选择座位'},409)
                    room['ready'][role]=not room['ready'][role]
                room['revision']+=1
                return self.reply({'state':snapshot(room)})
            if action=='start':
                if role!='host' or room['phase']!='lobby' or not all(room[r] and room['ready'][r] for r in active) or {room['slots'][r] for r in active}!=set(range(len(active))):
                    return self.reply({'error':'需要所有已开放座位的玩家加入并准备'},409)
                room['phase']='loading'
                room['runId']+=1
                room['revision']+=1
                return self.reply({'state':snapshot(room)})
            if action=='signal':
                message=data.get('message')
                other=data.get('to')
                if other not in active or other==role or not room[other]:
                    return self.reply({'error':'信令目标无效'},400)
                if not isinstance(message,dict) or message.get('type') not in ('offer','answer','ice','ended'):
                    return self.reply({'error':'信令格式不合法'},400)
                if room['phase']!='loading':
                    return self.reply({'error':'本局尚未开始或已经结束'},409)
                if len(room['queues'][other])>=256:
                    return self.reply({'error':'对方长时间没有响应，请重新建房'},409)
                room['queues'][other].append({'from':role,'message':message})
                return self.reply({'ok':True})
            if action=='drop':
                dropped=data.get('roles')
                if role!='host' or room['phase']!='loading' or not isinstance(dropped,list) or not dropped or any(r not in active[1:] for r in dropped):
                    return self.reply({'error':'无效离线通知'},400)
                for other in dropped:
                    room[other]=None
                    room['queues'][other]=[]
                    room['ready'][other]=False
                room['revision']+=1
                return self.reply({'state':snapshot(room)})
            if action=='leave':
                if role=='host':
                    del ROOMS[code]
                elif room['phase']=='lobby':
                    room[role]=None
                    room['slots'][role]=None
                    room['queues'][role]=[]
                    room['ready']={r:False for r in ROLES}
                    room['revision']+=1
                else:
                    # Keep the locked seat. The host schedules its removal from
                    # deterministic input membership; this is not a room end.
                    room[role]=None
                    room['queues'][role]=[]
                    room['ready'][role]=False
                    room['revision']+=1
                return self.reply({'ok':True})
            self.reply({'error':'未知接口'},404)


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--port',type=int,default=9868)
    parser.add_argument('--bind',default='0.0.0.0',help='监听地址；IPv6 公网测试使用 ::')
    args=parser.parse_args()
    root=Path(__file__).resolve().parent/'web'
    try:
        bind=args.bind
        server_type=RoomHTTPServer
        if ':' in bind:
            class IPv6HTTPServer(RoomHTTPServer):
                address_family=socket.AF_INET6
            server_type=IPv6HTTPServer
        server=server_type((bind,args.port),partial(Handler,directory=str(root)))
    except OSError as error:
        raise SystemExit(f'Cannot listen on port {args.port}. Close the previous LAN server window and try again. ({error})')
    print(f'TH04 INPUT LOCKSTEP (not screen streaming) · web root: {root}',flush=True)
    print(f'A computer: http://localhost:{args.port}/lan.html',flush=True)
    addresses=sorted({item[4][0] for item in socket.getaddrinfo(socket.gethostname(),None,socket.AF_INET) if not item[4][0].startswith('127.')})
    for address in addresses:
        print(f'B computer: http://{address}:{args.port}/lan.html',flush=True)
    print('Keep this window open. Each browser loads and runs the game. LAN prototype only.',flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
