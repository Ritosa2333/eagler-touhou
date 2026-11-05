#!/usr/bin/env python3
import fcntl,hashlib,json,os,pathlib,re,subprocess,tempfile
BASE=pathlib.Path('/var/www/eagler-touhou')
STATE=pathlib.Path('/var/lib/eagler-hk-mirror')
SSH='ssh -i /root/hk-migration-20261028/mirror_key -o UserKnownHostsFile=/root/hk-migration-20261028/known_hosts -o StrictHostKeyChecking=yes -o ConnectTimeout=15'
def pull(source,target,*extra):
 subprocess.run(['rsync','-az','--compress-choice=zstd','--compress-level=3','--timeout=120','-e',SSH,*extra,'root@74.82.198.36:'+source,str(target)],check=True)
def atomic(path,data,mode):
 fd,tmp=tempfile.mkstemp(dir=path.parent)
 with os.fdopen(fd,'wb') as f:f.write(data)
 os.chmod(tmp,mode);os.replace(tmp,path)
def main():
 STATE.mkdir(mode=0o700,parents=True,exist_ok=True)
 with open('/run/lock/eagler-hk-mirror.lock','w') as lock:
  fcntl.flock(lock,fcntl.LOCK_EX)
  pull('.hk-export/',STATE/'export/')
  meta=json.loads((STATE/'export/release.json').read_text())
  done=json.loads((STATE/'published.json').read_text()) if (STATE/'published.json').exists() else {}
  if done.get('identity')!=meta['identity']:
   name=meta['release']
   if not re.fullmatch('[A-Za-z0-9_-]+',name) or not re.fullmatch('[a-f0-9]{64}',meta['identity']):raise ValueError('Invalid release identity')
   target=BASE/'releases'/(name+'-mirror-'+meta['identity'][:12])
   target.mkdir(parents=True,exist_ok=True)
   extra=['--checksum','--delete','--delay-updates','--exclude=.tmp']
   if (BASE/'current').exists():extra+=['--link-dest='+str((BASE/'current').resolve())]
   pull('releases/'+name+'/',str(target)+'/',*extra)
   for name,expected in meta['files'].items():
    path=target/name
    if not path.resolve().is_relative_to(target) or not path.is_file():raise ValueError('Missing file '+name)
    digest=hashlib.sha256()
    with path.open('rb') as f:
     for block in iter(lambda:f.read(1024*1024),b''):digest.update(block)
    if path.stat().st_size!=expected['size'] or digest.hexdigest()!=expected['sha256']:raise ValueError('File mismatch '+name)
   pull('.hk-export/release.json',STATE/'confirmed.json')
   if json.loads((STATE/'confirmed.json').read_text())['identity']!=meta['identity']:raise ValueError('Source advanced, retry next poll')
   link=BASE/'.current-hk-next'
   link.unlink(missing_ok=True);link.symlink_to(target)
   os.replace(link,BASE/'current')
   atomic(STATE/'published.json',json.dumps({'identity':meta['identity'],'release':str(target),'files':len(meta['files'])}).encode(),0o600)
  turn=(STATE/'export/active-turn.json').read_bytes();value=json.loads(turn)
  if not value.get('urls') or not value.get('sharedSecret'):raise ValueError('Invalid TURN publication')
  atomic(pathlib.Path('/etc/eagler-turn-monitor/active-turn.json'),turn,0o600)
  inventory=(STATE/'export/resource-inventory.json').read_bytes();json.loads(inventory)
  atomic(pathlib.Path('/etc/eagler-resource-best-beijing-inventory.json'),inventory,0o644)
  pull('sync-language-artifacts/',str(BASE/'sync-language-artifacts')+'/', '--delay-updates')
  print(json.dumps({'identity':meta['identity'],'status':'published'}))
if __name__=='__main__':main()
