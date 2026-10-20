
// block 00408510
00408510  mov        eax, dword ptr [eax + 0x500]
00408516  push       ebx
00408517  push       esi
00408518  push       edi
00408519  lea        esi, [eax + 4]
0040851c  mov        edi, 8

// block 00408521
00408521  cmp        dword ptr [esi + 0x80], 0
00408528  je         0x40856e

// block 0040852a
0040852a  mov        ecx, dword ptr [0x4b43cc]
00408530  fld        dword ptr [0x4a3e04]
00408536  mov        edx, dword ptr [ecx + 0x7c]
00408539  not        edx
0040853b  push       1
0040853d  and        edx, 1
00408540  push       edx
00408541  push       ecx
00408542  mov        ebx, esi
00408544  fstp       dword ptr [esp]
00408547  call       0x40caa0

// block 0040854c
0040854c  fld        dword ptr [0x4a3e04]
00408552  mov        eax, dword ptr [0x4b43cc]
00408557  mov        ecx, dword ptr [eax + 0x7c]
0040855a  not        ecx
0040855c  and        ecx, 1
0040855f  push       1
00408561  or         ecx, 2
00408564  push       ecx
00408565  push       ecx
00408566  fstp       dword ptr [esp]
00408569  call       0x4286f0

// block 0040856e
0040856e  add        esi, 0xb4
00408574  sub        edi, 1
00408577  jne        0x408521

// block 00408579
00408579  pop        edi
0040857a  pop        esi
0040857b  xor        eax, eax
0040857d  pop        ebx
0040857e  ret        
