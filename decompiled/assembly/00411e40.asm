
// block 00411e40
00411e40  fldz       
00411e42  and        dword ptr [eax + 0x1028], 0xfffffffe
00411e49  xor        edx, edx
00411e4b  fstp       dword ptr [eax + 8]
00411e4e  lea        ecx, [eax + 8]
00411e51  mov        dword ptr [eax + 0xc], edx
00411e54  mov        dword ptr [eax + 0x101c], eax
00411e5a  mov        dword ptr [eax + 0x1018], 0xffffffff
00411e64  mov        dword ptr [eax + 0x1020], edx
00411e6a  mov        dword ptr [eax + 4], ecx
00411e6d  mov        dword ptr [eax + 0x1030], ecx
00411e73  mov        dword ptr [eax + 0x1034], edx
00411e79  mov        dword ptr [eax + 0x1038], edx
00411e7f  ret        
