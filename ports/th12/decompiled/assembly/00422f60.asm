
// block 00422f60
00422f60  mov        dword ptr [eax + 0x60], 2
00422f67  mov        eax, dword ptr [0x4b43e4]
00422f6c  test       eax, eax
00422f6e  je         0x422f84

// block 00422f70
00422f70  mov        ecx, dword ptr [0x4b0ca4]
00422f76  mov        edx, dword ptr [0x4b0ca0]
00422f7c  push       ecx
00422f7d  push       edx
00422f7e  push       eax
00422f7f  call       0x41cf40

// block 00422f84
00422f84  ret        
