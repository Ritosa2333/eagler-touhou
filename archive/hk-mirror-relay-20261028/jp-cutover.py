#!/usr/bin/env python3
import fcntl,json,os,pathlib,shutil,subprocess,urllib.request
BASE=pathlib.Path('/root/hk-migration-20261028')
SITE=pathlib.Path('/etc/nginx/sites-available/touhouonline.com')
def status():
 try:
  with urllib.request.urlopen('http://127.0.0.1:18156/status',timeout=4) as r:return json.load(r)
 except Exception:
  if (BASE/'core-moved.json').exists():return {'products':{},'moved':True}
  raise
def playing(value):return sum(x.get('playing',0) for x in value.get('products',{}).values())
def main():
 action=os.environ.get('SSH_ORIGINAL_COMMAND','status')
 if action not in ('status','prepare','activate'):raise ValueError('Unsupported operation')
 BASE.mkdir(mode=0o700,exist_ok=True)
 with open('/run/lock/eagler-hk-cutover.lock','w') as lock:
  fcntl.flock(lock,fcntl.LOCK_EX)
  current=status()
  if action=='status':print(json.dumps(current));return
  if current.get('moved'):print(json.dumps(current));return
  if playing(current):raise RuntimeError('Active games retained on JP')
  if action=='prepare':
   dest=pathlib.Path('/var/www/eagler-touhou/.hk-export/nicknames')
   shutil.copytree('/var/lib/eagler-touhou/nicknames',dest,dirs_exist_ok=True)
   print(json.dumps({'prepared':True,'status':current}));return
  original=SITE.read_text()
  if 'proxy_pass http://127.0.0.1:19446;' not in original:raise ValueError('Unexpected JP relay configuration')
  changed=original.replace('proxy_pass http://127.0.0.1:19446;', '''proxy_pass https://103.73.161.228;
        proxy_ssl_server_name on;
        proxy_ssl_name relay.touhouonline.com;
        proxy_ssl_verify on;
        proxy_ssl_verify_depth 4;
        proxy_ssl_trusted_certificate /etc/ssl/certs/ca-certificates.crt;''')
  # The dedicated HK proxy performs client attribution using the forwarded address.
  changed=changed.replace('proxy_set_header Host $host;\n        proxy_read_timeout 75s;', 'proxy_set_header Host relay.touhouonline.com;\n        proxy_read_timeout 75s;')
  (BASE/'site-before-core-move.conf').write_text(original)
  SITE.write_text(changed)
  try:
   subprocess.run(['nginx','-t'],check=True,capture_output=True)
   # Recheck immediately before the public proxy is switched.
   if playing(status()):raise RuntimeError('A game started during preparation')
   subprocess.run(['systemctl','reload','nginx'],check=True)
  except Exception:
   SITE.write_text(original)
   subprocess.run(['nginx','-t'],check=True,capture_output=True)
   subprocess.run(['systemctl','reload','nginx'],check=True)
   raise
  subprocess.run(['systemctl','disable','--now','eagler-netplay-lifecycle-20261025'],check=True,capture_output=True)
  data={'moved':True,'destination':'103.73.161.228','previousStatus':current}
  (BASE/'core-moved.json').write_text(json.dumps(data))
  print(json.dumps(data))
if __name__=='__main__':main()
