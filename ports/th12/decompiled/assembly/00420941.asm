
// block 00420941
00420941  mov        eax, dword ptr [0x4b43dc]
00420946  mov        ecx, dword ptr [eax + 0x48]
00420949  push       0
0042094b  push       0xe
0042094d  lea        edx, [esp + 0x38]
00420951  push       edx
00420952  push       ecx
00420953  mov        eax, 0x17
00420958  xor        ecx, ecx
0042095a  call       0x4615a0

// block 0042095f
0042095f  mov        edx, dword ptr [esp + 0x30]
00420963  mov        dword ptr [ebp + 0x5c], edx
00420966  jmp        0x420ced
