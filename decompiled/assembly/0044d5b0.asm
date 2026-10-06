
// block 0044d5b0
0044d5b0  mov        eax, dword ptr [0x4b0e5c]
0044d5b5  push       esi
0044d5b6  xor        esi, esi
0044d5b8  cmp        eax, esi
0044d5ba  je         0x44d615

// block 0044d5bc
0044d5bc  mov        ecx, dword ptr [0x4b0e60]
0044d5c2  push       ecx
0044d5c3  push       eax
0044d5c4  call       dword ptr [0x49802c]

// block 0044d5ca
0044d5ca  mov        edx, dword ptr [0x4b0e5c]
0044d5d0  push       edx
0044d5d1  call       dword ptr [0x498028]

// block 0044d5d7
0044d5d7  mov        eax, dword ptr [0x4b0e64]
0044d5dc  push       eax
0044d5dd  call       dword ptr [0x498030]

// block 0044d5e3
0044d5e3  mov        dword ptr [0x4b0e4c], esi
0044d5e9  mov        dword ptr [0x4b0e50], esi
0044d5ef  mov        dword ptr [0x4b0e5c], esi
0044d5f5  mov        dword ptr [0x4b0e64], esi
0044d5fb  mov        dword ptr [0x4b0e60], esi
0044d601  mov        dword ptr [0x4b0e68], esi
0044d607  mov        dword ptr [0x4b0e48], 0xffffffff
0044d611  mov        al, 1
0044d613  pop        esi
0044d614  ret        

// block 0044d615
0044d615  xor        al, al
0044d617  pop        esi
0044d618  ret        
