
// block 004208eb
004208eb  mov        ecx, dword ptr [0x4b43dc]
004208f1  mov        edx, dword ptr [ecx + 0x48]
004208f4  push       0
004208f6  push       0x11
004208f8  lea        eax, [esp + 0x30]
004208fc  push       eax
004208fd  push       edx
004208fe  mov        eax, 0x17
00420903  xor        ecx, ecx
00420905  call       0x4615a0

// block 0042090a
0042090a  mov        eax, dword ptr [esp + 0x28]
0042090e  mov        dword ptr [ebp + 0x5c], eax
00420911  jmp        0x420ced
