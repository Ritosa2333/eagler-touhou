"""Stage and seal the complete Windows TH09 development handoff (no live state)."""
from pathlib import Path
import hashlib, json, os, shutil, sys, zipfile

workspace = Path(__file__).resolve().parents[2]
if os.name == 'nt' and not str(workspace).startswith('\\\\?\\'):
    workspace = Path('\\\\?\\' + str(workspace))
name = 'TH09-20260921-development'
stage = workspace / 'artifacts' / (name + '-stage')
archive = workspace / (name + '.zip')
skip = {'.git', '.codex', '.agents', '__pycache__', '.cache', 'npm-cache', 'objects', 'rebuild-check'}

def digest(p):
    with p.open('rb') as f: return hashlib.file_digest(f, 'sha256').hexdigest()

def copy_file(source, target):
    dest = stage / target
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, dest)

def copy_tree(source, target, exclude=()):
    if not source.is_dir(): raise RuntimeError('Missing dependency: ' + str(source))
    for current, dirs, files in os.walk(source):
        dirs[:] = sorted(d for d in dirs if d not in skip and d not in exclude)
        for n in sorted(files):
            p = Path(current) / n
            if n.startswith('.env') or n.lower() in {'cloudflared-token.txt', 'processes.json', 'debug.log'} or p.suffix in {'.pyc', '.tmp', '.log'}: continue
            copy_file(p, Path(target) / p.relative_to(source))

def write(name, text):
    dest=stage/name; dest.parent.mkdir(parents=True,exist_ok=True)
    dest.write_text(text, encoding='utf-8', newline='\r\n' if name.endswith('.cmd') else '\n')

def prepare(resume=False):
    if archive.exists() or (stage.exists() and not resume): raise RuntimeError('Delivery already exists; refusing overwrite')
    stage.mkdir(parents=True, exist_ok=resume)
    game=workspace/'th09_web'
    for d in ['cpp','sdl-runtime','scripts','tests','launcher','docs','licenses','assets','reference','node_modules']:
        copy_tree(game/d,'th09_web/'+d)
    for f in ['package.json','package-lock.json','README.md','target.json','THIRD-PARTY-NOTICES.txt','启动花映塚网页版.cmd']:
        copy_file(game/f,'th09_web/'+f)
    for d in ['site','scripts','node_modules','verification']:
        copy_tree(game/'artifacts/sdl-release'/d,'th09_web/artifacts/sdl-release/'+d)
    for f in ['th09.wasm','th09.mjs','build.json','package.json','README.md','启动花映塚网页版.cmd']:
        copy_file(game/'artifacts/sdl-release'/f,'th09_web/artifacts/sdl-release/'+f)
    for d in ['sdl','input']: copy_tree(workspace/'portable'/d,'portable/'+d)
    for d in ['verification','analysis/jp']: copy_tree(game/'artifacts/cpp'/d,'th09_web/artifacts/cpp/'+d)
    for f in ['game-core-test.wasm','game-core-test-build.json']: copy_file(game/'artifacts/cpp'/f,'th09_web/artifacts/cpp/'+f)
    copy_file(game/'artifacts/bugfix-20260920/title-card.png','th09_web/artifacts/bugfix-20260920/title-card.png')
    for f in ['regression.json','launcher-acceptance.json','launcher-files.json']:
        copy_file(game/'artifacts/bugfix-20260920'/f,'verification/'+f)
    copy_file(game/'artifacts/replay-regression/continuous.json','verification/continuous.json')
    for f in ['replay-regression-current.log','title-current-check.log','stage7-final.log']:
        copy_file(game/'artifacts'/f,'verification/'+f)
    original=workspace/'[th09] 东方花映塚 (日文版)'
    for f in ['th09.exe','th09.dat','thbgm.dat']:
        copy_file(original/f,original.name+'/'+f)
    copy_file(original/'replay/th9_udyt22.rpy',original.name+'/replay/th9_udyt22.rpy')
    print('Game sources, reference assets, release and verification staged.',flush=True)
    for d in ['emsdk','architecture/typescript','architecture/python']: copy_tree(workspace/'tools'/d,'tools/'+d)
    for d in ['wasi-sdk-34.0-x86_64-windows','decomp']: copy_tree(workspace/'th10_web/tools'/d,'th10_web/tools/'+d)
    for f in ['node.exe','Node.LICENSE']: copy_file(workspace/'th10_web/tools'/f,'th10_web/tools/'+f)
    copy_file(workspace/'th10_web/scripts/native/browser-launch.mjs','th10_web/scripts/native/browser-launch.mjs')
    for d in ['playwright','playwright-core']: copy_tree(workspace/'th10_web/node_modules'/d,'th10_web/node_modules/'+d)
    copy_tree(workspace/'th08_web/node_modules/@alexaltea/unicorn-js','th08_web/node_modules/@alexaltea/unicorn-js')
    for f in ['cp932.bin','blend.bin','msgothic.ttc']: copy_file(workspace/'th08_web/assets/sdl-native'/f,'th08_web/assets/sdl-native/'+f)
    py=Path(sys.base_prefix)
    for p in py.iterdir():
        if p.is_file() and (p.suffix in {'.exe','.dll'} or p.name.startswith('LICENSE')): copy_file(p,'tools/python/'+p.name)
    for d in ['DLLs','Lib','libs','include']: copy_tree(py/d,'tools/python/'+d,exclude=('site-packages',))
    site=py/'Lib/site-packages'
    for p in site.iterdir():
        if p.name.startswith(('numpy','cffi','pycparser','_cffi_backend')):
            if p.is_dir(): copy_tree(p,'tools/python/Lib/site-packages/'+p.name)
            else: copy_file(p,'tools/python/Lib/site-packages/'+p.name)
    browsers=Path(os.environ['LOCALAPPDATA'])/'ms-playwright'
    for p in browsers.iterdir():
        if p.is_dir() and p.name.startswith(('chromium_headless_shell-','ffmpeg-')): copy_tree(p,'tools/playwright-browsers/'+p.name)
    copy_file(game/'scripts/handoff/prepare-toolchain.py','tools/prepare-toolchain.py')
    write('tools/emsdk/.emscripten',"import os\n_sdk = os.environ['EMSDK']\nLLVM_ROOT = os.path.join(_sdk, 'install', 'bin')\nBINARYEN_ROOT = os.path.join(_sdk, 'install')\nNODE_JS = [os.path.abspath(os.path.join(_sdk, '..', '..', 'th10_web', 'tools', 'node.exe'))]\nCACHE = os.path.join(_sdk, 'install', 'emscripten', 'cache')\nEMSCRIPTEN_ROOT = os.path.join(_sdk, 'install', 'emscripten')\n")
    write('开发环境.cmd','@echo off\nset "PATH=%~dp0tools\\python;%~dp0th10_web\\tools;%PATH%"\nset "PLAYWRIGHT_BROWSERS_PATH=%~dp0tools\\playwright-browsers"\nset "EMSDK=%~dp0tools\\emsdk"\nset "EM_CONFIG=%~dp0tools\\emsdk\\.emscripten"\nset "PYTHONNOUSERSITE=1"\n"%~dp0tools\\python\\python.exe" "%~dp0tools\\prepare-toolchain.py"\n')
    write('启动花映塚网页版.cmd','@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\ncd /d "%~dp0"\necho 请打开 http://127.0.0.1:8096/ ，保持本窗口开启。\n"%~dp0th10_web\\tools\\node.exe" "%~dp0th09_web\\artifacts\\sdl-release\\scripts\\serve.mjs" --port 8096\npause\n')
    write('重新编译花映塚.cmd','@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\ncd /d "%~dp0th09_web"\nnode scripts\\build-sdl.mjs --release\nif errorlevel 1 goto failed\nnode scripts\\package-release.mjs\nif errorlevel 1 goto failed\necho 编译及打包完成。\npause\nexit /b 0\n:failed\necho 编译失败，请保留上方错误。\npause\nexit /b 1\n')
    write('验证录像修复.cmd','@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\ncd /d "%~dp0th09_web"\nnode tests\\browser\\supplied-replay-release-check.mjs\nif errorlevel 1 goto failed\necho 用户录像在发布版连续回放验证通过。\npause\nexit /b 0\n:failed\necho 验证失败，请保留输出。\npause\nexit /b 1\n')
    write('README-交付说明.md', '''# 花映塚完整开发交付 · 2026-09-21

与此前永夜抄、风神录完整开发包相同：本包同时包含可运行网站、完整项目源码、共享渲染与输入代码、原版对照材料、素材、测试和 Windows x64 开发工具。th08_web / th10_web 目录仅包含花映塚引用的共享依赖，不是另外两款完整游戏。

## 测试组游玩

完整解压到可写的较短路径（例如 E:\\TH09），双击根目录“启动花映塚网页版.cmd”，访问 http://127.0.0.1:8096/ 。保持窗口开启。工具链包含长文件名，建议使用支持长路径的解压工具。已附 Node，无需安装 Node 或 Python。不要在 ZIP 内直接运行，也不要只提取 th09_web 子目录。局域网手机一般缺少 HTTPS 安全上下文，可将发布服务接到自己的 HTTPS 反向代理测试。

## 重新开发与验证

双击“重新编译花映塚.cmd”，使用包内 Emscripten 6.0.9、SDL3 ports 和 TypeScript 重建。构建路径已改成随解压目录定位，不需要原作者 E 盘路径。首次构建会生成中间文件。附带已缓存的 SDL3 / SDL3_ttf 依赖。

“验证录像修复.cmd”使用包内无头 Chromium，通过正式网页录像管理器导入 th9_udyt22.rpy 并连续回放九关。源码、脚本或游戏逻辑修改后可再次执行。更广泛原版对照：先在 cmd 执行 `call 开发环境.cmd`，然后 `cd th09_web`，执行 `node scripts/cpp/build.mjs` 和 `node --test tests/cpp/session-replays.test.mjs`。Ghidra/JDK、WASI SDK、Unicorn、原版 1.50a EXE 和解包 reference 也已包含。

本包已有原版素材与 OGG；如需重新转码，在上述开发环境运行 `python scripts/prepare-assets.py`。不含玩家的 score.dat、th09.cfg、其他私人录像或 Cloudflare 隧道凭据；唯一外部测试录像是本次指定样本。

## 目录

- th09_web：全部 C++、网页启动器、平台层、测试、脚本、素材与 reference。
- portable/sdl、portable/input：与永夜抄 / 风神录共用的渲染和触控源码。
- th09_web/artifacts/sdl-release：与本轮公网相同游戏构建的可运行站点及白名单服务器；开发文件不会由此服务器公开。
- tools、th10_web/tools：开发工具；仅运行游戏无需加载这些编译器。
- verification 及 th09_web/artifacts/*/verification：本次验证记录。游戏修复与验证边界详见 th09_web/docs/2026-09-21-录像修复与复核.md。
- FILE-SHA256.json：交付文件逐项摘要；根目录 ZIP 另有整体 SHA-256。

这是完整测试/开发交付，不是只含自主代码的开源仓库。原版资源和第三方组件保留各自归属及许可，见 THIRD-PARTY-NOTICES 和各工具许可证。已有历史报告保留原始测试范围；未把一份样本通过描述为所有路线已证明 1:1。
''')
    print('Toolchains and relocatable entry scripts staged.',flush=True)

def seal():
    if archive.exists(): raise RuntimeError('Refuse to replace existing ZIP')
    files=sorted(p for p in stage.rglob('*') if p.is_file() and not any(x in skip for x in p.relative_to(stage).parts) and p.suffix not in {'.pyc','.tmp'})
    files=[p for p in files if p.name!='FILE-SHA256.json']
    sums={p.relative_to(stage).as_posix():digest(p) for p in files}
    write('FILE-SHA256.json',json.dumps(sums,ensure_ascii=False,indent=2)+'\n')
    files.append(stage/'FILE-SHA256.json')
    print('Sealed',len(files),'files; compressing...',flush=True)
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=6,allowZip64=True) as z:
        for p in files: z.write(p,name+'/'+p.relative_to(stage).as_posix())
    print('Checking every compressed entry against its SHA-256...',flush=True)
    with zipfile.ZipFile(archive) as z:
        for n,h in sums.items():
            with z.open(name+'/'+n) as f:
                if hashlib.file_digest(f,'sha256').hexdigest()!=h: raise RuntimeError('Archive readback mismatch: '+n)
    h=digest(archive)
    archive.with_suffix('.zip.sha256').write_text(h+'  '+archive.name+'\n',encoding='ascii')
    result={'archive':archive.name,'bytes':archive.stat().st_size,'entries':len(files),'sha256':h,'readbackVerified':True}
    (workspace/'artifacts/development-delivery-th09-20260921.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result),flush=True)

if __name__=='__main__':
    if sys.argv[1:] == ['stage']: prepare()
    elif sys.argv[1:] == ['resume-stage']: prepare(resume=True)
    elif sys.argv[1:] == ['archive']: seal()
    else: raise SystemExit('Use stage or archive')
