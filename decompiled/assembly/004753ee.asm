
// block 004753ee
004753ee  mov        edi, edi
004753f0  push       esi
004753f1  push       edi
004753f2  call       dword ptr [0x4980e4]

// block 004753f8
004753f8  push       dword ptr [0x4adac8]
004753fe  mov        edi, eax
00475400  call       0x475279

// block 00475405
00475405  call       eax

// block 00475407
00475407  mov        esi, eax
00475409  test       esi, esi
0047540b  jne        0x47545b

// block 0047540d
0047540d  push       0x214
00475412  push       1
00475414  call       0x477813

// block 00475419
00475419  mov        esi, eax
0047541b  pop        ecx
0047541c  pop        ecx
0047541d  test       esi, esi
0047541f  je         0x47545b

// block 00475421
00475421  push       esi
00475422  push       dword ptr [0x4adac8]
00475428  push       dword ptr [0x4b4108]
0047542e  call       0x4751de

// block 00475433
00475433  pop        ecx
00475434  call       eax

// block 00475436
00475436  test       eax, eax
00475438  je         0x475452

// block 0047543a
0047543a  push       0
0047543c  push       esi
0047543d  call       0x475307

// block 00475442
00475442  pop        ecx
00475443  pop        ecx
00475444  call       dword ptr [0x4981f0]

// block 0047544a
0047544a  or         dword ptr [esi + 4], 0xffffffff
0047544e  mov        dword ptr [esi], eax
00475450  jmp        0x47545b

// block 00475452
00475452  push       esi
00475453  call       0x46c941

// block 00475458
00475458  pop        ecx
00475459  xor        esi, esi

// block 0047545b
0047545b  push       edi
0047545c  call       dword ptr [0x49813c]

// block 00475462
00475462  pop        edi
00475463  mov        eax, esi
00475465  pop        esi
00475466  ret        
