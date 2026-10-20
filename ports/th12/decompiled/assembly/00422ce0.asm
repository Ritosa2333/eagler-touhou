
// block 00422ce0
00422ce0  inc        dword ptr [eax + 0x58]
00422ce3  mov        ecx, dword ptr [eax + 0x58]
00422ce6  cmp        ecx, 8
00422ce9  push       esi
00422cea  jle        0x422cf5

// block 00422cec
00422cec  mov        dword ptr [eax + 0x58], 8
00422cf3  jmp        0x422d11

// block 00422cf5
00422cf5  mov        edx, 0x12
00422cfa  call       0x453d90

// block 00422cff
00422cff  mov        esi, dword ptr [0x4b43e4]
00422d05  push       0
00422d07  mov        eax, 4
00422d0c  call       0x420f90

// block 00422d11
00422d11  mov        eax, dword ptr [0x4b0c9c]
00422d16  mov        ecx, dword ptr [0x4b0c98]
00422d1c  mov        edx, dword ptr [0x4b43e4]
00422d22  push       eax
00422d23  push       ecx
00422d24  push       edx
00422d25  call       0x41ce60

// block 00422d2a
00422d2a  pop        esi
00422d2b  ret        
