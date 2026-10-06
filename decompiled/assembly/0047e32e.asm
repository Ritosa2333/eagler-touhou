
// block 0047e32e
0047e32e  mov        edi, edi
0047e330  push       ebp
0047e331  mov        ebp, esp
0047e333  push       esi
0047e334  mov        esi, ecx
0047e336  cmp        dword ptr [esi], 9
0047e339  je         0x47e371

// block 0047e33b
0047e33b  push       edi
0047e33c  mov        edi, dword ptr [ebp + 8]
0047e33f  cmp        dword ptr [edi], 0
0047e342  je         0x47e370

// block 0047e344
0047e344  push       0
0047e346  push       8
0047e348  mov        ecx, 0x4b42f8
0047e34d  call       0x47dc29

// block 0047e352
0047e352  test       eax, eax
0047e354  je         0x47e362

// block 0047e356
0047e356  mov        ecx, dword ptr [edi]
0047e358  mov        dword ptr [eax], ecx
0047e35a  mov        ecx, dword ptr [edi + 4]
0047e35d  mov        dword ptr [eax + 4], ecx
0047e360  jmp        0x47e364

// block 0047e362
0047e362  xor        eax, eax

// block 0047e364
0047e364  test       eax, eax
0047e366  je         0x47e370

// block 0047e368
0047e368  inc        dword ptr [esi]
0047e36a  mov        ecx, dword ptr [esi]
0047e36c  mov        dword ptr [esi + ecx*4 + 4], eax

// block 0047e370
0047e370  pop        edi

// block 0047e371
0047e371  mov        eax, esi
0047e373  pop        esi
0047e374  pop        ebp
0047e375  ret        4
