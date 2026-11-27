#!/usr/bin/env python3
"""生成 tests/golden/score-file.json：score-oracle.py 跑原版 0x4497e0 / 0x449a00 的几组用例，记 sha1。

用法：score-golden.py <th16.exe> <out.json>

用例：
  defaults   无文件 → 默认镜像 / 写出字节 / 写盘后镜像
  poked      无文件，改机体块 0 / 3 与公共块若干字段后写出
  reload     读 poked 写出的文件 → 镜像应等于 poked 写盘后镜像；再原样写出
  bad-header poked 写出的文件把 ver 改成 3 → 0x449880 丢弃，回到默认值
  unlock-all 读 poked，合计块符卡 0 挑战数改成 99999（验封顶）后跑 0x44a930 再写出
load 字段：null / "<用例>"（读该用例写出的文件）/ "<用例>:badver"
"""
import hashlib, json, pathlib, subprocess, sys, tempfile

HERE = pathlib.Path(__file__).resolve().parent

POKES = [
    '18:40420f00',                # 块 0 Easy 第 1 名分数
    '1e:4142434445464748',        # 块 0 Easy 第 1 名名字
    '515c:05000000',              # 块 0 游玩次数
    '5160:4433221100000000',      # 块 0 游戏时长
    '516c:03000000',              # 块 0 Normal 通关次数
    '5188:02000000',              # 块 0 Normal 未续关通关
    '51e5:01',                    # 块 0 练习 Normal 第 1 面到达位
    '1590:07000000',              # 块 0 符卡 20 挑战数（0x960 + 20 × 0x9c）
    '14acc:01000000',             # 块 3 Easy 未续关通关
    '19f8c:5045525355534d41',     # 公共块名字
    '19fa9:01',                   # 公共块 BGM 3 已听过
    '19fc8:efbeadde01000000',     # 公共块总时长
    '19fd0:3412',                 # 公共块 noise[0]
]

CASES = [
    {'name': 'defaults', 'load': None, 'pokes': []},
    {'name': 'poked', 'load': None, 'pokes': POKES},
    {'name': 'reload', 'load': 'poked', 'pokes': []},
    {'name': 'bad-header', 'load': 'poked:badver', 'pokes': []},
    {'name': 'unlock-all', 'load': 'poked', 'pokes': ['155c0:9f860100', 'unlock-all']},
]


def sha1(b):
    return hashlib.sha1(b).hexdigest()


def main():
    exe, out = sys.argv[1], sys.argv[2]
    saved = {}
    results = []
    with tempfile.TemporaryDirectory() as tmp:
        tmp = pathlib.Path(tmp)
        for c in CASES:
            args = [sys.executable, str(HERE / 'score-oracle.py'), exe, str(tmp / 'o.json')]
            if c['load']:
                src, _, variant = c['load'].partition(':')
                data = bytearray(saved[src])
                if variant == 'badver':
                    data[8] = 3
                path = tmp / (c['name'] + '.in')
                path.write_bytes(bytes(data))
                args += ['--load', str(path)]
            for p in c['pokes']:
                args += ['--unlock-all'] if p == 'unlock-all' else ['--poke', p]
            subprocess.run(args, check=True)
            r = json.loads((tmp / 'o.json').read_text())
            saved[c['name']] = bytes.fromhex(r['saved'])
            results.append({**c, 'rng_seed': r['rng_seed'],
                            'image': sha1(bytes.fromhex(r['image'])),
                            'saved': sha1(saved[c['name']]), 'saved_size': len(saved[c['name']]),
                            'image_after_save': sha1(bytes.fromhex(r['image_after_save']))})
    pathlib.Path(out).write_text(json.dumps({'cases': results}, indent=1) + '\n')


if __name__ == '__main__':
    main()
