import hashlib
import struct
from .formats import require,span
from .ice_masks import EXE_SHA256

def read_profile(binary,address,script):
    require(0<len(binary)<=1048576 and hashlib.sha256(binary).hexdigest()==EXE_SHA256,'bullet profiles require pinned original Japanese executable')
    header=struct.unpack_from('<I',span(binary,0x3c,4))[0];require(span(binary,header,4)==b'PE\0\0','invalid original PE signature')
    sections=struct.unpack_from('<H',span(binary,header+6,2))[0];optional=struct.unpack_from('<H',span(binary,header+20,2))[0]
    require(sections<=96,'invalid original section count');relative=address-0x400000;matches=[]
    for index in range(sections):
        section=span(binary,header+24+optional+index*40,40);virtual,size,raw=struct.unpack_from('<III',section,12)
        if virtual<=relative and relative+288<=virtual+size:matches.append(span(binary,raw+relative-virtual,288))
    require(len(matches)==1,'ambiguous original bullet table');profile=matches[0]
    require(struct.unpack_from('<i',profile)[0]==script,'unexpected original bullet animation')
    return profile

def read_bullet_profiles(binary):
    profile=read_profile(binary,0x4b2950,39)
    return {'schema':'th128-original-bullet-profiles/1','sourceExeSha256':EXE_SHA256,
        'profiles':[{'type':4,'address':'0x004b2950','hex':profile.hex(),'sha256':hashlib.sha256(profile).hexdigest()}]}

def read_boss_bullet_profiles(binary):
    profile=read_profile(binary,0x4b24d0+10*0x120,45)
    return {'schema':'th128-original-boss-bullet-profiles/1','sourceExeSha256':EXE_SHA256,
        'profiles':[{'type':10,'hex':profile.hex(),'sha256':hashlib.sha256(profile).hexdigest()}]}
