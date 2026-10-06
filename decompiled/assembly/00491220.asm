
// block 00491220
00491220  push       8
00491222  push       0x4ab208
00491227  call       0x46fcec

// block 0049122c
0049122c  xor        eax, eax
0049122e  cmp        dword ptr [0x4d52dc], eax
00491234  je         0x49128c

// block 00491236
00491236  test       byte ptr [ebp + 8], 0x40
0049123a  je         0x491284

// block 0049123c
0049123c  cmp        dword ptr [0x4ae448], eax
00491242  je         0x491284

// block 00491244
00491244  mov        dword ptr [ebp - 4], eax
00491247  ldmxcsr    dword ptr [ebp + 8]
0049124b  jmp        0x49127b

// block 0049127b
0049127b  mov        dword ptr [ebp - 4], 0xfffffffe
00491282  jmp        0x49128c

// block 00491284
00491284  and        dword ptr [ebp + 8], 0xffffffbf
00491288  ldmxcsr    dword ptr [ebp + 8]

// block 0049128c
0049128c  call       0x46fd31

// block 00491291
00491291  ret        
