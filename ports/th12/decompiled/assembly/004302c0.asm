
// block 004302c0
004302c0  cmp        dword ptr [edi + 0x990], 1
004302c7  je         0x4302f3

// block 004302c9
004302c9  push       esi
004302ca  mov        esi, dword ptr [0x4ce8cc]
004302d0  call       0x45a3c0

// block 004302d5
004302d5  mov        eax, dword ptr [edi + 8]
004302d8  push       1
004302da  mov        dword ptr [edi + 0x990], 1
004302e4  mov        ecx, dword ptr [eax]
004302e6  mov        edx, dword ptr [ecx + 0xe4]
004302ec  push       0x1c
004302ee  push       eax
004302ef  call       edx

// block 004302f1
004302f1  pop        esi
004302f2  ret        

// block 004302f3
004302f3  xor        eax, eax
004302f5  ret        
