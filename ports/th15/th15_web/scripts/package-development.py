"""Create the relocatable TH15 complete development ZIP used by the test group."""
from pathlib import Path
import hashlib, json, os, shutil, sys, zipfile
from datetime import datetime
root = Path(__file__).resolve().parents[2]
if os.name == 'nt': root = Path('\\\\?\\' + str(root))
game = root / 'th15_web'
meta = json.loads((game/'reference/web-release.json').read_text(encoding='utf-8'))
name = 'TH15-'+meta['version']+'-development-'+datetime.now().strftime('%Y%m%d')
stage = root/'artifacts'/(name+'-stage')
archive = root/(name+'.zip')
skip = {'.git','.codex','.agents','__pycache__','.cache','npm-cache','objects','objects-debug','project','downloads'}
private = {'debug.log','processes.json','server-process.json','cloudflared-token.txt','.emscripten','scoreth15.dat','th15.cfg'}
def digest(p):
    with p.open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()
def copy(p,n):
    if not p.is_file(): raise RuntimeError('Missing file: '+str(p))
    dest=stage/n; dest.parent.mkdir(parents=True,exist_ok=True)
    if dest.is_file() and dest.stat().st_size==p.stat().st_size and dest.stat().st_mtime_ns==p.stat().st_mtime_ns: return
    shutil.copy2(p,dest)
def tree(p,n,exclude=(),predicate=None):
    if not p.is_dir(): raise RuntimeError('Missing directory: '+str(p))
    for current,dirs,files in os.walk(p):
        dirs[:]=sorted(d for d in dirs if d not in skip and d not in exclude)
        (stage/n/Path(current).relative_to(p)).mkdir(parents=True,exist_ok=True)
        for f in sorted(files):
            q=Path(current)/f
            if f.startswith('.env') or f.lower() in private or q.suffix.lower() in {'.pyc','.tmp','.log','.pem','.key','.pfx'}: continue
            if predicate and not predicate(q): continue
            if q.is_symlink(): raise RuntimeError('Unexpected file symlink: '+str(q))
            copy(q,Path(n)/q.relative_to(p))
def write(n,t):
    p=stage/n;p.parent.mkdir(parents=True,exist_ok=True)
    p.write_text(t,encoding='utf-8',newline='\r\n' if str(n).endswith('.cmd') else '\n')
def prepare(resume=False):
    if archive.exists() or (stage.exists() and not resume): raise RuntimeError('Refusing overwrite')
    stage.mkdir(parents=True,exist_ok=resume)
    for d in ['cpp','sdl-runtime','scripts','tests','launcher','docs','assets','reference','tools']: tree(game/d,'th15_web/'+d)
    for n in ['README.md','target.json','CHANGELOG.txt','启动绀珠传网页版.cmd']: copy(game/n,'th15_web/'+n)
    release=game/'artifacts/sdl-release'
    for d in ['site','scripts','node_modules']: tree(release/d,'th15_web/artifacts/sdl-release/'+d)
    for n in ['th15.wasm','th15.mjs','build.json','package.json']: copy(release/n,'th15_web/artifacts/sdl-release/'+n)
    for n in ['THIRD-PARTY-NOTICES.txt','LICENSE-launcher.txt']: copy(release/'site'/n,n)
    for d in ['sdl','input']: tree(root/'portable'/d,'portable/'+d)
    for d in ['analysis','verification']: tree(game/'artifacts/cpp'/d,'th15_web/artifacts/cpp/'+d)
    for n in ['game-core-test.wasm','game-core-test-build.json','verified-modules-summary.json']: copy(game/'artifacts/cpp'/n,'th15_web/artifacts/cpp/'+n)
    for d in ['sdl-application','sdl-audio','sdl-ending','sdl-graphics','sdl-store','sdl-title','fps-20261005','game-performance-20261004','stage5-performance-20261004','restart-background-20261004']:
        tree(game/'artifacts'/d,'th15_web/artifacts/'+d,exclude=('site','scripts','node_modules'))
    for d in ['native-process-extra','native-process-replay','tester-replay-20261003']:
        tree(game/'artifacts'/d,'th15_web/artifacts/'+d,exclude=('native-extra','native-reisen','native-sanae','replay'),predicate=lambda p:p.suffix in {'.bin','.json','.rpy','.mjs','.ps1','.asm'})
    baseline=json.loads((game/'reference/architecture-baseline.json').read_text(encoding='utf-8'))
    changes={e['path']:e for e in json.loads((game/'reference/shared-architecture-extensions.json').read_text(encoding='utf-8'))['changes']}
    for e in baseline['sharedFiles']:
        if digest(root/e['path'])!=changes.get(e['path'],{}).get('currentSha256',e['sha256']): raise RuntimeError('Shared source drift: '+e['path'])
        copy(root/e['path'],e['path'])
    for n in ['release-server.mjs','netplay-relay.mjs']: copy(root/'th09_web/scripts'/n,'th09_web/scripts/'+n)
    tree(root/'th09_web/node_modules/ws','th09_web/node_modules/ws')
    for d in ['emsdk','architecture/typescript','architecture/python']: tree(root/'tools'/d,'tools/'+d)
    for d in ['wasi-sdk-34.0-x86_64-windows','decomp']: tree(root/'th10_web/tools'/d,'th10_web/tools/'+d)
    for n in ['node.exe','Node.LICENSE']: copy(root/'th10_web/tools'/n,'th10_web/tools/'+n)
    copy(root/'th10_web/scripts/native/browser-launch.mjs','th10_web/scripts/native/browser-launch.mjs')
    copy(root/'th11_web/scripts/native/font-oracle.mjs','th11_web/scripts/native/font-oracle.mjs')
    for d in ['playwright','playwright-core']: tree(root/'th10_web/node_modules'/d,'th10_web/node_modules/'+d)
    for d in ['@alexaltea/unicorn-js','koffi','@koromix']: tree(root/'th08_web/node_modules'/d,'th08_web/node_modules/'+d)
    copy(root/'th08_web/package.json','th08_web/package.json')
    original=root/'[th15] 东方绀珠传 (汉化版+日文版)'
    for n in ['th15.exe','th15c.exe','th15.dat','thbgm.dat','custom.exe','d3dx9_43.dll','th15.sdb','readme.txt','readme(utf8).txt','readme(zh).txt','omake.txt','omake(utf8).txt','omake(zh).txt','Staff与汉化说明.txt']: copy(original/n,original.name+'/'+n)
    copy(original/'replay/th15_ud0186.rpy',original.name+'/replay/th15_ud0186.rpy')
    py=Path(sys.base_prefix)
    for p in py.iterdir():
        if p.is_file() and (p.suffix in {'.exe','.dll'} or p.name.startswith('LICENSE')): copy(p,'tools/python/'+p.name)
    for d in ['DLLs','Lib','libs','include']: tree(py/d,'tools/python/'+d,exclude=('site-packages',))
    for p in (py/'Lib/site-packages').iterdir():
        if p.name.startswith(('numpy','cffi','pycparser','_cffi_backend')):
            dest='tools/python/Lib/site-packages/'+p.name
            tree(p,dest) if p.is_dir() else copy(p,dest)
    for p in (Path(os.environ['LOCALAPPDATA'])/'ms-playwright').iterdir():
        if p.is_dir() and p.name.startswith(('chromium_headless_shell-','ffmpeg-')): tree(p,'tools/playwright-browsers/'+p.name)
    copy(root/'th09_web/scripts/handoff/prepare-toolchain.py','tools/prepare-toolchain.py')
    write('tools/emsdk/.emscripten',"import os\n_sdk = os.environ['EMSDK']\nLLVM_ROOT = os.path.join(_sdk, 'install', 'bin')\nBINARYEN_ROOT = os.path.join(_sdk, 'install')\nNODE_JS = [os.path.abspath(os.path.join(_sdk, '..', '..', 'th10_web', 'tools', 'node.exe'))]\nCACHE = os.path.join(_sdk, 'install', 'emscripten', 'cache')\nEMSCRIPTEN_ROOT = os.path.join(_sdk, 'install', 'emscripten')\n")
    write('开发环境.cmd','@echo off\nset "PATH=%~dp0tools\\python;%~dp0th10_web\\tools;%PATH%"\nset "PLAYWRIGHT_BROWSERS_PATH=%~dp0tools\\playwright-browsers"\nset "EMSDK=%~dp0tools\\emsdk"\nset "EM_CONFIG=%~dp0tools\\emsdk\\.emscripten"\nset "PYTHONNOUSERSITE=1"\n"%~dp0tools\\python\\python.exe" "%~dp0tools\\prepare-toolchain.py"\n')
    write('启动绀珠传网页版.cmd','@echo off\nchcp 65001 >nul\nsetlocal\ncd /d "%~dp0"\necho 请打开 http://127.0.0.1:8115/ ，保持本窗口开启。\n"%~dp0th10_web\\tools\\node.exe" "%~dp0th15_web\\artifacts\\sdl-release\\scripts\\serve.mjs" --port 8115\necho 服务已停止；如失败请保留上方错误。\npause\n')
    prefix='@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\nif errorlevel 1 goto failed\ncd /d "%~dp0"\n'
    suffix='echo Completed.\npause\nexit /b 0\n:failed\necho Failed; retain the error above.\npause\nexit /b 1\n'
    write('重新编译绀珠传.cmd',prefix+'node th15_web\\scripts\\build-sdl-application.mjs --release\nif errorlevel 1 goto failed\nnode th15_web\\scripts\\package-release.mjs\nif errorlevel 1 goto failed\n'+suffix)
    write('验证开发包.cmd',prefix+'node th15_web\\scripts\\verify-delivery.mjs\nif errorlevel 1 goto failed\n'+suffix)
    write('deploy/server.mjs',"import {releaseServer} from '../th15_web/artifacts/sdl-release/scripts/serve.mjs';\nimport {fileURLToPath} from 'node:url';\nconst port=Number(process.env.PORT||3007),host=process.env.HOST||'127.0.0.1';\nif(!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid PORT');\nconst result=await releaseServer({root:fileURLToPath(new URL('../th15_web/artifacts/sdl-release/site',import.meta.url)),port,host});\nconsole.log('TH15 '+result.manifest.webVersion+' '+result.manifest.version+' '+result.url);\n")
    manifest=json.loads((release/'site/manifest.json').read_text(encoding='utf-8'))
    write('README-交付说明.md',f'''# 绀珠传 WEB {meta['version']} 完整开发交付 · {meta['date']}

沿用永夜抄、风神录的完整开发包形式：可运行网站、完整 C++ 与前端源码、共享 SDL3/WebGL2 渲染／输入源码、原版素材、测试与原版对照工具，以及 Windows x64 编译工具链。

完整解压到较短可写目录（例如 E:/TH15），双击根目录“启动绀珠传网页版.cmd”，打开 http://127.0.0.1:8115/ ，保持窗口开启。不要在 ZIP 内运行或只解压 th15_web。本地测试端口避免与现有公网 3007 服务冲突。

本包为最新 {meta['version']}：原版右下角 FPS、默认 1280×960 与启动器 640×480 切换、全关卡性能优化、录像排序／Extra 掉落／Restart 背景以及此前测试组修复。完整更新记录见 CHANGELOG.txt。游戏逻辑基于日文原版 1.00b；附带汉化 EXE 作为原版对照材料，不代表网页版已汉化。

双击“重新编译绀珠传.cmd”用包内 Python、Node、Emscripten／SDL3 缓存及 TypeScript 重建网站。已附 WASI SDK、Unicorn、Playwright Chromium、Ghidra/JDK、Capstone 和音频转换依赖。原版字体重新采样需要 Windows 原日文字体；常规构建直接使用附带的测量字形。命令见 handoff/BUILD-AND-TEST.md。

保留完整目录结构；th08_web、th09_web、th10_web、th11_web 为工具与共享依赖，不是其他游戏的完整开发包。Windows x64 工具链可离线构建；Linux 部署已构建网站只需 Node，重新编译需配置同版本的 Linux 工具链。

部署说明见 deploy/部署说明.md。版本和发布身份见 th15_web/reference/release-acceptance.json。“验证开发包.cmd”检查发布文件与源码，FILE-SHA256.json 提供全部交付文件校验。verification 记录本次独立目录启动与重建结果；旧游戏对照报告保留原始构建哈希，不代表每个补丁重跑所有路线。

历史调试候选站点、临时服务器记录、维护者浏览器存档和个人录像目录没有打包；reference/replays 及原版 replay/th15_ud0186.rpy 保留已记录／用户提供的开发样本。玩家存档及录像存于各自浏览器。开发包不包含 Cloudflare 凭据。手机模拟测试不能代替测试组实际手机驱动与帧率检查。

游戏、字体和第三方工具保留各自归属与许可。这是包含原版素材和工具链的测试／开发交付。
''')
    copy(game/'CHANGELOG.txt','CHANGELOG.txt')
    write('deploy/部署说明.md',f'''# 绀珠传 WEB {meta['version']} 测试环境部署

Windows 根目录运行 th10_web/tools/node.exe deploy/server.mjs。
Linux 安装 Node.js 22 或兼容更新版本，根目录运行 node deploy/server.mjs。
最小运行目录：deploy/server.mjs 和完整 th15_web/artifacts/sdl-release/{{site,scripts,node_modules}}，保留相对位置。

默认监听 127.0.0.1:3007。环境变量 PORT 可改端口；容器／跨主机代理可设 HOST=0.0.0.0。测试组使用自己的 HTTPS 域名与反向代理，资源缓存和离线功能需要 HTTPS。

只公开 site 或附带的清单白名单服务器，不要公开完整开发目录。保留源站 MIME、COOP、COEP、CORP、CSP、Range、Cache-Control 响应头。manifest.json、version.json、app-shell-sw.js 按源站策略更新，勿额外强制全站缓存。先完整上传新版本，再切换目录并重启服务，避免清单与资源混用。

更新核对 /manifest.json：game=th15，webVersion={meta['version']}，version={manifest['version']}。
Wasm SHA-256：{manifest['execution']['sha256']}。

玩家退出游戏后刷新，并按更新提示操作；无需清空存档。测试新浏览器和旧缓存的启动／更新、音频、原版录像、Extra、连续触控、导入导出、分辨率按钮。包内不带作者的隧道账号或令牌。
''')
    write('handoff/BUILD-AND-TEST.md','''# 构建与测试

在完整解压后的根目录，用 cmd 执行 call 开发环境.cmd，再运行：

    node th15_web/scripts/build-sdl-application.mjs --release
    node th15_web/scripts/package-release.mjs
    node th15_web/scripts/verify-delivery.mjs

更新 th15_web/artifacts/sdl-release 并校验源码、共享架构、生产导出、所有资源、HTTP 与部署入口。

原版对照核心和冻结模块回归：

    node th15_web/scripts/cpp/build.mjs
    node th15_web/scripts/cpp/test-suite.mjs delivery-local

集成开发夹具与真实生产启动器测试：

    node th15_web/scripts/build-sdl-application.mjs
    node th15_web/scripts/build-sdl-graphics.mjs
    set TH15_BROWSER_SITE=artifacts/sdl-release/site
    node --test th15_web/tests/sdl/production-launcher-browser.test.mjs

开发夹具含测试导出，不能正式部署。上述 TH15_BROWSER_SITE 指向当前发布目录，检查桌面和模拟手机的真实 RAF、游戏、暂停、返回标题与浏览器本地存档。完整模块／长录像对照耗时较长，范围以各脚本输出为准。

历史报告保留原版本哈希，当前发布身份以 reference/web-release.json、reference/release-acceptance.json 和 sdl-release/site/manifest.json 为准。源码绝对路径可能写入 Wasm，异地重建二进制不强求字节相同，需检查源码一致性与重建运行结果。
''')
    write('.gitignore','tools/\nth10_web/tools/\n**/node_modules/\n**/objects/\n**/__pycache__/\n**/.cache/\n*.zip\n*.log\n')
    build=json.loads((release/'build.json').read_text(encoding='utf-8'))
    for n,h in {**build['sources'],**build['headers']}.items():
        if digest(stage/n)!=h: raise RuntimeError('Release source mismatch: '+n)
    missing=[]
    for d in ['cpp','sdl-runtime','scripts','tests','launcher']:
        for base,dirs,files in os.walk(game/d):
            dirs[:]=[d for d in dirs if d not in skip]
            for f in files:
                p=Path(base)/f
                if p.suffix in {'.cpp','.hpp','.h','.inc','.ts','.mjs','.js','.py','.ps1','.cmd','.java'} and not (stage/p.relative_to(root)).is_file(): missing.append(p.relative_to(root).as_posix())
    if missing: raise RuntimeError('Missing source: '+repr(missing))
    write('handoff/source-coverage.json',json.dumps({'passed':True,'missing':missing,'scope':'All TH15 C++ / runtime / launcher / scripts / tests; shared SDL3/input; source hashes match published build'},indent=2)+'\n')
    write('handoff/config.json',json.dumps({'game':'th15','webVersion':meta['version'],'releaseDate':meta['date'],'manifestVersion':manifest['version'],'productionWasmSha256':manifest['execution']['sha256'],'archiveRoot':name},indent=2)+'\n')
    print(json.dumps({'stage':str(stage),'status':'prepared','version':meta['version']}),flush=True)
def seal():
    if archive.exists(): raise RuntimeError('Refusing overwrite')
    for f in ['distribution.json','relocated-development.json']:
        if not json.loads((stage/'verification'/f).read_text(encoding='utf-8'))['passed']: raise RuntimeError('Delivery checks required')
    files=sorted(p for p in stage.rglob('*') if p.is_file() and not any(x in skip for x in p.relative_to(stage).parts) and p.suffix not in {'.pyc','.tmp'} and p.name!='FILE-SHA256.json')
    sums={p.relative_to(stage).as_posix():digest(p) for p in files}
    write('FILE-SHA256.json',json.dumps(sums,ensure_ascii=False,indent=2)+'\n');files.append(stage/'FILE-SHA256.json')
    empty=sorted(p for p in stage.rglob('*') if p.is_dir() and not any(p.iterdir()) and not any(x in skip for x in p.relative_to(stage).parts))
    print('Compressing',len(files),'files',flush=True)
    with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=6,allowZip64=True) as z:
        for p in empty:z.write(p,name+'/'+p.relative_to(stage).as_posix()+'/')
        for i,p in enumerate(files):
            z.write(p,name+'/'+p.relative_to(stage).as_posix())
            if i and i%5000==0: print('Compressed',i,'/',len(files),flush=True)
    print('Reading back all ZIP entries and SHA-256',flush=True)
    with zipfile.ZipFile(archive) as z:
        for n,h in sums.items():
            with z.open(name+'/'+n) as f:
                if hashlib.file_digest(f,'sha256').hexdigest()!=h: raise RuntimeError('ZIP mismatch: '+n)
        if json.loads(z.read(name+'/FILE-SHA256.json'))!=sums: raise RuntimeError('ZIP checksum manifest mismatch')
    result={'archive':str(archive),'webVersion':meta['version'],'bytes':archive.stat().st_size,'files':len(files),'emptyDirectories':len(empty),'sha256':digest(archive),'readbackVerified':True,'stage':str(stage)}
    (root/'artifacts'/(name+'-delivery.json')).write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result),flush=True)
if __name__=='__main__':
    if sys.argv[1:]==['stage']: prepare()
    elif sys.argv[1:]==['resume-stage']: prepare(True)
    elif sys.argv[1:]==['archive']: seal()
    else: raise SystemExit('Use stage, resume-stage, archive')
