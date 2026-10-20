
// block 004094b0
004094b0  push       esi
004094b1  mov        esi, ecx
004094b3  lea        ecx, [esi + 8]
004094b6  call       0x4027e0

// block 004094bb
004094bb  mov        edx, 0xfffffffe
004094c0  and        dword ptr [esi + 0x4f4], edx
004094c6  and        dword ptr [esi + 0x508], edx
004094cc  mov        ecx, 0xc
004094d1  lea        eax, [esi + 0x710]

// block 004094d7
004094d7  and        dword ptr [eax], edx
004094d9  add        eax, 0x34
004094dc  sub        ecx, 1
004094df  jns        0x4094d7

// block 004094e1
004094e1  and        dword ptr [esi + 0x9e8], edx
004094e7  mov        eax, esi
004094e9  pop        esi
004094ea  ret        
