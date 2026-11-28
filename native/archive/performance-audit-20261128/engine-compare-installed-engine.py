from pathlib import Path
import hashlib,json,struct,uuid

root=Path(__file__).resolve().parent
paths=[Path('D:/workspace/eagler/toolchains/flutter-stable/bin/cache/artifacts/engine/windows-x64-release/flutter_windows.dll'),Path('C:/Program Files/FlClash/flutter_windows.dll')]
def sha(data): return hashlib.sha256(data).hexdigest()
def inspect(path):
    data=path.read_bytes()
    pe=struct.unpack_from('<I',data,60)[0]
    assert data[pe:pe+4]==b'PE\0\0'
    section_count=struct.unpack_from('<H',data,pe+6)[0]
    optional_size=struct.unpack_from('<H',data,pe+20)[0]
    optional=pe+24
    assert struct.unpack_from('<H',data,optional)[0]==0x20b
    sections={}
    for i in range(section_count):
        off=optional+optional_size+40*i
        name=data[off:off+8].rstrip(b'\0').decode('ascii')
        virtual_size,rva,size,raw=struct.unpack_from('<IIII',data,off+8)
        sections[name]={'rva':rva,'virtualBytes':virtual_size,'offset':raw,'bytes':size,'sha256':sha(data[raw:raw+size])}
    def raw(rva):
        for section in sections.values():
            if section['rva']<=rva<section['rva']+section['bytes']:
                return section['offset']+rva-section['rva']
        raise ValueError(hex(rva))
    metadata=[]
    def field(name,offset,size):
        metadata.append({'name':name,'offset':offset,'bytes':size,'hex':data[offset:offset+size].hex()})
    field('COFF TimeDateStamp',pe+8,4)
    # Version strings are build provenance, distinct from PE debug metadata.
    # The SDK value is verified against its own cache manifest; the installed
    # reference is read at the same code/data layout and must be a full hash.
    manifest=json.loads(Path('D:/workspace/eagler/toolchains/flutter-stable/bin/cache/flutter.version.json').read_text())
    pinned_revision=manifest['engineRevision'].encode('ascii')
    current_bytes=paths[0].read_bytes()
    assert current_bytes.count(pinned_revision)==1
    revision_offset=current_bytes.index(pinned_revision)
    revision=data[revision_offset:revision_offset+40]
    assert len(revision)==40 and all(c in b'0123456789abcdef' for c in revision)
    assert data[revision_offset-1]==0 and data[revision_offset+40]==0
    if path==paths[0]:
        assert revision.decode()==manifest['engineRevision']
    field('Embedded engine revision string',revision_offset,40)
    export_rva,export_size=struct.unpack_from('<II',data,optional+112)
    if export_rva: field('Export TimeDateStamp',raw(export_rva)+4,4)
    debug_rva,debug_size=struct.unpack_from('<II',data,optional+112+6*8)
    debug=[]
    if debug_rva:
        assert debug_size%28==0
        for i in range(debug_size//28):
            off=raw(debug_rva)+28*i
            _,stamp,major,minor,kind,size,rva,pointer=struct.unpack_from('<IIHHIIII',data,off)
            field(f'Debug[{i}] TimeDateStamp',off+4,4)
            item={'type':kind,'offset':off,'payloadOffset':pointer,'payloadBytes':size}
            payload=data[pointer:pointer+size]
            assert len(payload)==size
            if kind==2 and payload[:4]==b'RSDS':
                item.update(guid=str(uuid.UUID(bytes_le=payload[4:20])),age=struct.unpack_from('<I',payload,20)[0],pdb=payload[24:].split(b'\0')[0].decode('utf-8'))
                field(f'Debug[{i}] CodeView PDB GUID',pointer+4,16)
            elif kind==19:
                algorithm,digest=payload.split(b'\0',1)
                item.update(algorithm=algorithm.decode('ascii'),digest=digest.hex())
                field(f'Debug[{i}] PDB checksum',pointer+len(algorithm)+1,len(digest))
            debug.append(item)
    return data,{'path':str(path),'sha256':sha(data),'bytes':len(data),'sections':sections,'metadata':metadata,'debugDirectories':debug}

first,a=inspect(paths[0]);second,b=inspect(paths[1])
assert len(first)==len(second)
differences=[i for i,(x,y) in enumerate(zip(first,second)) if x!=y]
assert [(x['name'],x['offset'],x['bytes']) for x in a['metadata']]==[(x['name'],x['offset'],x['bytes']) for x in b['metadata']]
classified=[];covered=set()
for left,right in zip(a['metadata'],b['metadata']):
    offsets=[i for i in differences if left['offset']<=i<left['offset']+left['bytes']]
    if offsets:
        classified.append({'name':left['name'],'offset':left['offset'],'bytes':left['bytes'],'differentBytes':len(offsets),'currentHex':left['hex'],'referenceHex':right['hex']})
        covered.update(offsets)
normalized=[bytearray(first),bytearray(second)]
for item in a['metadata']:
    for data in normalized: data[item['offset']:item['offset']+item['bytes']]=bytes(item['bytes'])
result={'scope':'Read-only installed engine comparison. No running process interaction, binary replacement or renderer selection inference. Embedded engine revision differs; machine code equality does not establish source commit identity.','current':a,'installedFlClash':b,'differentByteCount':len(differences),'classifiedDifferences':classified,'unclassifiedDifferenceOffsets':sorted(set(differences)-covered),'equalSections':[name for name in a['sections'] if a['sections'][name]['sha256']==b['sections'][name]['sha256']],'normalizedBuildProvenanceEqual':normalized[0]==normalized[1],'normalizedSha256':[sha(x) for x in normalized],'peSpecification':'https://learn.microsoft.com/en-us/windows/win32/debug/pe-format'}
(root/'installed-engine-comparison.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
assert not result['unclassifiedDifferenceOffsets']
assert result['normalizedBuildProvenanceEqual']
print(json.dumps({key:result[key] for key in ['differentByteCount','classifiedDifferences','unclassifiedDifferenceOffsets','equalSections','normalizedBuildProvenanceEqual']},indent=2))
