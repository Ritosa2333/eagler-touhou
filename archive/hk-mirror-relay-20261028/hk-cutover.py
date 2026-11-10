#!/usr/bin/env python3
import fcntl,json,pathlib,subprocess,os,time,urllib.request
BASE=pathlib.Path('/root/hk-migration-20261028')
SITE=pathlib.Path('/etc/nginx/sites-available/touhouonline.com')
SSH=['ssh','-i',str(BASE/'cutover_key'),'-o','UserKnownHostsFile='+str(BASE/'known_hosts'),'-o','StrictHostKeyChecking=yes','-o','ConnectTimeout=10','root@74.82.198.36']
def jp(action):
 result=subprocess.run([*SSH,action],capture_output=True,text=True,timeout=30)
 if result.returncode:raise RuntimeError('JP cutover step failed: '+action+' '+result.stderr[-1000:])
 return json.loads(result.stdout)
def main():
 with open('/run/lock/eagler-hk-activate.lock','w') as lock:
  fcntl.flock(lock,fcntl.LOCK_EX)
  if (BASE/'core-moved.json').exists():return
  value=jp('status')
  if value.get('moved'):
   (BASE/'core-moved.json').write_text(json.dumps(value));return
  if any(v.get('playing',0) for v in value.get('products',{}).values()):
   print(json.dumps({'status':'draining','products':value['products']}));return
  jp('prepare')
  subprocess.run(['rsync','-az','--timeout=60','-e','ssh -i '+str(BASE/'mirror_key')+' -o UserKnownHostsFile='+str(BASE/'known_hosts')+' -o StrictHostKeyChecking=yes','root@74.82.198.36:.hk-export/nicknames/','/var/lib/eagler-touhou/nicknames/'],check=True)
  subprocess.run(['systemctl','restart','eagler-netplay-lifecycle-20261025'],check=True)
  for attempt in range(30):
   try:
    with urllib.request.urlopen('http://127.0.0.1:18156/status',timeout=2) as response:json.load(response)
    break
   except Exception:
    if attempt==29:raise
    time.sleep(0.2)
  before=SITE.read_text()
  SITE.write_text((BASE/'site-local-relay.conf').read_text())
  try:
   subprocess.run(['nginx','-t'],check=True,capture_output=True)
   subprocess.run(['systemctl','reload','nginx'],check=True)
   result=jp('activate')
  except Exception:
   state=jp('status')
   if state.get('moved'):
    (BASE/'core-moved.json').write_text(json.dumps(state));return
   SITE.write_text(before)
   subprocess.run(['nginx','-t'],check=True,capture_output=True)
   subprocess.run(['systemctl','reload','nginx'],check=True)
   raise
  (BASE/'core-moved.json').write_text(json.dumps(result))
  print(json.dumps({'status':'moved','destination':'103.73.161.228'}))
if __name__=='__main__':main()
