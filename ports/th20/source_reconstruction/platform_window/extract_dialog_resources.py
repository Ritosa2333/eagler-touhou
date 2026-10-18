"""Recover two ordinary Win32 dialogs into reviewable textual RC source.

Read-only PE resources, never instructions. The resource compiler regenerates
the DLGTEMPLATE data; the resulting .res can be byte-compared independently.
"""
from pathlib import Path
import argparse, struct, hashlib, json
HERE=Path(__file__).resolve().parent
EXPECTED='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
def original_dialogs(exe):
    raw=Path(exe).read_bytes();assert hashlib.sha256(raw).hexdigest()==EXPECTED
    pe=struct.unpack_from('<I',raw,0x3c)[0];n=struct.unpack_from('<H',raw,pe+6)[0]
    sec=pe+24+struct.unpack_from('<H',raw,pe+20)[0]
    def off(rva):
        for i in range(n):
            _,start,size,pos=struct.unpack_from('<IIII',raw,sec+i*40+8)
            if start<=rva<start+size:return pos+rva-start
        raise ValueError(hex(rva))
    root=off(struct.unpack_from('<I',raw,pe+24+96+2*8)[0])
    def entries(relative):
        at=root+relative;named,count=struct.unpack_from('<HH',raw,at+12)
        return [struct.unpack_from('<II',raw,at+16+8*i) for i in range(named+count)]
    dialogs={}
    for typ,target in entries(0):
        if typ!=5:continue
        for name,target in entries(target&0x7fffffff):
            if name not in (203,204):continue
            for lang,target in entries(target&0x7fffffff):
                rva,size=struct.unpack_from('<II',raw,root+target)
                dialogs[(name,lang)]=raw[off(rva):off(rva)+size]
    return dialogs
class Reader:
    def __init__(self,data):self.data=data;self.at=0
    def take(self,fmt):
        out=struct.unpack_from('<'+fmt,self.data,self.at);self.at+=struct.calcsize('<'+fmt);return out
    def string(self):
        n=self.take('H')[0]
        if n==0xffff:return self.take('H')[0]
        units=[]
        while n:units.append(n);n=self.take('H')[0]
        return struct.pack('<'+'H'*len(units),*units).decode('utf-16le')
    def align(self):self.at=(self.at+3)&~3
def quoted(value):
    if isinstance(value,int):return str(value)
    return '"'+value.replace('\\','\\\\').replace('"','""').replace('\r','\\r').replace('\n','\\n').replace('\t','\\t')+'"'
def to_rc(name,lang,data):
    r=Reader(data)
    ver,sig,helpid,exstyle,style,count,x,y,cx,cy=r.take('HHIIIHhhhh')
    assert (ver,sig)==(1,65535)
    menu,klass,title=r.string(),r.string(),r.string()
    lines=[f'LANGUAGE {lang&0x3ff}, {lang>>10}',f'{name} DIALOGEX {x}, {y}, {cx}, {cy}, {helpid}',
           f'STYLE 0x{style:08x}',f'EXSTYLE 0x{exstyle:08x}',f'CAPTION {quoted(title)}']
    if menu:lines.append('MENU '+quoted(menu))
    if klass:lines.append('CLASS '+quoted(klass))
    if style&0x40:
        size,weight,italic,charset=r.take('HHBB');font=r.string()
        lines.append(f'FONT {size}, {quoted(font)}, {weight}, {italic}, {charset}')
    lines.append('BEGIN')
    classes={0x80:'Button',0x81:'Edit',0x82:'Static',0x83:'ListBox',0x84:'ScrollBar',0x85:'ComboBox'}
    for i in range(count):
        r.align();helpid,exstyle,style,x,y,cx,cy,ident=r.take('IIIhhhhI')
        klass,title=r.string(),r.string();extra=r.take('H')[0]
        assert extra==0, 'custom control creation data requires explicit recovery'
        klass=classes.get(klass,klass)
        if ident==0xffffffff:ident=-1
        lines.append(f'    CONTROL {quoted(title)}, {ident}, {quoted(klass)}, 0x{style:08x}, {x}, {y}, {cx}, {cy}, 0x{exstyle:08x}, {helpid}')
    assert r.at==len(data) or not any(data[r.at:])
    return '\n'.join(lines+['END',''])
def res_dialogs(path):
    data=Path(path).read_bytes();at=0;out={}
    while at<len(data):
        size,header=struct.unpack_from('<II',data,at)
        r=Reader(data);r.at=at+8;typ,name=r.string(),r.string();r.align()
        _,_,lang,_,_=r.take('IHHII')
        if typ==5:out[(name,lang)]=data[at+header:at+header+size]
        at=(at+header+size+3)&~3
    return out
def main():
    ap=argparse.ArgumentParser();ap.add_argument('exe');ap.add_argument('--verify-res',type=Path);args=ap.parse_args()
    dialogs=original_dialogs(args.exe)
    if args.verify_res:
        rebuilt=res_dialogs(args.verify_res)
        report={'specimen_sha256':EXPECTED,'status':'passed','resources':[]}
        for key,value in dialogs.items():
            equal=rebuilt.get(key)==value
            if not equal:report['status']='failed'
            report['resources'].append({'id':key[0],'language':key[1],'bytes':len(value),'equal':equal,
              'original_sha256':hashlib.sha256(value).hexdigest(),
              'compiled_sha256':hashlib.sha256(rebuilt.get(key,b'')).hexdigest()})
        (HERE/'dialog_validation.json').write_text(json.dumps(report,indent=2)+'\n')
        print(json.dumps(report,indent=2));raise SystemExit(report['status']!='passed')
    lines=['#pragma code_page(65001)','#include <windows.h>', '// Data-only resources recovered as editable source, no EXE embedding.','']
    lines.extend(to_rc(name,lang,value) for (name,lang),value in sorted(dialogs.items()))
    (HERE/'dialogs.rc').write_text('\n'.join(lines),encoding='utf-8')
    print('Recovered',len(dialogs),'dialog resource variants')
if __name__=='__main__':main()
