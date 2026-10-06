
// block 00422dd0
00422dd0  inc        dword ptr [eax + 0x60]
00422dd3  mov        ecx, dword ptr [eax + 0x60]
00422dd6  cmp        ecx, 9
00422dd9  jle        0x422de4

// block 00422ddb
00422ddb  mov        dword ptr [eax + 0x60], 9
00422de2  jmp        0x422dee

// block 00422de4
00422de4  mov        edx, 0x30
00422de9  call       0x453d90

// block 00422dee
00422dee  mov        eax, dword ptr [0x4b0ca4]
00422df3  mov        ecx, dword ptr [0x4b0ca0]
00422df9  mov        edx, dword ptr [0x4b43e4]
00422dff  push       eax
00422e00  push       ecx
00422e01  push       edx
00422e02  call       0x41cf40

// block 00422e07
00422e07  ret        
