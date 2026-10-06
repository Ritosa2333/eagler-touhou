
// block 004848bc
004848bc  call       0x475467

// block 004848c1
004848c1  mov        ecx, eax
004848c3  mov        eax, dword ptr [ecx + 0x6c]
004848c6  cmp        eax, dword ptr [0x4adab8]
004848cc  je         0x4848de

// block 004848ce
004848ce  mov        edx, dword ptr [0x4ad9d4]
004848d4  test       dword ptr [ecx + 0x70], edx
004848d7  jne        0x4848de

// block 004848d9
004848d9  call       0x474181

// block 004848de
004848de  mov        eax, dword ptr [eax + 8]
004848e1  ret        
