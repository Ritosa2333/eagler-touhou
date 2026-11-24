import json
from pathlib import Path
from th128_assets.messages import parse_stage_messages
from th128_assets.pipeline import atomic_json

root=Path(__file__).resolve().parents[1];local=root/'local';catalog=json.loads((local/'catalog.json').read_text(encoding='utf8'));summary=[]
for variant,record in catalog['variants'].items():
    directory=(local/record['manifest']).parent
    for source in sorted((directory/'raw').glob('st_*.msg')):
        parsed=parse_stage_messages(source.read_bytes(),encoding='cp932' if variant=='ja' else 'gbk')
        atomic_json(directory/'animations'/f'message-{source.stem}.json',parsed)
        summary.append({'variant':variant,'source':source.name,'sha256':parsed['sourceSha256'],'entries':len(parsed['entries']),
            'commands':parsed['commandCount'],'decodedOnly':True,'executed':False})
atomic_json(root/'docs/stage-message-inventory.json',{'schema':'th128-stage-message-inventory/1','sources':summary,'dialogueGameplayImplemented':False})
print(f'{len(summary)} original route dialogue resources decoded with exact source hashes; dialogue execution remains unimplemented')
