import hashlib
import struct
from .formats import require,span

EXE_SHA256='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
PROFILES=((0x4b1ef8,21,1),(0x4b1f50,25,1),(0x4b1fb8,33,1),(0x4b2040,49,2),(0x4b21c8,65,2))

def read_ice_masks(binary):
    require(0<len(binary)<=1048576,'ice masks require bounded pinned Japanese TH128 executable')
    digest=hashlib.sha256(binary).hexdigest()
    require(digest==EXE_SHA256,'ice masks require the pinned Japanese TH128 executable; no approximate masks are generated')
    header=struct.unpack_from('<I',span(binary,0x3c,4))[0]
    require(span(binary,header,4)==b'PE\0\0','invalid original PE signature')
    sections=struct.unpack_from('<H',span(binary,header+6,2))[0]
    optional_size=struct.unpack_from('<H',span(binary,header+20,2))[0]
    optional=header+24
    require(sections<=96 and struct.unpack_from('<I',span(binary,optional+28,4))[0]==0x400000,'unexpected original image layout')
    profiles=[]
    for selector,(address,rows,words) in enumerate(PROFILES,1):
        relative=address-0x400000;size=rows*words*4;matches=[]
        for index in range(sections):
            section=span(binary,optional+optional_size+index*40,40)
            virtual,raw_size,raw=struct.unpack_from('<III',section,12)
            if virtual<=relative and relative+size<=virtual+raw_size:matches.append(span(binary,raw+relative-virtual,size))
        require(len(matches)==1,'ambiguous/unmapped original ice bitmap')
        payload=matches[0];profiles.append({'selector':selector,'rows':rows,'words':words,'hex':payload.hex(),
            'sha256':hashlib.sha256(payload).hexdigest(),'address':f'0x{address:08x}'})
    return {'schema':'th128-original-ice-masks/1','sourceExeSha256':digest,'profiles':profiles}
