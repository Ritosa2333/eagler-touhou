import fcntl,hashlib,json,os,subprocess,time,uuid
from pathlib import Path

ops=Path(__file__).resolve().parent
plan=json.loads((ops/'input.json').read_text());assembled=json.loads((ops/'assembled.json').read_text());state=json.loads((ops/'state.json').read_text())
base=Path('/var/www/eagler-touhou-test');current=base/'current';candidate=Path(assembled['candidate']);previous=Path(state['previous'])
nginx=Path('/etc/nginx/sites-enabled/test.touhou.vip').resolve()
sha=lambda data:hashlib.sha256(data).hexdigest()
def atomic(path,data):
    temporary=path.with_name(path.name+'.th05-next-'+uuid.uuid4().hex)
    temporary.write_bytes(data);os.chmod(temporary,0o644);os.replace(temporary,path)
def switch(target):
    pending=base/('.current-th05-'+uuid.uuid4().hex);pending.symlink_to(target,target_is_directory=True);os.replace(pending,current)
def protected():
    result={}
    for name in ['eagler-touhou','eagler-touhou-beta']:
        path=Path('/var/www')/name/'current'
        if path.exists():result[name]={'target':str(path.resolve()),'deployment':sha((path/'deployment.json').read_bytes())}
    return result
assert plan['base']==str(base) and candidate.parent==base/'releases' and candidate==Path(state['candidate'])
assert current.resolve()==previous and sha((previous/'deployment.json').read_bytes())==plan['baselineDeploymentSha256']
release=json.loads((candidate/'release-manifest.json').read_text());assert release['releaseId']==assembled['releaseId'] and sha((candidate/'deployment.json').read_bytes())==release['deploymentSha256']
old_nginx=nginx.read_bytes();new_nginx=(ops/'test.touhou.vip.next').read_bytes();assert sha(old_nginx)==plan['baselineNginxSha256']
before=protected()
with open('/var/lib/eagler-test-sync/lock','w') as lock:
    fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
    assert current.resolve()==previous
    (ops/'nginx.before').write_bytes(old_nginx)
    try:
        atomic(nginx,new_nginx)
        subprocess.run(['nginx','-t'],check=True,capture_output=True,text=True)
        subprocess.run(['systemctl','reload','nginx'],check=True)
        switch(candidate)
        site=json.loads(subprocess.check_output(['curl','--max-time','25','--fail','--silent','--show-error','--resolve','test.touhou.vip:443:127.0.0.1','https://test.touhou.vip/site-manifest.json']))
        assert site['games']['th05']['package']['revision']==plan['packageRevision']
        for name in ['th05.package.json','runtime/th05/'+plan['generation']['generation']+'/th05.wasm','games/th05/th05.data','games/th05/music/ogg/akyu-01.ogg','games/th05/music/ogg/room-r_00.ogg','games/th05/language/lang_zh-hans.zip']:
            for attempt in range(12):
                result=subprocess.run(['curl','--max-time','25','--fail','--silent','--show-error','--head','--resolve','test.touhou.vip:443:127.0.0.1','https://test.touhou.vip/'+name],capture_output=True)
                if result.returncode==0:break
                if attempt==11:raise RuntimeError(name+': '+result.stderr.decode())
                time.sleep(0.5)
        assert before==protected()
    except Exception:
        switch(previous);atomic(nginx,old_nginx);subprocess.run(['nginx','-t'],check=True);subprocess.run(['systemctl','reload','nginx'],check=True);raise
receipt={**assembled,'publishedAt':time.strftime('%Y-%m-%dT%H:%M:%SZ',time.gmtime()),'nginxBefore':sha(old_nginx),'nginxAfter':sha(new_nginx),'protected':before}
(ops/'published.json').write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt))
