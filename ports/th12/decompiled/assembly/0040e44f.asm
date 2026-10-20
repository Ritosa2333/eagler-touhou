
// block 0040e44f
0040e44f  push       ebp
0040e450  push       0xb

// block 0040e452
0040e452  mov        edx, dword ptr [0x4b43dc]
0040e458  mov        eax, dword ptr [edx + 0x48]
0040e45b  lea        ecx, [esp + 0x24]
0040e45f  push       ecx
0040e460  push       eax
0040e461  mov        eax, 0x17
0040e466  xor        ecx, ecx
0040e468  call       0x4615a0

// block 0040e46d
0040e46d  push       ebp
0040e46e  push       0x12
0040e470  jmp        0x40e58f

// block 0040e58f
0040e58f  mov        ecx, dword ptr [esp + 0x24]
0040e593  mov        eax, dword ptr [0x4b43dc]
0040e598  lea        edx, [esp + 0x24]
0040e59c  mov        dword ptr [edi + 0x10], ecx
0040e59f  mov        ecx, dword ptr [eax + 0x48]
0040e5a2  push       edx
0040e5a3  push       ecx
