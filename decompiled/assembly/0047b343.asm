
// block 0047b343
0047b343  mov        edi, edi
0047b345  push       esi
0047b346  mov        eax, 0x4aa9f4
0047b34b  mov        esi, 0x4aa9f4
0047b350  push       edi
0047b351  mov        edi, eax
0047b353  cmp        eax, esi
0047b355  jae        0x47b366

// block 0047b357
0047b357  mov        eax, dword ptr [edi]
0047b359  test       eax, eax
0047b35b  je         0x47b35f

// block 0047b35d
0047b35d  call       eax

// block 0047b35f
0047b35f  add        edi, 4
0047b362  cmp        edi, esi
0047b364  jb         0x47b357

// block 0047b366
0047b366  pop        edi
0047b367  pop        esi
0047b368  ret        
