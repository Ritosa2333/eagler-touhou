"""GPU-free native ANMFile tables for TH12 1.00b Unicorn diagnostics.

The caller maps the supplied region before calling install_resource. This is
not a replacement for the original loader's texture/D3D implementation.
Layout consumed directly by 454d10/454b80: ANMFile+108 loaded flag, +118
72-byte sprite records, +11c script pointer array, +124 invalid-resource flag.
"""
import struct,hashlib,pathlib
EXE_SHA256='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
def check_original(exe):
    if hashlib.sha256(pathlib.Path(exe).read_bytes()).hexdigest()!=EXE_SHA256:
        raise ValueError('Pinned TH12 1.00b executable required')
def install_resource(vm,raw,base,bank_id=0):
    u16=lambda at:struct.unpack_from('<H',raw,at)[0]
    u32=lambda at:struct.unpack_from('<I',raw,at)[0]
    entry=0;scripts=[];sprites=[]
    while True:
        if u32(entry)!=7:raise ValueError('ANM v7 required')
        ns,nc=u16(entry+4),u16(entry+6);w,h=u16(entry+10),u16(entry+12)
        for i in range(ns):
            at=entry+u32(entry+64+i*4);sid,x,y,sw,sh=struct.unpack_from('<Iffff',raw,at)
            record=bytearray(72)
            # Original 454b80 reads these precise fields, including non-zero
            # denominator dimensions when constructing the transform matrices.
            struct.pack_into('<fffffffffff',record,0x1c,float(h or 1),float(w or 1),
                x/(w or 1),(x+sw)/(w or 1),y/(h or 1),(y+sh)/(h or 1),sh,sw,1.,1.,0.)
            sprites.append(bytes(record))
        for i in range(nc):
            sid,off=struct.unpack_from('<iI',raw,entry+64+ns*4+i*8)
            if sid!=len(scripts):raise ValueError('Non-contiguous native script ID')
            scripts.append(entry+off)
        nxt=u32(entry+36)
        if not nxt:break
        entry+=nxt
    resource=base;raw_base=base+0x200
    sprite_table=(raw_base+len(raw)+15)&~15
    script_table=(sprite_table+len(sprites)*72+15)&~15
    vm.mem_write(resource,bytes(0x140));vm.mem_write(raw_base,raw)
    if sprites:vm.mem_write(sprite_table,b''.join(sprites))
    if scripts:vm.mem_write(script_table,struct.pack('<'+'I'*len(scripts),*(raw_base+off for off in scripts)))
    vm.mem_write(resource,struct.pack('<H',bank_id))
    for off,value in [(0x108,1),(0x110,len(sprites)),(0x114,0),(0x118,sprite_table),(0x11c,script_table),(0x120,len(scripts)),(0x124,0)]:
        vm.mem_write(resource+off,struct.pack('<I',value))
    return {'resource':resource,'raw':raw_base,'sprites':sprite_table,'scripts':script_table,
        'spriteCount':len(sprites),'scriptCount':len(scripts),'end':script_table+len(scripts)*4}
