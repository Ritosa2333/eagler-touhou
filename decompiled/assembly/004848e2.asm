
// block 004848e2
004848e2  call       0x475467

// block 004848e7
004848e7  mov        ecx, eax
004848e9  mov        eax, dword ptr [ecx + 0x6c]
004848ec  cmp        eax, dword ptr [0x4adab8]
004848f2  je         0x484904

// block 004848f4
004848f4  mov        edx, dword ptr [0x4ad9d4]
004848fa  test       dword ptr [ecx + 0x70], edx
004848fd  jne        0x484904

// block 004848ff
004848ff  call       0x474181

// block 00484904
00484904  add        eax, 0xc
00484907  ret        
