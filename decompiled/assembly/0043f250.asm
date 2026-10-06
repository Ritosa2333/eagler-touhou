
// block 0043f250
0043f250  mov        dword ptr [esi], 0x4a20cc
0043f256  xor        eax, eax
0043f258  mov        dword ptr [esi + 0xb4], eax
0043f25e  mov        dword ptr [esi + 0x28], eax
0043f261  mov        dword ptr [esi + 0xfc], eax
0043f267  mov        edx, 1
0043f26c  mov        dword ptr [esi + 0xf8], edx
0043f272  mov        ecx, 0x3e7
0043f277  mov        dword ptr [esi + 0x30], ecx
0043f27a  mov        dword ptr [esi + 0x18c], eax
0043f280  mov        dword ptr [esi + 0x100], eax
0043f286  mov        dword ptr [esi + 0x1d4], eax
0043f28c  mov        dword ptr [esi + 0x1d0], edx
0043f292  mov        dword ptr [esi + 0x108], ecx
0043f298  mov        dword ptr [esi + 0x264], eax
0043f29e  mov        dword ptr [esi + 0x1d8], eax
0043f2a4  mov        dword ptr [esi + 0x2ac], eax
0043f2aa  mov        dword ptr [esi + 0x2a8], edx
0043f2b0  mov        dword ptr [esi + 0x1e0], ecx
0043f2b6  and        dword ptr [esi + 0x2c4], 0xfffffffe
0043f2bd  mov        dword ptr [esi + 0x5a1c], eax
0043f2c3  mov        dword ptr [esi + 0x5990], eax
0043f2c9  mov        dword ptr [esi + 0x5a64], eax
0043f2cf  mov        dword ptr [esi + 0x5a60], edx
0043f2d5  mov        dword ptr [esi + 0x5998], ecx
0043f2db  push       0x5c38
0043f2e0  push       eax
0043f2e1  push       esi
0043f2e2  mov        dword ptr [esi + 0x5c1c], 0x4a3738
0043f2ec  mov        dword ptr [esi + 0x5c20], eax
0043f2f2  mov        dword ptr [esi + 0x5c24], eax
0043f2f8  mov        dword ptr [esi + 0x5c28], eax
0043f2fe  mov        dword ptr [esi + 0x5c2c], eax
0043f304  call       0x477420

// block 0043f309
0043f309  or         dword ptr [esi + 4], 2
0043f30d  add        esp, 0xc
0043f310  mov        dword ptr [0x4b4530], esi
0043f316  mov        eax, esi
0043f318  ret        
