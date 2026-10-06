
// block 00484896
00484896  call       0x475467

// block 0048489b
0048489b  mov        ecx, eax
0048489d  mov        eax, dword ptr [ecx + 0x6c]
004848a0  cmp        eax, dword ptr [0x4adab8]
004848a6  je         0x4848b8

// block 004848a8
004848a8  mov        edx, dword ptr [0x4ad9d4]
004848ae  test       dword ptr [ecx + 0x70], edx
004848b1  jne        0x4848b8

// block 004848b3
004848b3  call       0x474181

// block 004848b8
004848b8  mov        eax, dword ptr [eax + 4]
004848bb  ret        
