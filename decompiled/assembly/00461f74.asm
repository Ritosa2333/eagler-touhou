
// block 00461f74
00461f74  sub        esp, 0x2c
00461f77  lea        edx, [esp]
00461f7a  push       edx
00461f7b  push       0
00461f7d  push       eax
00461f7e  mov        eax, dword ptr [ecx + 0x48]
00461f81  call       eax

// block 00461f83
00461f83  mov        eax, dword ptr [esp]
00461f86  mov        ecx, dword ptr [eax]
00461f88  lea        edx, [esp + 0xc]
00461f8c  push       edx
00461f8d  push       eax
00461f8e  mov        eax, dword ptr [ecx + 0x30]
00461f91  call       eax

// block 00461f93
00461f93  mov        eax, dword ptr [esp]
00461f96  mov        ecx, dword ptr [eax]
00461f98  push       0
00461f9a  push       0
00461f9c  lea        edx, [esp + 0xc]
00461fa0  push       edx
00461fa1  push       eax
00461fa2  mov        eax, dword ptr [ecx + 0x34]
00461fa5  call       eax

// block 00461fa7
00461fa7  mov        ecx, dword ptr [esp + 0x28]
00461fab  imul       ecx, dword ptr [esp + 4]
00461fb0  mov        edx, dword ptr [esp + 8]
00461fb4  push       ecx
00461fb5  push       0
00461fb7  push       edx
00461fb8  call       0x477420

// block 00461fbd
00461fbd  mov        eax, dword ptr [esp + 0xc]
00461fc1  mov        ecx, dword ptr [eax]
00461fc3  mov        edx, dword ptr [ecx + 0x38]
00461fc6  add        esp, 0xc
00461fc9  push       eax
00461fca  call       edx

// block 00461fcc
00461fcc  mov        eax, dword ptr [esp]
00461fcf  test       eax, eax
00461fd1  je         0x461fdb

// block 00461fd3
00461fd3  mov        ecx, dword ptr [eax]
00461fd5  mov        edx, dword ptr [ecx + 8]
00461fd8  push       eax
00461fd9  call       edx

// block 00461fdb
00461fdb  add        esp, 0x2c
00461fde  ret        
