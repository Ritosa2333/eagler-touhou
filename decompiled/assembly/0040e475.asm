
// block 0040e475
0040e475  mov        eax, dword ptr [0x4b43dc]
0040e47a  mov        ecx, dword ptr [eax + 0x48]
0040e47d  push       ebp
0040e47e  push       0x14
0040e480  lea        edx, [esp + 0x24]
0040e484  push       edx
0040e485  push       ecx
0040e486  mov        eax, 0x17
0040e48b  xor        ecx, ecx
0040e48d  call       0x4615a0

// block 0040e492
0040e492  mov        edx, dword ptr [esp + 0x1c]
0040e496  mov        ecx, dword ptr [0x4b43dc]
0040e49c  push       ebp
0040e49d  push       0x1b
0040e49f  lea        eax, [esp + 0x24]
0040e4a3  mov        dword ptr [edi + 0x10], edx
0040e4a6  mov        edx, dword ptr [ecx + 0x48]
0040e4a9  push       eax
0040e4aa  push       edx
0040e4ab  jmp        0x40e5a4
