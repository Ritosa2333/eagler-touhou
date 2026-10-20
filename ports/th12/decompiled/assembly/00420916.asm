
// block 00420916
00420916  mov        edx, dword ptr [0x4b43dc]
0042091c  mov        eax, dword ptr [edx + 0x48]
0042091f  push       0
00420921  push       0x15
00420923  lea        ecx, [esp + 0x34]
00420927  push       ecx
00420928  push       eax
00420929  mov        eax, 0x17
0042092e  xor        ecx, ecx
00420930  call       0x4615a0

// block 00420935
00420935  mov        ecx, dword ptr [esp + 0x2c]
00420939  mov        dword ptr [ebp + 0x5c], ecx
0042093c  jmp        0x420ced
