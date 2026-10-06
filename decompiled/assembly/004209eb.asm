
// block 004209eb
004209eb  mov        ecx, dword ptr [0x4b43dc]
004209f1  mov        edx, dword ptr [ecx + 0x48]
004209f4  push       0
004209f6  push       0x10
004209f8  lea        eax, [esp + 0x48]
004209fc  push       eax
004209fd  push       edx
004209fe  mov        eax, 0x17
00420a03  xor        ecx, ecx
00420a05  call       0x4615a0

// block 00420a0a
00420a0a  mov        eax, dword ptr [esp + 0x40]
00420a0e  mov        dword ptr [ebp + 0x5c], eax
00420a11  jmp        0x420ced
