
// block 0043f320
0043f320  call       0x43f3d0

// block 0043f325
0043f325  test       eax, eax
0043f327  je         0x43f341

// block 0043f329
0043f329  mov        eax, dword ptr [0x4cee78]
0043f32e  shr        eax, 0xd
0043f331  not        eax
0043f333  and        eax, 1
0043f336  or         eax, 2
0043f339  mov        dword ptr [0x4cee40], eax
0043f33e  xor        eax, eax
0043f340  ret        

// block 0043f341
0043f341  mov        eax, dword ptr [0x4b44f8]
0043f346  test       eax, eax
0043f348  je         0x43f3ac

// block 0043f34a
0043f34a  push       esi
0043f34b  push       edi
0043f34c  mov        edi, 0xb4
0043f351  cmp        dword ptr [eax + 0x4f4], edi
0043f357  jge        0x43f37e

// block 0043f359
0043f359  mov        esi, dword ptr [0x498084]
0043f35f  nop        

// block 0043f360
0043f360  test       dword ptr [0x4cee78], 0x180
0043f36a  jne        0x43f37e

// block 0043f36c
0043f36c  push       0x10
0043f36e  call       esi

// block 0043f370
0043f370  mov        ecx, dword ptr [0x4b44f8]
0043f376  cmp        dword ptr [ecx + 0x4f4], edi
0043f37c  jl         0x43f360

// block 0043f37e
0043f37e  mov        esi, dword ptr [0x4ce8cc]
0043f384  mov        edi, dword ptr [esi + 0x4b50c4]
0043f38a  add        esi, 0x4b50c4
0043f390  test       edi, edi
0043f392  je         0x43f3aa

// block 0043f394
0043f394  call       0x4604e0

// block 0043f399
0043f399  mov        edx, dword ptr [esi]
0043f39b  push       edx
0043f39c  call       0x46ca4f

// block 0043f3a1
0043f3a1  add        esp, 4
0043f3a4  mov        dword ptr [esi], 0

// block 0043f3aa
0043f3aa  pop        edi
0043f3ab  pop        esi

// block 0043f3ac
0043f3ac  mov        eax, dword ptr [0x4b4530]
0043f3b1  mov        eax, dword ptr [eax + 0xc]
0043f3b4  or         dword ptr [eax + 4], 2
0043f3b8  mov        dword ptr [0x4cf468], 1
0043f3c2  xor        eax, eax
0043f3c4  ret        
