
// block 00427ec0
00427ec0  mov        edx, 0xfffffffe
00427ec5  mov        dword ptr [esi], 0x4a05fc
00427ecb  and        dword ptr [esi + 0x24], edx
00427ece  and        dword ptr [esi + 0x38], edx
00427ed1  and        dword ptr [esi + 0x4c], edx
00427ed4  mov        ecx, 0x11
00427ed9  lea        eax, [esi + 0x94]
00427edf  nop        

// block 00427ee0
00427ee0  and        dword ptr [eax], edx
00427ee2  add        eax, 0x34
00427ee5  sub        ecx, 1
00427ee8  jns        0x427ee0

// block 00427eea
00427eea  and        dword ptr [esi + 0x448], edx
00427ef0  push       0x454
00427ef5  push       0
00427ef7  push       esi
00427ef8  call       0x477420

// block 00427efd
00427efd  fldz       
00427eff  mov        eax, dword ptr [esi + 0x24]
00427f02  add        esp, 0xc
00427f05  test       al, 1
00427f07  jne        0x427f27

// block 00427f09
00427f09  or         eax, 1
00427f0c  fst        dword ptr [esi + 0x1c]
00427f0f  mov        dword ptr [esi + 0x18], 0
00427f16  mov        dword ptr [esi + 0x14], 0xfff0bdc1
00427f1d  mov        dword ptr [esi + 0x20], 0x4b2ed0
00427f24  mov        dword ptr [esi + 0x24], eax

// block 00427f27
00427f27  fstp       dword ptr [esi + 0x1c]
00427f2a  mov        dword ptr [esi + 0x18], 0
00427f31  mov        dword ptr [esi + 0x14], 0xffffffff
00427f38  mov        eax, esi
00427f3a  ret        
