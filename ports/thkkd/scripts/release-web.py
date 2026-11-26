"""从当前网页仓库生成海惠堂本地站点与源码包，游戏资源由玩家导入。"""
from pathlib import Path
import argparse
import hashlib
import importlib.util
import json
import os
import shutil
import subprocess
import zipfile

PORT = Path(__file__).resolve().parents[1]
ROOT = PORT.parents[1]
spec = importlib.util.spec_from_file_location('thkkd_release_policy', PORT/'scripts/release-policy.py')
policy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(policy)

def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def source_files():
    names = subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard','-z'],cwd=ROOT).decode('utf-8').split('\0')
    for name in sorted(set(names)):
        if not name:
            continue
        path = Path(name)
        if path.parts[0] not in {'launcher','common','tools','docs','services','ports','archive'} and len(path.parts)>1:
            continue
        if path.parts[0]=='ports' and path.parts[1]!='thkkd':
            continue
        if path.parts[0]=='archive' and (len(path.parts)<3 or path.parts[1] not in {'thkkd-web-20261126','thkkd-quality-mp-20261126','thkkd-rendering-20261126','thkkd-performance-20261126','thkkd-cpu-performance-20261126','thkkd-import-20261126','thkkd-original-import-20261126','thkkd-render-overhead-20261126','thkkd-web-only-20261126','thkkd-adaptation-20261127','thkkd-high-refresh-20261127','thkkd-replay-entry-20261127'} or path.name=='baseline.json'):
            continue
        if path.name in {'test-th09-spectator-continuity.mjs','test-th09-spectator-directory.mjs','test-th09-spectator-panel.mjs','test-th09-spectator-timeline.mjs'}:
            continue
        if len(path.parts) >= 3 and path.parts[:2] == ('ports', 'thkkd') and path.parts[2] in {'runtime', 'assets', 'content'}:
            continue
        if any(part in policy.SKIP_DIRS|{'node_modules','toolchains','artifacts','test-results','.github'} for part in path.parts):
            continue
        if path.suffix.lower() in policy.SKIP_SUFFIXES|{'.thkkdr','.rep','.dmp'} or path.name in policy.SKIP_NAMES:
            continue
        if path.name.startswith('.env') and not path.name.endswith(('.example','.template')):
            continue
        if (ROOT/path).is_file():
            yield ROOT/path

def scan(path, relative):
    data=path.read_bytes()
    # 两项固定程序文本分别为 Emscripten 虚拟路径和 ImGui 导航键注释。
    scan_data=data.replace(b'/home/web_user',b'/virtual/web_user').replace(b'/Home/End',b'/Navigation/End')
    policy.check_bytes(scan_data,relative)
    for text in policy.png_compressed_text(data):
        policy.check_bytes(text,relative)
    return data

def inventory(folder):
    return [{'path':p.relative_to(folder).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p),'mtimeNs':p.stat().st_mtime_ns}
            for p in sorted(folder.rglob('*')) if p.is_file() and p.name not in {'PACKAGE-MANIFEST.json','PRIVACY-CHECK.json'}]

def archive(folder,target,prefix):
    manifest=inventory(folder)
    for row in manifest:
        scan(folder/row['path'],row['path'])
    (folder/'PACKAGE-MANIFEST.json').write_text(json.dumps({'files':manifest},ensure_ascii=False,indent=2)+'\n','utf-8')
    with zipfile.ZipFile(target,'x',zipfile.ZIP_DEFLATED,compresslevel=1,allowZip64=True) as output:
        for path in sorted(folder.rglob('*')):
            if not path.is_file() or path.name == 'PRIVACY-CHECK.json':
                continue
            item=zipfile.ZipInfo(prefix+'/'+path.relative_to(folder).as_posix(),policy.zip_date(path.stat().st_mtime))
            item.create_system=0;item.external_attr=0x20
            item.compress_type=zipfile.ZIP_STORED if path.suffix.lower() in {'.png','.webp','.jpg','.ogg','.webm','.wmv','.mp3','.zip','.woff2'} else zipfile.ZIP_DEFLATED
            output.writestr(item,path.read_bytes(),compresslevel=1)
    return {'path':target.name,'bytes':target.stat().st_size,'sha256':digest(target),'files':len(manifest)}

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--check-source',action='store_true')
    parser.add_argument('--site',type=Path,default=PORT/'build/site')
    parser.add_argument('--work-dir',type=Path)
    parser.add_argument('--node-dir',type=Path,default=Path(os.environ.get('EMSDK',ROOT/'toolchains/emsdk'))/'node/24.19.0_64bit')
    parser.add_argument('--validation',type=Path,default=ROOT/'archive/thkkd-original-import-20261126/README.md')
    args=parser.parse_args()
    files=list(source_files())
    failures=[]
    for path in files:
        try:scan(path,path.relative_to(ROOT).as_posix())
        except ValueError as error:failures.append(str(error))
    if failures:
        print(json.dumps({'result':'failed','findings':failures},ensure_ascii=False,indent=2));raise SystemExit(1)
    if args.check_source:
        print(json.dumps({'result':'passed','files':len(files)}));return
    output=args.output.resolve()
    if output.exists() and any(output.iterdir()):
        raise FileExistsError('交付目录已有内容，请使用新的版本目录')
    output.mkdir(parents=True,exist_ok=True)
    site=args.site.resolve()
    host=json.loads((site/'host-manifest.json').read_text('utf-8'))
    from urllib.parse import urlsplit
    runtime_directory=(site/urlsplit(host['games']['thkkd']['runtime']).path.lstrip('/')).parent.resolve()
    if not runtime_directory.is_relative_to(site):
        raise ValueError('Runtime 路径超出站点目录')
    runtime_identity=json.loads((runtime_directory/'manifest.json').read_text('utf-8'))
    current_runtime=json.loads((PORT/'runtime/manifest.json').read_text('utf-8'))
    if runtime_identity['variant']!='normal' or runtime_identity['buildId']!=current_runtime['buildId']:
        raise ValueError('站点没有使用当前普通版 Runtime')
    build=json.loads((PORT/'build/web/build.json').read_text('utf-8'))
    for name,expected in build['inputs'].items():
        if digest(PORT/name)!=expected:
            raise ValueError('WASM 源码输入已变化，请先重新构建：'+name)
    if digest(runtime_directory/'thkkd.wasm')!=build['files']['thkkd.wasm']['sha256']:
        raise ValueError('站点 WASM 与当前构建记录不符')
    for name,record in json.loads((PORT/'runtime/runtime-files.json').read_text('utf-8'))['files'].items():
        if digest(runtime_directory/name)!=record['sha256']:
            raise ValueError('站点 Runtime 文件与当前版本不符：'+name)
    source=(args.work_dir.resolve() if args.work_dir else PORT/'build/release-work'/output.name)/'source'
    if source.exists():
        raise FileExistsError('源码打包工作目录已有内容，请先归档或使用新的工作目录')
    source.mkdir(parents=True)
    for path in files:
        target=source/path.relative_to(ROOT);target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(path,target)
    original=(source/'README.md').read_text('utf-8-sig')
    (source/'README.md').write_text('本源码包包含 eagler-touhou 启动器、公共模块、构建工具及海惠堂源码，原版资源由使用者本地提供。网页构建入口为 ports/thkkd/scripts/build-site.ps1，其他作品源码沿网页仓库独立维护。\n\n'+original,'utf-8')
    game=output/'thkkd-web-local'
    shutil.copytree(site,game/'site',copy_function=shutil.copy2)
    shutil.copy2(ROOT/'launcher/src/launcher/thkkd-original-assets.json',game/'RESOURCE-TIMES.json')
    shutil.copy2(PORT/'THIRD_PARTY.md',game/'THIRD_PARTY.md')
    for name in ['scripts/serve-static.mjs','server/static-content-policy.mjs','lib/pc98-wasm-compression.mjs']:
        target=game/'launcher'/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(ROOT/'launcher'/name,target)
    node=args.node_dir.resolve()
    (game/'server').mkdir();shutil.copy2(node/'node.exe',game/'server/node.exe');shutil.copy2(node/'LICENSE',game/'server/NODE-LICENSE.txt')
    (game/'package.json').write_text('{"type":"module","private":true}\n','utf-8')
    (game/'启动海惠堂.cmd').write_bytes(('@echo off\r\ncd /d "%~dp0"\r\n"server\\node.exe" "launch.mjs" %*\r\npause\r\n').encode('utf-8'))
    shutil.copy2(PORT/'scripts/local-launch.mjs',game/'launch.mjs')
    (game/'README.txt').write_text('東方海恵堂网页单机本地版\n\n双击“启动海惠堂.cmd”，等待服务器显示 Local 地址后在浏览器打开 http://127.0.0.1:8174/ 。选择海惠堂，同时导入原版 dat.dxa、bgm.dxa，或包含它们的 ZIP 后启动；第一次安装后，同一浏览器和地址可离线使用。关闭服务器命令窗口即可停止服务。\n\n方向键移动，Z 射击/确认，X Bomb/取消，Shift 低速，Esc 暂停，暂停后 R 重开。网站设置提供 WASD、自定义键位、手柄映射、触控布局、全屏、独立音量、判定点和显示限频。\n\n存档、配置和 Replay 位于浏览器站点数据，使用网站文件管理导入导出。原版 rpy 使用游戏内菜单和网站播放器；含自由坐标的 thkkdr 使用网站录像工作台。覆盖导入请按界面确认，清除站点数据前导出备份。\n\n本版本保留六面、EX、Omniverse、九种机体与原作解锁规则。源码及验证身份见 SOURCE.json，验证范围见 VALIDATION.md。\n','utf-8')
    commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    association={'schema':'thkkd/local-delivery/1','sourceCommit':commit,'sourceTree':subprocess.check_output(['git','rev-parse','HEAD:ports/thkkd'],cwd=ROOT,text=True).strip(),'originalSourceCommit':'64d48dc366f2d57210dbdac25be82e0e71b2641d','toolchain':'Emscripten 6.0.9','node':'24.19.0','build':build,'runtime':json.loads((PORT/'runtime/runtime-files.json').read_text('utf-8')),'resourceImport':{'files':['dat.dxa','bgm.dxa'],'formats':['original-dxa-pair','original-zip'],'resourcesBundled':False,'resourceExport':False}}
    for folder in [source,game]:
        (folder/'SOURCE.json').write_text(json.dumps(association,ensure_ascii=False,indent=2)+'\n','utf-8')
    shutil.copy2(args.validation,game/'VALIDATION.md')
    validation_details=args.validation.with_name('validation.json')
    if validation_details.is_file():shutil.copy2(validation_details,game/'validation.json')
    packages=[archive(source,output/'thkkd-web-source.zip','eagler-touhou'),archive(game,output/'thkkd-web-local.zip','thkkd-web-local')]
    (output/'RELEASE.json').write_text(json.dumps({'sourceCommit':commit,'packages':packages},ensure_ascii=False,indent=2)+'\n','utf-8')
    (output/'SHA256SUMS.txt').write_text(''.join(p['sha256']+'  '+p['path']+'\n' for p in packages),'utf-8')
    print(json.dumps({'result':'passed','sourceCommit':commit,'packages':[{k:p[k] for k in ['path','bytes','sha256']} for p in packages]},ensure_ascii=False))

if __name__=='__main__':main()
