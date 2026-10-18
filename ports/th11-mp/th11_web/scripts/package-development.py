"""Create the complete, relocatable TH11 Windows development delivery."""
from pathlib import Path
import hashlib
import json
import os
import shutil
import sys
import zipfile

workspace = Path(__file__).resolve().parents[2]
if os.name == 'nt' and not str(workspace).startswith('\\\\?\\'):
    workspace = Path('\\\\?\\' + str(workspace))
name = 'TH11-20260927-development'
stage = workspace / 'artifacts' / (name + '-stage')
archive = workspace / (name + '.zip')
skip = {'.git', '.codex', '.agents', '__pycache__', '.cache', 'npm-cache', 'objects', 'rebuild-check', 'delivery-rebuild'}
private = {'cloudflared-token.txt', 'processes.json', 'server-process.json', 'debug.log', 'scoreth11.dat', 'th11.cfg'}

def digest(p):
    with p.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()

def copy(source, relative):
    dest = stage / relative
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists() and dest.stat().st_size == source.stat().st_size and dest.stat().st_mtime_ns == source.stat().st_mtime_ns:
        return
    shutil.copy2(source, dest)

def tree(source, relative, exclude=()):
    if not source.is_dir():
        raise RuntimeError('Missing dependency: ' + str(source))
    for current, dirs, files in os.walk(source):
        dirs[:] = sorted(d for d in dirs if d not in skip and d not in exclude)
        (stage / relative / Path(current).relative_to(source)).mkdir(parents=True, exist_ok=True)
        for n in sorted(files):
            p = Path(current) / n
            if n.startswith('.env') or n.lower() in private or p.suffix.lower() in {'.pyc', '.tmp', '.log', '.pem', '.key', '.pfx'}:
                continue
            if p.is_symlink():
                raise RuntimeError('Unexpected dependency symlink: ' + str(p))
            copy(p, Path(relative) / p.relative_to(source))

def write(relative, text):
    dest = stage / relative
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(text, encoding='utf-8', newline='\r\n' if str(relative).endswith('.cmd') else '\n')

def prepare(resume=False):
    if (stage.exists() and not resume) or archive.exists():
        raise RuntimeError('Delivery already exists; refusing overwrite')
    stage.mkdir(parents=True, exist_ok=resume)
    game = workspace / 'th11_web'
    for d in ['cpp', 'sdl-runtime', 'scripts', 'tests', 'launcher', 'docs', 'assets', 'reference']:
        tree(game / d, 'th11_web/' + d)
    for f in ['README.md', 'target.json', '启动地灵殿网页版.cmd']:
        copy(game / f, 'th11_web/' + f)
    tree(game / 'config', 'th11_web/config')
    release = game / 'artifacts/sdl-release'
    for d in ['site', 'scripts', 'node_modules', 'verification']:
        tree(release / d, 'th11_web/artifacts/sdl-release/' + d)
    for f in ['th11.wasm', 'th11.mjs', 'build.json', 'package.json']:
        copy(release / f, 'th11_web/artifacts/sdl-release/' + f)
    for f in ['THIRD-PARTY-NOTICES.txt', 'LICENSE-launcher.txt']:
        copy(release / 'site' / f, f)
    for d in ['sdl', 'input']:
        tree(workspace / 'portable' / d, 'portable/' + d)
    for d in ['verification', 'analysis/jp', 'world-checkpoint-99450']:
        tree(game / 'artifacts/cpp' / d, 'th11_web/artifacts/cpp/' + d)
    for p in (game / 'artifacts/cpp').iterdir():
        if p.is_file() and p.suffix in {'.wasm', '.json'}:
            copy(p, 'th11_web/artifacts/cpp/' + p.name)
    tree(game / 'artifacts/sdl3/browser', 'th11_web/artifacts/sdl3/browser')
    for f in ['build.json', 'th11-harness.mjs', 'th11-harness.wasm']:
        copy(game / 'artifacts/sdl3' / f, 'th11_web/artifacts/sdl3/' + f)
    for folder, names in {
        'cpp': ['spell-overlay-check.log', 'spell-overlay-recheck.log', 'spell-overlay-equivalence.log',
                'damage-fixed-regression.log', 'world-prefix-equivalence.log', 'world-damage-fixed-tail-proof.log',
                'world-damage-fixed-extra.log', 'world-damage-fixed-demos.log'],
        'sdl-release': ['spell-overlay-launcher.log', 'spell-overlay-update.log', 'spell-overlay-public.log'],
        'sdl3': ['spell-overlay-browser.log']
    }.items():
        for f in names:
            copy(game / 'artifacts' / folder / f, 'verification/original-logs/' + f)
    original = workspace / '[th11] 东方地灵殿 (汉化版+日文版)'
    for f in ['th11.exe', 'th11c.exe', 'th11.dat', 'th11c.dat', 'thbgm.dat', 'custom.exe', 'custom_c.exe', 'd3dx9_37.dll', '汉化版readme.txt']:
        copy(original / f, original.name + '/' + f)
    tree(original / '附带文档', original.name + '/附带文档')
    baseline = json.loads((game / 'config/final-baseline.json').read_text(encoding='utf-8'))
    for f, h in baseline['files'].items():
        if digest(workspace / f) != h:
            raise RuntimeError('Shared baseline drift: ' + f)
        copy(workspace / f, f)
    for f in ['release-server.mjs', 'netplay-relay.mjs']:
        copy(workspace / 'th09_web/scripts' / f, 'th09_web/scripts/' + f)
    tree(workspace / 'th09_web/node_modules/ws', 'th09_web/node_modules/ws')
    # Historical shell for the shipped cross-game update test, not game source.
    tree(workspace / 'th09_web/artifacts/sdl-release/site', 'th09_web/artifacts/sdl-release/site')
    for game_id in ['th08', 'th10']:
        copy(workspace / (game_id + '_web/artifacts/sdl-release/site/manifest.json'), game_id + '_web/artifacts/sdl-release/site/manifest.json')
    for d in ['emsdk', 'architecture/typescript', 'architecture/python']:
        tree(workspace / 'tools' / d, 'tools/' + d)
    for d in ['wasi-sdk-34.0-x86_64-windows', 'decomp']:
        tree(workspace / 'th10_web/tools' / d, 'th10_web/tools/' + d)
    for f in ['node.exe', 'Node.LICENSE']:
        copy(workspace / 'th10_web/tools' / f, 'th10_web/tools/' + f)
    copy(workspace / 'th10_web/scripts/native/browser-launch.mjs', 'th10_web/scripts/native/browser-launch.mjs')
    for d in ['playwright', 'playwright-core']:
        tree(workspace / 'th10_web/node_modules' / d, 'th10_web/node_modules/' + d)
    for d in ['@alexaltea/unicorn-js', 'koffi', '@koromix']:
        tree(workspace / 'th08_web/node_modules' / d, 'th08_web/node_modules/' + d)
    copy(workspace / 'th08_web/package.json', 'th08_web/package.json')
    # Full interpreter plus only the packages needed for regenerating audio.
    py = Path(sys.base_prefix)
    for p in py.iterdir():
        if p.is_file() and (p.suffix in {'.exe', '.dll'} or p.name.startswith('LICENSE')):
            copy(p, 'tools/python/' + p.name)
    for d in ['DLLs', 'Lib', 'libs', 'include']:
        tree(py / d, 'tools/python/' + d, exclude=('site-packages',))
    for p in (py / 'Lib/site-packages').iterdir():
        if p.name.startswith(('numpy', 'cffi', 'pycparser', '_cffi_backend')):
            dest = 'tools/python/Lib/site-packages/' + p.name
            tree(p, dest) if p.is_dir() else copy(p, dest)
    browsers = Path(os.environ['LOCALAPPDATA']) / 'ms-playwright'
    for p in browsers.iterdir():
        if p.is_dir() and p.name.startswith(('chromium_headless_shell-', 'ffmpeg-')):
            tree(p, 'tools/playwright-browsers/' + p.name)
    copy(workspace / 'th09_web/scripts/handoff/prepare-toolchain.py', 'tools/prepare-toolchain.py')
    write('tools/emsdk/.emscripten', "import os\n_sdk = os.environ['EMSDK']\nLLVM_ROOT = os.path.join(_sdk, 'install', 'bin')\nBINARYEN_ROOT = os.path.join(_sdk, 'install')\nNODE_JS = [os.path.abspath(os.path.join(_sdk, '..', '..', 'th10_web', 'tools', 'node.exe'))]\nCACHE = os.path.join(_sdk, 'install', 'emscripten', 'cache')\nEMSCRIPTEN_ROOT = os.path.join(_sdk, 'install', 'emscripten')\n")
    write('开发环境.cmd', '@echo off\nset "PATH=%~dp0tools\\python;%~dp0th10_web\\tools;%PATH%"\nset "PLAYWRIGHT_BROWSERS_PATH=%~dp0tools\\playwright-browsers"\nset "EMSDK=%~dp0tools\\emsdk"\nset "EM_CONFIG=%~dp0tools\\emsdk\\.emscripten"\nset "PYTHONNOUSERSITE=1"\n"%~dp0tools\\python\\python.exe" "%~dp0tools\\prepare-toolchain.py"\n')
    write('启动地灵殿网页版.cmd', '@echo off\nchcp 65001 >nul\nsetlocal\ncd /d "%~dp0"\necho 请打开 http://127.0.0.1:8113/ ，保持本窗口开启。\n"%~dp0th10_web\\tools\\node.exe" "%~dp0th11_web\\artifacts\\sdl-release\\scripts\\serve.mjs" --port 8113\npause\n')
    write('重新编译地灵殿.cmd', '@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\nif errorlevel 1 goto failed\ncd /d "%~dp0"\nnode th11_web\\scripts\\build-sdl.mjs --release\nif errorlevel 1 goto failed\nnode th11_web\\scripts\\package-release.mjs\nif errorlevel 1 goto failed\necho 编译及站点打包完成。\npause\nexit /b 0\n:failed\necho 编译失败，请保留上方错误。\npause\nexit /b 1\n')
    write('验证符卡显示修复.cmd', '@echo off\nchcp 65001 >nul\nsetlocal\ncall "%~dp0开发环境.cmd"\ncd /d "%~dp0"\nnode th11_web\\scripts\\cpp\\build.mjs\nif errorlevel 1 goto failed\nnode --test th11_web\\tests\\cpp\\spell-overlay.test.mjs\nif errorlevel 1 goto failed\necho 符卡数字与原版输出对照通过。\npause\nexit /b 0\n:failed\necho 验证失败，请保留输出。\npause\nexit /b 1\n')
    write('deploy/server.mjs', "import {releaseServer} from '../th11_web/artifacts/sdl-release/scripts/serve.mjs';\nimport {fileURLToPath} from 'node:url';\nconst port=Number(process.env.PORT||3007),host=process.env.HOST||'127.0.0.1';\nif(!Number.isInteger(port)||port<1||port>65535)throw Error('Invalid PORT');\nconst result=await releaseServer({root:fileURLToPath(new URL('../th11_web/artifacts/sdl-release/site',import.meta.url)),port,host});\nconsole.log('TH11 '+result.manifest.version+' '+result.url);\n")
    write('README-交付说明.md', '''# 地灵殿完整开发交付 · 2026-09-27

沿用永夜抄、风神录、花映塚的完整开发包形式：可运行网站、全部 C++ 与网页源码、共享 SDL3 渲染／输入源码、游戏资源、原版对照材料、测试及 Windows x64 工具链。已包含本次符卡 Bonus 分数、History 收取记录、失败标记与透明度修复。

## 直接测试

完整解压到可写且较短的目录（例如 E:\\TH11），双击根目录“启动地灵殿网页版.cmd”，打开 http://127.0.0.1:8113/ 。保持窗口开启。已附 Node，不需要另装开发工具；不要在 ZIP 内运行，也不要只解压 th11_web。工具链包含长文件名，建议使用支持长路径的解压工具。

## 部署到测试组环境

详见 deploy/部署说明.md。可直接部署已经构建的网站，不需要先编译。使用测试组自己的 HTTPS 域名及反向代理；本包不带原作者的隧道凭据。默认服务为 3007，仅监听本机。服务器仅公开发布清单中的站点文件。

## 开发与重建

双击“重新编译地灵殿.cmd”：使用包内 Python、Node、Emscripten / SDL3 缓存与 TypeScript 从源码生成 Wasm 和网站。完整工具链按 Windows x64 配置；Linux 部署已构建站点仅需安装 Node.js 22 或更新的兼容版本，Linux 重新编译需另配本平台工具链。

“验证符卡显示修复.cmd”会构建测试核心并与原版 EXE 对照符卡数字输出。其他测试：先在 cmd 运行 `call 开发环境.cmd`，再按 th11_web/docs/BUILD.md 的命令执行。原版分析另附 Ghidra、JDK、WASI SDK 与 Unicorn。已附 OGG、字形数据及生成脚本；重新采样 Windows 字体需机器上安装相应日文字体。

开发树保留原有相对路径。th08_web / th10_web 是本项目引用的工具及共享基准；th09_web 还附了旧启动器升级测试基线。它们不是其他游戏的完整开发交付。

## 版本与验收范围

站点版本 40704f8bcd9f1dd9f4cb63f5，发布 Wasm SHA-256 d3ab386f9ffd3bf9c10ee2a4e45a587b68eb42e883c04110b83cff1d5e568bc4。

本次补丁通过 361 组符卡原版对照、原版字形／符卡动画回归、主线与 Extra 共 143,928 帧新旧核心等价比较，以及实际浏览器画面、启动器和更新检查。详细范围见 th11_web/docs/2026-09-27-符卡分数显示修复.md。旧整体验收报告保留原始构建哈希，不能作为当前补丁的全项目重跑证明；verify-release.mjs 的严格全项目验收需要在新构建上重跑它要求的报告。本次交付包完整性与异地目录重建检查另见 verification。

公网旧环境已确认加载新版，但后续自动菜单进入步骤未完成，不计作通过。桌面浏览器模拟不代替手机真机验证；请测试组覆盖机体／难度、完整流程、存档、录像与目标手机。

玩家存档和录像保存在各自浏览器，不写入服务器。本包不包含维护者存档、私人录像、Cloudflare 凭据、进程状态或个人浏览器目录；reference/replays 为有来源记录的开发测试样本。

FILE-SHA256.json 列出每个交付文件的哈希。压缩包包含原版素材及第三方工具，各自归属和许可保持不变；这是测试／开发交付，不是只含自主代码的开源仓库。
''')
    write('deploy/部署说明.md', '''# 测试环境部署

## 已构建站点

Windows：完整解压后，在根目录执行 `th10_web\\tools\\node.exe deploy/server.mjs`。

Linux：安装 Node.js 22 或更新的兼容版本，在根目录执行 `node deploy/server.mjs`。最小运行目录为 `deploy/server.mjs` 和完整的 `th11_web/artifacts/sdl-release/{site,scripts,node_modules}`；这些目录的相对位置必须保留。

默认 `127.0.0.1:3007`。可用环境变量 PORT 改端口；需要容器或跨主机代理时设 HOST=0.0.0.0。由测试组的 HTTPS 反向代理转发至该端口，网页直接打开域名根路径 `/`。手机资源缓存和离线功能要求 HTTPS 安全上下文。

## 反向代理

保留源站提供的 MIME、Cross-Origin-Opener-Policy、Cross-Origin-Embedder-Policy、Content-Security-Policy、Range 和 Cache-Control 响应头。不要额外注入跨域脚本。不要对整个站点启用“缓存所有内容”；manifest.json、version.json、app-shell-sw.js 应按源站策略检查更新。发布文件不要分批上传到正在提供流量的目录，应先完整上传，再切换服务目录并重启 Node，使内存中的清单与磁盘资源一致。

只能把 site 作为静态内容根目录，或使用附带的清单白名单服务器；不要把完整开发包目录设为公网静态根目录。

更新后核对 `/manifest.json` 中 game=th11、version=40704f8bcd9f1dd9f4cb63f5。玩家退出游戏、刷新页面并按更新提示操作。仅更新程序无需删除存档；避免为更新而清空整个浏览器站点数据。

## 验收建议

首先用全新浏览器确认首次下载、进入游戏及音频，然后检查已有缓存更新、手机拖动／B／暂停、存档导入导出、原版录像及符卡 Bonus/History 显示。使用自己的测试域名执行公共入口检查：在开发环境设置 TH11_PUBLIC_URL，再运行 `node th11_web/tests/browser/public-check.mjs`。该脚本使用桌面无头浏览器模拟触屏，不能代替真机验收。
''')
    write('.gitignore', 'tools/\nth10_web/tools/\n**/node_modules/\n**/objects/\n**/__pycache__/\n**/.cache/\n*.zip\n*.log\n')
    print(json.dumps({'stage': str(stage), 'status': 'prepared'}), flush=True)

def seal():
    if archive.exists():
        raise RuntimeError('Refusing to overwrite ZIP')
    files = sorted(p for p in stage.rglob('*') if p.is_file()
                   and not any(x in skip for x in p.relative_to(stage).parts)
                   and p.suffix not in {'.pyc', '.tmp'} and p.name != 'FILE-SHA256.json')
    sums = {p.relative_to(stage).as_posix(): digest(p) for p in files}
    write('FILE-SHA256.json', json.dumps(sums, ensure_ascii=False, indent=2) + '\n')
    files.append(stage / 'FILE-SHA256.json')
    print('Compressing', len(files), 'files...', flush=True)
    empty_dirs = sorted(p for p in stage.rglob('*') if p.is_dir() and not any(p.iterdir())
                        and not any(x in skip for x in p.relative_to(stage).parts))
    with zipfile.ZipFile(archive, 'x', zipfile.ZIP_DEFLATED, compresslevel=6, allowZip64=True) as z:
        for p in empty_dirs:
            z.write(p, name + '/' + p.relative_to(stage).as_posix() + '/')
        for i, p in enumerate(files):
            z.write(p, name + '/' + p.relative_to(stage).as_posix())
            if i and i % 5000 == 0:
                print('Compressed', i, '/', len(files), flush=True)
    print('Reading back every ZIP entry...', flush=True)
    with zipfile.ZipFile(archive) as z:
        for p in empty_dirs:
            if not z.getinfo(name + '/' + p.relative_to(stage).as_posix() + '/').is_dir():
                raise RuntimeError('Missing empty directory: ' + str(p))
        for n, h in sums.items():
            with z.open(name + '/' + n) as f:
                if hashlib.file_digest(f, 'sha256').hexdigest() != h:
                    raise RuntimeError('ZIP hash mismatch: ' + n)
    result = {'archive': str(archive), 'bytes': archive.stat().st_size, 'files': len(files), 'emptyDirectories': len(empty_dirs),
              'sha256': digest(archive), 'readbackVerified': True}
    (workspace / 'artifacts/TH11-20260927-delivery.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result), flush=True)

if __name__ == '__main__':
    if sys.argv[1:] == ['stage']:
        prepare()
    elif sys.argv[1:] == ['resume-stage']:
        prepare(resume=True)
    elif sys.argv[1:] == ['archive']:
        seal()
    else:
        raise SystemExit('Use stage or archive')
