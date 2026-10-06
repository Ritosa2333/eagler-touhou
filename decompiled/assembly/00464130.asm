
// block 00464130
00464130  push       ecx
00464131  mov        eax, dword ptr [0x4ae590]
00464136  cmp        eax, -1
00464139  jne        0x46413f

// block 0046413b
0046413b  or         eax, eax
0046413d  pop        ecx
0046413e  ret        

// block 0046413f
0046413f  push       0
00464141  lea        ecx, [esp + 4]
00464145  push       ecx
00464146  push       esi
00464147  push       edx
00464148  push       eax
00464149  call       dword ptr [0x4980ac]

// block 0046414f
0046414f  cmp        esi, dword ptr [esp]
00464152  je         0x464184

// block 00464154
00464154  mov        eax, dword ptr [0x4ae590]
00464159  push       eax
0046415a  call       dword ptr [0x4980a4]

// block 00464160
00464160  test       dword ptr [0x4cee78], 0x8000
0046416a  je         0x46417d

// block 0046416c
0046416c  push       0x4cf128
00464171  call       dword ptr [0x49808c]

// block 00464177
00464177  dec        byte ptr [0x4cf21a]

// block 0046417d
0046417d  mov        eax, 0xfffffffe
00464182  pop        ecx
00464183  ret        

// block 00464184
00464184  xor        eax, eax
00464186  pop        ecx
00464187  ret        
