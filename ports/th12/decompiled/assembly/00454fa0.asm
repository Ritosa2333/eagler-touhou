
// block 00454fa0
00454fa0  push       ebp
00454fa1  push       esi
00454fa2  push       edi
00454fa3  mov        edi, ecx
00454fa5  mov        ecx, dword ptr [edx + 0x11c]
00454fab  mov        esi, dword ptr [ecx + edi*4]
00454fae  xor        ecx, ecx
00454fb0  cmp        esi, ecx
00454fb2  je         0x4550b9

// block 00454fb8
00454fb8  cmp        dword ptr [edx + 0x124], ecx
00454fbe  jne        0x4550b9

// block 00454fc4
00454fc4  mov        word ptr [eax + 0x3ea], di
00454fcb  mov        edi, dword ptr [eax + 0x47c]
00454fd1  mov        dword ptr [eax + 0x3f8], edx
00454fd7  test       edi, 0x400
00454fdd  je         0x454ffa

// block 00454fdf
00454fdf  fld        dword ptr [eax + 0x40]
00454fe2  or         edi, 8
00454fe5  fmul       qword ptr [0x4a4168]
00454feb  xor        edi, 0x400
00454ff1  mov        dword ptr [eax + 0x47c], edi
00454ff7  fstp       dword ptr [eax + 0x40]

// block 00454ffa
00454ffa  fldz       
00454ffc  mov        edi, 7
00455001  mov        word ptr [eax + 0x47c], di
00455008  mov        dword ptr [eax + 0x3bc], 0xffffffff
00455012  fst        dword ptr [eax + 0x70]
00455015  mov        dword ptr [eax + 0x6c], ecx
00455018  mov        edi, 0xfff0bdc1
0045501d  mov        dword ptr [eax + 0x68], edi
00455020  mov        dword ptr [eax + 0xe0], ecx
00455026  mov        dword ptr [eax + 0x12c], ecx
0045502c  mov        dword ptr [eax + 0x158], ecx
00455032  mov        dword ptr [eax + 0x1a4], ecx
00455038  mov        dword ptr [eax + 0x1e0], ecx
0045503e  mov        dword ptr [eax + 0x21c], ecx
00455044  mov        dword ptr [eax + 0x268], ecx
0045504a  mov        dword ptr [eax + 0x294], ecx
00455050  mov        bp, word ptr [edx]
00455053  and        dword ptr [eax + 0x47c], 0xfffff3ff
0045505d  mov        word ptr [eax + 0x3e6], bp
00455064  mov        dword ptr [eax + 0x3f8], edx
0045506a  mov        dword ptr [eax + 0x3ec], esi
00455070  mov        dword ptr [eax + 0x3f0], esi
00455076  mov        edx, dword ptr [eax + 0x78]
00455079  test       dl, 1
0045507c  jne        0x455094

// block 0045507e
0045507e  or         edx, 1
00455081  fst        dword ptr [eax + 0x70]
00455084  mov        dword ptr [eax + 0x6c], ecx
00455087  mov        dword ptr [eax + 0x68], edi
0045508a  mov        dword ptr [eax + 0x74], 0x4b2ed0
00455091  mov        dword ptr [eax + 0x78], edx

// block 00455094
00455094  fstp       dword ptr [eax + 0x70]
00455097  mov        dword ptr [eax + 0x6c], ecx
0045509a  mov        dword ptr [eax + 0x68], 0xffffffff
004550a1  and        dword ptr [eax + 0x47c], 0xfffffffe
004550a8  push       eax
004550a9  call       0x455630

// block 004550ae
004550ae  mov        eax, dword ptr [0x4ce8cc]
004550b3  inc        dword ptr [eax + 0xa0]

// block 004550b9
004550b9  pop        edi
004550ba  pop        esi
004550bb  pop        ebp
004550bc  ret        
