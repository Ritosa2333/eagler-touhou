
// block 0046f10e
0046f10e  mov        eax, dword ptr [0x4d6450]
0046f113  push       esi
0046f114  mov        esi, dword ptr [0x4d6440]
0046f11a  push       edi
0046f11b  xor        edi, edi
0046f11d  cmp        esi, eax
0046f11f  jne        0x46f155

// block 0046f121
0046f121  add        eax, 0x10
0046f124  imul       eax, eax, 0x14
0046f127  push       eax
0046f128  push       dword ptr [0x4d6444]
0046f12e  push       edi
0046f12f  push       dword ptr [0x4b3c04]
0046f135  call       dword ptr [0x498180]

// block 0046f13b
0046f13b  cmp        eax, edi
0046f13d  jne        0x46f143

// block 0046f13f
0046f13f  xor        eax, eax
0046f141  jmp        0x46f1bb

// block 0046f143
0046f143  add        dword ptr [0x4d6450], 0x10
0046f14a  mov        esi, dword ptr [0x4d6440]
0046f150  mov        dword ptr [0x4d6444], eax

// block 0046f155
0046f155  imul       esi, esi, 0x14
0046f158  add        esi, dword ptr [0x4d6444]
0046f15e  push       0x41c4
0046f163  push       8
0046f165  push       dword ptr [0x4b3c04]
0046f16b  call       dword ptr [0x4981d4]

// block 0046f171
0046f171  mov        dword ptr [esi + 0x10], eax
0046f174  cmp        eax, edi
0046f176  je         0x46f13f

// block 0046f178
0046f178  push       4
0046f17a  push       0x2000
0046f17f  push       0x100000
0046f184  push       edi
0046f185  call       dword ptr [0x498184]

// block 0046f18b
0046f18b  mov        dword ptr [esi + 0xc], eax
0046f18e  cmp        eax, edi
0046f190  jne        0x46f1a4

// block 0046f192
0046f192  push       dword ptr [esi + 0x10]
0046f195  push       edi
0046f196  push       dword ptr [0x4b3c04]
0046f19c  call       dword ptr [0x4981d0]

// block 0046f1a2
0046f1a2  jmp        0x46f13f

// block 0046f1a4
0046f1a4  or         dword ptr [esi + 8], 0xffffffff
0046f1a8  mov        dword ptr [esi], edi
0046f1aa  mov        dword ptr [esi + 4], edi
0046f1ad  inc        dword ptr [0x4d6440]
0046f1b3  mov        eax, dword ptr [esi + 0x10]
0046f1b6  or         dword ptr [eax], 0xffffffff
0046f1b9  mov        eax, esi

// block 0046f1bb
0046f1bb  pop        edi
0046f1bc  pop        esi
0046f1bd  ret        
