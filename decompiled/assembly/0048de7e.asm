
// block 0048de7e
0048de7e  mov        edi, edi
0048de80  push       ebp
0048de81  mov        ebp, esp
0048de83  mov        eax, dword ptr [ebp + 8]
0048de86  push       esi
0048de87  mov        esi, dword ptr [0x4b43ac]
0048de8d  test       eax, 0xfffffffe
0048de92  je         0x48deb0

// block 0048de94
0048de94  call       0x46e96f

// block 0048de99
0048de99  mov        dword ptr [eax], 0x16
0048de9f  xor        eax, eax
0048dea1  push       eax
0048dea2  push       eax
0048dea3  push       eax
0048dea4  push       eax
0048dea5  push       eax
0048dea6  call       0x470f2d

// block 0048deab
0048deab  add        esp, 0x14
0048deae  jmp        0x48deb5

// block 0048deb0
0048deb0  mov        dword ptr [0x4b43ac], eax

// block 0048deb5
0048deb5  mov        eax, esi
0048deb7  pop        esi
0048deb8  pop        ebp
0048deb9  ret        
