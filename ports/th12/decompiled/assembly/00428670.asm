
// block 00428670
00428670  mov        eax, dword ptr [0x4b44f4]
00428675  mov        ecx, dword ptr [eax + 0x18]
00428678  push       edi
00428679  xor        edi, edi
0042867b  cmp        ecx, edi
0042867d  je         0x4286e5

// block 0042867f
0042867f  push       esi

// block 00428680
00428680  mov        eax, dword ptr [ecx + 0x448]
00428686  fldz       
00428688  mov        esi, dword ptr [ecx + 8]
0042868b  test       al, 1
0042868d  jne        0x4286b8

// block 0042868f
0042868f  or         eax, 1
00428692  fst        dword ptr [ecx + 0x440]
00428698  mov        dword ptr [ecx + 0x43c], edi
0042869e  mov        dword ptr [ecx + 0x438], 0xfff0bdc1
004286a8  mov        dword ptr [ecx + 0x444], 0x4b2ed0
004286b2  mov        dword ptr [ecx + 0x448], eax

// block 004286b8
004286b8  fstp       dword ptr [ecx + 0x440]
004286be  mov        dword ptr [ecx + 0x43c], edi
004286c4  mov        dword ptr [ecx + 0x438], 0xffffffff
004286ce  mov        edx, dword ptr [ecx]
004286d0  mov        eax, dword ptr [edx + 0x14]
004286d3  push       edi
004286d4  push       1
004286d6  mov        dword ptr [ecx + 0x44c], edi
004286dc  call       eax

// block 004286de
004286de  mov        ecx, esi
004286e0  cmp        esi, edi
004286e2  jne        0x428680

// block 004286e4
004286e4  pop        esi

// block 004286e5
004286e5  xor        eax, eax
004286e7  pop        edi
004286e8  ret        
