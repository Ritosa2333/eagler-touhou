
// block 00464c40
00464c40  mov        eax, dword ptr [esi + 4]
00464c43  push       ebp
00464c44  xor        ebp, ebp
00464c46  cmp        eax, ebp
00464c48  je         0x464ca3

// block 00464c4a
00464c4a  push       edi
00464c4b  mov        edi, dword ptr [0x4980f4]
00464c51  push       0xc8
00464c56  push       eax
00464c57  mov        dword ptr [esi + 0xc], 1
00464c5e  mov        dword ptr [esi + 0x10], ebp
00464c61  call       edi

// block 00464c63
00464c63  cmp        eax, 0x102
00464c68  jne        0x464c92

// block 00464c6a
00464c6a  push       ebx
00464c6b  mov        ebx, dword ptr [0x498084]

// block 00464c71
00464c71  push       1
00464c73  mov        dword ptr [esi + 0xc], 1
00464c7a  mov        dword ptr [esi + 0x10], ebp
00464c7d  call       ebx

// block 00464c7f
00464c7f  mov        eax, dword ptr [esi + 4]
00464c82  push       0xc8
00464c87  push       eax
00464c88  call       edi

// block 00464c8a
00464c8a  cmp        eax, 0x102
00464c8f  je         0x464c71

// block 00464c91
00464c91  pop        ebx

// block 00464c92
00464c92  mov        ecx, dword ptr [esi + 4]
00464c95  push       ecx
00464c96  call       dword ptr [0x4980a4]

// block 00464c9c
00464c9c  mov        dword ptr [esi + 4], ebp
00464c9f  mov        dword ptr [esi + 0x18], ebp
00464ca2  pop        edi

// block 00464ca3
00464ca3  pop        ebp
00464ca4  ret        
