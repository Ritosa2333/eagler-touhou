
// block 0044f200
0044f200  push       -1
0044f202  push       0x496e93
0044f207  mov        eax, dword ptr fs:[0]
0044f20d  push       eax
0044f20e  push       ebx
0044f20f  push       esi
0044f210  mov        eax, dword ptr [0x4ad138]
0044f215  xor        eax, esp
0044f217  push       eax
0044f218  lea        eax, [esp + 0xc]
0044f21c  mov        dword ptr fs:[0], eax
0044f222  mov        ebx, dword ptr [esp + 0x1c]
0044f226  mov        esi, 0x4cf0d8
0044f22b  mov        dword ptr [esp + 0x14], 1
0044f233  call       0x464c40

// block 0044f238
0044f238  mov        dword ptr [ebx + 0x48], 1
0044f23f  call       0x4624c0
