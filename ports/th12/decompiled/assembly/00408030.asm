
// block 00408030
00408030  push       ecx
00408031  cmp        dword ptr [edi], 0
00408034  push       ebx
00408035  push       esi
00408036  je         0x4080b5

// block 00408038
00408038  fld        dword ptr [edi + 4]
0040803b  lea        esi, [edi + 4]
0040803e  push       ecx
0040803f  mov        eax, 0x1c
00408044  fstp       dword ptr [esp]
00408047  call       0x453e20

// block 0040804c
0040804c  fld        dword ptr [0x4a3db8]
00408052  mov        eax, dword ptr [0x4b43cc]
00408057  mov        ecx, dword ptr [eax + 0x7c]
0040805a  not        ecx
0040805c  push       1
0040805e  and        ecx, 1
00408061  push       ecx
00408062  push       ecx
00408063  mov        ebx, esi
00408065  fstp       dword ptr [esp]
00408068  call       0x40caa0

// block 0040806d
0040806d  fld        dword ptr [0x4a3db8]
00408073  mov        edx, dword ptr [0x4b43cc]
00408079  mov        eax, dword ptr [edx + 0x7c]
0040807c  not        eax
0040807e  and        eax, 1
00408081  push       1
00408083  or         eax, 2
00408086  push       eax
00408087  push       ecx
00408088  fstp       dword ptr [esp]
0040808b  call       0x4286f0

// block 00408090
00408090  fld        dword ptr [0x4a3e54]
00408096  mov        eax, dword ptr [0x4b4514]
0040809b  push       0x3c
0040809d  push       0xb
0040809f  sub        esp, 8
004080a2  fstp       dword ptr [esp + 4]
004080a6  fld        dword ptr [0x4a3e04]
004080ac  fstp       dword ptr [esp]
004080af  push       esi
004080b0  call       0x4390f0

// block 004080b5
004080b5  mov        ecx, dword ptr [edi]
004080b7  push       ecx
004080b8  mov        ebx, 1
004080bd  call       0x461970

// block 004080c2
004080c2  mov        eax, dword ptr [edi + 0xb0]
004080c8  mov        dword ptr [edi], 0
004080ce  mov        dword ptr [edi + 0x84], 0
004080d8  test       eax, eax
004080da  je         0x4080e0

// block 004080dc
004080dc  and        dword ptr [eax + 0x70], 0xfffffffe

// block 004080e0
004080e0  pop        esi
004080e1  mov        dword ptr [edi + 0xb0], 0
004080eb  pop        ebx
004080ec  pop        ecx
004080ed  ret        
