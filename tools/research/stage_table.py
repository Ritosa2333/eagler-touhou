import argparse
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from th128_assets.formats import require, validate_archive_name
from th128_assets.pipeline import PROJECT, atomic_json


EXPECTED_EXE='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'


def extract_stage_table(executable, available):
    import pefile

    with executable.open('rb') as source:
        data=source.read(16*1024*1024+1)
    require(hashlib.sha256(data).hexdigest()==EXPECTED_EXE,'stage-table addresses require the pinned Japanese executable')
    image=pefile.PE(data=data)
    base=image.OPTIONAL_HEADER.ImageBase

    def text_at(address):
        require(base<=address<base+image.OPTIONAL_HEADER.SizeOfImage,'stage string pointer outside image')
        raw=image.get_data(address-base,256)
        ending=raw.find(b'\0')
        require(ending>0,'unterminated stage string')
        try:
            return raw[:ending].decode('ascii')
        except UnicodeDecodeError as error:
            raise ValueError('non-ASCII native stage resource name') from error

    start, stride=0x4B1A30,40
    table=image.get_data(start-base,17*stride)
    require(len(table)==17*stride,'truncated native stage table')
    stages=[]
    for identity in range(1,17):
        row=table[identity*stride:(identity+1)*stride]
        values=struct.unpack('<10I',row)
        require(values[0]==identity,'native stage-table identity mismatch')
        names=[text_at(pointer) for pointer in values[1:7]]
        background,ecl,music,boss_music,message,logo=names
        for resource in (background,ecl,message,logo):
            validate_archive_name(resource)
            require(resource in available,f'native stage resource missing from archive: {resource}')
        tracks=[]
        for resource in (music,boss_music):
            require(re.fullmatch(r'bgm/th128_[0-9]{2}\.wav',resource) is not None,'unexpected native music reference')
            tracks.append(resource.removeprefix('bgm/'))
        stages.append({'id':identity,'address':hex(start+identity*stride),'raw':row.hex(),
                       'background':background,'ecl':ecl,'music':tracks,'message':message,'logo':logo,
                       'uninterpretedFields':list(values[7:]),
                       'pointers':[hex(pointer) for pointer in values[1:7]]})
    image.close()
    return {'schema':'th128-original-stage-table/1','exeSha256':EXPECTED_EXE,'address':hex(start),
            'stride':stride,'stages':stages,'tableSha256':hashlib.sha256(table).hexdigest(),
            'windowsGameProcessLaunched':False,
            'limitations':['Static resource table, not proof of dynamic route selection or progression.',
                           'Index zero is a debug record referring to non-TH128 resources and is deliberately excluded.',
                           'The last three fields are retained without assigning unverified semantics.']}


def main(argv=None):
    parser=argparse.ArgumentParser(description='Read actual native stage resource mappings; do not infer backgrounds from ECL filenames.')
    parser.add_argument('--exe',type=Path,required=True)
    parser.add_argument('--catalog',type=Path,default=PROJECT/'local/catalog.json')
    parser.add_argument('--report',type=Path,default=PROJECT/'local/research/stage-table.json')
    arguments=parser.parse_args(argv)
    try:
        destination=arguments.report.absolute()
        research=(PROJECT/'local/research').absolute()
        require(destination.parent==research and destination.name=='stage-table.json','stage report must use local/research/stage-table.json')
        require(research.resolve()==research,'research directory must not be redirected')
        catalog=json.loads(arguments.catalog.read_text(encoding='utf-8'))
        manifest_path=arguments.catalog.parent/catalog['variants']['ja']['manifest']
        require(manifest_path.resolve().is_relative_to(arguments.catalog.parent.resolve()),'manifest path outside private content')
        manifest=json.loads(manifest_path.read_text(encoding='utf-8'))
        report=extract_stage_table(arguments.exe,{item['name'] for item in manifest['files']})
        require(research.resolve()==research,'research destination changed')
        atomic_json(destination,report)
        print(f'Extracted {len(report["stages"])} native stage mappings; all archive references exist.')
        print('Example: st_a1_2.ecl uses stage_b2.std, not stage_a2.std.')
    except (OSError,ValueError,KeyError,ImportError) as error:
        print(f'Stage table failed: {error}',file=sys.stderr)
        return 1
    return 0


if __name__=='__main__':
    raise SystemExit(main())
