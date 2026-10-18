from pathlib import Path
import zipfile,hashlib,json
root=Path(__file__).resolve().parents[1]
out=root/'artifacts/bugfix-20260920'
release=out/'release'
name='TH09-Web-20260920-test-fix'
runzip=out/(name+'-Windows.zip')
srczip=out/(name+'-Source.zip')
for p in [runzip,srczip]:
    if p.exists(): raise RuntimeError('Refuse to replace existing delivery: '+str(p))
def add_tree(z,base,prefix,exclude=()):
    for p in sorted(base.rglob('*')):
        rel=p.relative_to(base)
        if not p.is_file() or any(x in exclude for x in rel.parts): continue
        if p.name.startswith('.env') or p.suffix.lower() in {'.pem','.key'}: continue
        z.write(p,str(Path(prefix)/rel).replace('\\','/'))
with zipfile.ZipFile(runzip,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
    for d in ['site','scripts','node_modules','runtime','docs','verification']:
        add_tree(z,release/d,name+'/'+d)
    for f in ['README.md','package.json','启动花映塚网页版.cmd']:
        z.write(release/f,name+'/'+f)
with zipfile.ZipFile(srczip,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
    z.write(root/'docs/2026-09-20-源码重建说明.md','README-源码重建.md')
    for d in ['cpp','sdl-runtime','scripts','tests','launcher','docs','licenses']:
        add_tree(z,root/d,'th09_web/'+d,('.cache','__pycache__','node_modules','.git'))
    for f in ['package.json','package-lock.json','README.md','target.json','THIRD-PARTY-NOTICES.txt','启动花映塚网页版.cmd']:
        z.write(root/f,'th09_web/'+f)
    for d in ['sdl','input']:
        add_tree(z,root.parent/'portable'/d,'portable/'+d,('__pycache__',))
    for p in (root/'reference').glob('*'):
        if p.is_file():z.write(p,'th09_web/reference/'+p.name)
    for f in ['demorpy0.rpy','demorpy1.rpy','demorpy2.rpy','title00.png']:
        z.write(root/'reference/assets'/f,'th09_web/reference/assets/'+f)
    z.write(out/'title-card.png','th09_web/artifacts/bugfix-20260920/title-card.png')
    add_tree(z,release/'verification','th09_web/verification')
results=[]
for p in [runzip,srczip]:
    with zipfile.ZipFile(p) as z:
        bad=z.testzip()
        if bad:raise RuntimeError('CRC failure: '+bad)
        names=z.namelist()
        assert not any(n.endswith('/th09.exe') or '/objects/' in n or '/.cache/' in n for n in names)
        entries=len(names)
    h=hashlib.file_digest(p.open('rb'),'sha256').hexdigest()
    results.append({'file':p.name,'bytes':p.stat().st_size,'sha256':h,'entries':entries,'crcPassed':True})
(out/'SHA256SUMS.txt').write_text(''.join(v['sha256']+'  '+v['file']+'\n' for v in results),encoding='ascii')
(out/'delivery-archives.json').write_text(json.dumps(results,indent=2)+'\n',encoding='utf-8')
print(json.dumps(results,indent=2),flush=True)
