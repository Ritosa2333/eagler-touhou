from pathlib import Path
import bisect,hashlib,json,re,struct
p=Path(__file__).resolve().parent
original=p.parent/'preview/flutter_inappwebview_windows_plugin.dll'
mapped=p/'flutter_inappwebview_windows_plugin.dll'
def sections(path):
    data=path.read_bytes();pe=struct.unpack_from('<I',data,60)[0]
    count=struct.unpack_from('<H',data,pe+6)[0];size=struct.unpack_from('<H',data,pe+20)[0]
    out={}
    for i in range(count):
        off=pe+24+size+40*i;name=data[off:off+8].rstrip(b'\0').decode()
        length,raw=struct.unpack_from('<II',data,off+16)
        out[name]={'bytes':length,'sha256':hashlib.sha256(data[raw:raw+length]).hexdigest()}
    return out
a=sections(original);b=sections(mapped)
assert a['.text']==b['.text'],(a['.text'],b['.text'])
text=(p/'webview.map').read_text(errors='strict')
base=int(re.search(r'Preferred load address is ([0-9A-Fa-f]+)',text)[1],16)
symbols=[]
for line in text.splitlines():
    match=re.match(r'\s+([0-9A-Fa-f]{4}):([0-9A-Fa-f]{8})\s+(\S+)\s+([0-9A-Fa-f]{16})\s+(.*)',line)
    if match and ' f ' in ' '+match[5]+' ':
        symbols.append({'rva':int(match[4],16)-base,'name':match[3],'object':match[5]})
symbols.sort(key=lambda s:s['rva']);addresses=[s['rva'] for s in symbols]
stacks=json.loads((p.parent/'close-stack-320.json').read_text(encoding='utf-8-sig'))
resolved=[]
for stack in stacks:
    for i,frame in enumerate(stack['stackFrames']):
        if Path(frame['module']).name!=original.name:continue
        rva=int(frame['offset'],16);at=bisect.bisect_right(addresses,rva)-1
        assert at>=0
        nearest=symbols[at]
        resolved.append({'thread':stack['tid'],'frame':i,'rva':frame['offset'],'symbolRva':hex(nearest['rva']),'displacement':rva-nearest['rva'],'mapPublicSymbol':nearest['name'],'object':nearest['object']})
result={'scope':'Relink existing94object inputs with /MAP only, into isolated files. Machine code is byte-identical. Public map names identify nearby functions; no private PDB/source-line or exact function-boundary claim.','originalSha256':hashlib.sha256(original.read_bytes()).hexdigest(),'mappedSha256':hashlib.sha256(mapped.read_bytes()).hexdigest(),'text':a['.text'],'equalSections':[key for key in a if a[key]==b.get(key)],'mapSha256':hashlib.sha256((p/'webview.map').read_bytes()).hexdigest(),'resolved':resolved}
(p/'resolved.json').write_text(json.dumps(result,indent=2)+'\n')
for frame in resolved:print(frame['rva'],frame['displacement'],frame['mapPublicSymbol'],frame['object'])
