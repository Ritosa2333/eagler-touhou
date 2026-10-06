
// block 004258d0
004258d0  push       esi
004258d1  mov        esi, ecx
004258d3  call       0x4027e0

// block 004258d8
004258d8  lea        ecx, [esi + 0x4b4]
004258de  call       0x4027e0

// block 004258e3
004258e3  mov        eax, 0xfffffffe
004258e8  and        dword ptr [esi + 0x998], eax
004258ee  and        dword ptr [esi + 0x9ac], eax
004258f4  mov        eax, esi
004258f6  pop        esi
004258f7  ret        
