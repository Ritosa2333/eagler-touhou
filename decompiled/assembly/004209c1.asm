
// block 004209c1
004209c1  mov        eax, dword ptr [0x4b43dc]
004209c6  mov        ecx, dword ptr [eax + 0x48]
004209c9  push       0
004209cb  push       0x10
004209cd  lea        edx, [esp + 0x44]
004209d1  push       edx
004209d2  push       ecx
004209d3  mov        eax, 0x17
004209d8  xor        ecx, ecx
004209da  call       0x4615a0

// block 004209df
004209df  mov        edx, dword ptr [esp + 0x3c]
004209e3  mov        dword ptr [ebp + 0x5c], edx
004209e6  jmp        0x420ced
