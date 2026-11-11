"""Expose the two verified NP21 mutable globals as WebAssembly exports."""
from pathlib import Path
import hashlib


def leb(value):
    out=bytearray()
    while True:
        byte=value&127
        value >>= 7
        out.append(byte | (128 if value else 0))
        if not value:return bytes(out)


def read_leb(data,at):
    value=shift=0
    while True:
        byte=data[at];at+=1
        value|=(byte&127)<<shift
        if byte<128:return value,at
        shift+=7
        if shift>=35:raise ValueError('invalid wasm leb')


def build(vendor):
    original=(vendor/'np21.wasm').read_bytes()
    if hashlib.sha256(original).hexdigest()!='d64bbe39549a48686b1ed04fbadec68d8b33643d220395a9c09b8d54834f36e4':
        raise ValueError('Unaudited NP21 binary')
    if original[:8]!=b'\0asm\x01\0\0\0':raise ValueError('invalid wasm')
    sections=[];at=8
    while at<len(original):
        kind=original[at];size,start=read_leb(original,at+1);end=start+size
        if end>len(original):raise ValueError('truncated wasm section')
        sections.append((kind,original[start:end]));at=end
    section=dict(sections)
    if section[4]!=bytes.fromhex('017001c411c411') or section[5]!=bytes.fromhex('0101c802c802'):
        raise ValueError('NP21 table or memory layout changed')
    if section[6]!=bytes.fromhex('027f0141f0e29d020b7f0141000b'):
        raise ValueError('NP21 native globals changed')
    count,pos=read_leb(section[7],0)
    exports=bytearray(leb(count+2)+section[7][pos:])
    for index in range(2):
        name=f'__rollback_g{index}'.encode()
        if name in original:raise ValueError('rollback export already exists')
        exports+=leb(len(name))+name+b'\x03'+leb(index)
    result=bytearray(original[:8])
    for kind,body in sections:
        if kind==7:body=exports
        result+=bytes([kind])+leb(len(body))+body
    result=bytes(result);target=vendor/'np21-rollback.wasm';target.write_bytes(result)
    return {'source_sha256':hashlib.sha256(original).hexdigest(),'sha256':hashlib.sha256(result).hexdigest(),
            'memory_bytes':328*65536,'globals':2,'table_entries':2244}


if __name__=='__main__':print(build(Path(__file__).resolve().parents[1]/'web/vendor/np2'))
