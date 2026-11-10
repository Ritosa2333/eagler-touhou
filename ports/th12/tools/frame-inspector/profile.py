"""TH12-owned retail layout. Read only at the completed main-loop boundary."""
import hashlib
import struct

EXE_SHA256 = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
COMMON_REVISION = 'cedb7101696b6f8f200a45814c594ec0f1052761'
# 44f8af follows all three original Update/Draw/Present branches. No skip
# is enforced in the isolated copy's configuration; no instruction is patched.
FRAME_BOUNDARY = 0x44f8af
FRAME_SIGNATURE = bytes.fromhex('89 44 24 0c')

def prepare_config(data):
    if len(data) != 60 or struct.unpack_from('<I', data)[0] != 0x120001:
        raise ValueError('TH12 1.00b configuration must be valid (60 bytes).')
    if any(data[26+i] >= n for i, n in enumerate((2, 3, 2, 4, 3, 3))):
        raise ValueError('Invalid original configuration options.')
    result = bytearray(data)
    result[29] = 1  # 4ceacd: windowed
    result[30] = 0  # 4ceace: no skipped Draw/Present frames
    return bytes(result)

def capture(reader):
    def value(address, fmt='i'):
        return reader.unpack('<'+fmt, address)[0]
    stage = value(0x4b0cb0)
    player = value(0x4b4514, 'I')
    gui = value(0x4b43e4, 'I')
    spell = value(0x4b43cc, 'I')
    replay = value(0x4b4518, 'I')
    row = {'stage': stage, 'scoreStored': value(0x4b0c44),
           'rng': list(reader.unpack('<II', 0x4ce568)),
           'piv': value(0x4b0c78), 'graze': value(0x4b0cdc),
           'power': value(0x4b0c48), 'replayActive': False,
           'replayCursor': None, 'displayScoreStored': None,
           'playerFixed': None, 'spell': {'card':0,'flags':0,'age':0},
           'bossLife':0,'bossPhaseAge':0}
    if replay and 1 <= stage <= 7:
        row['replayActive'] = value(replay+0x10) == 1
        if row['replayActive']:
            row['replayCursor'] = value(replay+stage*36+0xbc)
            row['totalReplayCursor'] = value(replay+0x1d0)
    if gui:
        row['displayScoreStored'] = value(gui+0x6cdc)
        row['scoreDisplayStep'] = value(gui+0x6ce0)
    if player:
        row['playerFixed'] = list(reader.unpack('<ii', player+0x988))
        row['playerState'] = value(player+0xa28)
        row['playerStateAge'] = value(player+0xa34)
        row['playerClock'] = value(player+0xa48)
        row['invulnerability'] = value(player+0xc404)
    if spell:
        row['spell'] = {'card': value(spell+0x78),
                        'flags': value(spell+0x7c, 'I'),
                        'age': value(spell+0x28)}
    manager=value(0x4b43dc,'I')
    if manager:
        node=value(manager+0x68,'I');visited=set()
        while node:
            if node in visited or len(visited)>=8192:raise ValueError('Enemy链表无效')
            visited.add(node)
            enemy,next_node=reader.unpack('<II',node)
            if value(enemy+0x26f8,'I')&0x400000:
                row['bossLife']=value(enemy+0x2648)
                row['bossPhaseAge']=value(enemy+0x12b0)
                break
            node=next_node
    return row

def replay_identity(reader,stage):
    replay=reader.unpack('<I',0x4b4518)[0]
    if not replay or not 1<=stage<=7:raise ValueError('原版尚未进入录像')
    base=reader.unpack('<I',replay+stage*36+0xa8)[0]
    header=reader.unpack('<I',replay+stage*36+0xb8)[0]
    raw=reader.read(header,160)
    number,seed,frames,payload=struct.unpack_from('<HHII',raw)
    if number!=stage or not 1<=frames<=600000 or not frames*6<=payload<=64*1024*1024:raise ValueError('原版录像数据无效')
    return {'stage':stage,'frames':frames,'stageSha256':hashlib.sha256(raw+reader.read(base,payload)).hexdigest()}

def verify_executable(path):
    if hashlib.sha256(path.read_bytes()).hexdigest() != EXE_SHA256:
        raise ValueError('Unsupported EXE: expected original TH12 1.00b.')
