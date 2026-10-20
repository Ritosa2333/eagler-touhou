
// block 00422e10
00422e10  mov        ecx, dword ptr [eax + 0x60]
00422e13  cmp        ecx, 9
00422e16  jl         0x422e20

// block 00422e18
00422e18  mov        dword ptr [eax + 0x64], 0
00422e1f  ret        

// block 00422e20
00422e20  add        dword ptr [eax + 0x64], 2
00422e24  cmp        dword ptr [eax + 0x64], 5
00422e28  jl         0x422e66

// block 00422e2a
00422e2a  inc        ecx
00422e2b  cmp        ecx, 9
00422e2e  mov        dword ptr [eax + 0x64], 0
00422e35  mov        dword ptr [eax + 0x60], ecx
00422e38  jle        0x422e43

// block 00422e3a
00422e3a  mov        dword ptr [eax + 0x60], 9
00422e41  jmp        0x422e4d

// block 00422e43
00422e43  mov        edx, 0x30
00422e48  call       0x453d90

// block 00422e4d
00422e4d  mov        eax, dword ptr [0x4b0ca4]
00422e52  mov        ecx, dword ptr [0x4b0ca0]
00422e58  mov        edx, dword ptr [0x4b43e4]
00422e5e  push       eax
00422e5f  push       ecx
00422e60  push       edx
00422e61  call       0x41cf40

// block 00422e66
00422e66  mov        eax, dword ptr [0x4b0ca4]
00422e6b  mov        ecx, dword ptr [0x4b0ca0]
00422e71  mov        edx, dword ptr [0x4b43e4]
00422e77  push       eax
00422e78  push       ecx
00422e79  push       edx
00422e7a  call       0x41cf40

// block 00422e7f
00422e7f  ret        
