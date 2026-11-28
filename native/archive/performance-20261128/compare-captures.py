from pathlib import Path
import hashlib,json,struct

task=Path(__file__).resolve().parent
before=json.loads((task/'before-manifest.json').read_text(encoding='utf-8'))
changed=[]
for row in before['captures']:
    rel=Path(row['file'])
    folder='baseline' if rel.parts[3]=='core' else rel.parts[3]
    path=task/'captures'/folder/rel.name
    data=path.read_bytes()
    assert data[:8]==b'\x89PNG\r\n\x1a\n',path
    assert list(struct.unpack('>II',data[16:24]))==[row['width'],row['height']],path
    digest=hashlib.sha256(data).hexdigest()
    if digest!=row['sha256']:
        changed.append({'file':row['file'],'before':row['sha256'],'after':digest})
report={'captures':len(before['captures']),'byteIdentical':len(before['captures'])-len(changed),'changed':changed,'scope':'Same capture fixtures and viewports | byte comparison is not design approval'}
(task/'capture-comparison.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(json.dumps(report,ensure_ascii=False))
