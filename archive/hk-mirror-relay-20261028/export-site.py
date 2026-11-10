#!/usr/bin/env python3
import fcntl,hashlib,json,os,pathlib,shutil,tempfile
BASE=pathlib.Path('/var/www/eagler-touhou')
OUT=BASE/'.hk-export'
def atomic(path,data,mode=0o600):
 path.parent.mkdir(parents=True,exist_ok=True)
 fd,tmp=tempfile.mkstemp(dir=path.parent)
 with os.fdopen(fd,'wb') as f:f.write(data)
 os.chmod(tmp,mode);os.replace(tmp,path)
def main():
 OUT.mkdir(mode=0o700,exist_ok=True)
 with open('/run/lock/eagler-hk-export.lock','w') as lock:
  fcntl.flock(lock,fcntl.LOCK_EX)
  root=(BASE/'current').resolve(strict=True)
  if root.parent!=BASE/'releases':raise ValueError('Release outside managed root')
  version=(root/'sync-version.json').read_bytes()
  identity=hashlib.sha256(str(root).encode()+version).hexdigest()
  previous=json.loads((OUT/'release.json').read_text()) if (OUT/'release.json').exists() else {}
  if previous.get('identity')!=identity:
   files={}
   for path in sorted(root.rglob('*')):
    name=path.relative_to(root).as_posix()
    if '.tmp' in path.relative_to(root).parts:continue
    if path.is_symlink():raise ValueError('Publish regular files before mirroring: '+name)
    if path.is_file():
     digest=hashlib.sha256()
     with path.open('rb') as f:
      for block in iter(lambda:f.read(1024*1024),b''):digest.update(block)
     files[name]={'size':path.stat().st_size,'sha256':digest.hexdigest()}
   value={'identity':identity,'release':root.name,'files':files}
   if root!=(BASE/'current').resolve() or version!=(root/'sync-version.json').read_bytes():raise ValueError('Release changed during inventory')
   atomic(OUT/'release.json',json.dumps(value,separators=(',',':')).encode())
  for source,name in [('/etc/eagler-turn-monitor/active-turn.json','active-turn.json'),('/etc/eagler-resource-best-beijing-inventory.json','resource-inventory.json')]:
   data=pathlib.Path(source).read_bytes();json.loads(data);atomic(OUT/name,data)
  print(json.dumps({'identity':identity,'release':root.name}))
if __name__=='__main__':main()
