
// block 004303c0
004303c0  push       esi
004303c1  mov        esi, dword ptr [0x4ce8cc]
004303c7  call       0x45a3c0

// block 004303cc
004303cc  mov        eax, dword ptr [esp + 8]
004303d0  mov        eax, dword ptr [eax + 8]
004303d3  mov        ecx, dword ptr [eax]
004303d5  pop        esi
004303d6  mov        dword ptr [esp + 4], eax
004303da  mov        eax, dword ptr [ecx + 0xe4]
004303e0  jmp        eax
