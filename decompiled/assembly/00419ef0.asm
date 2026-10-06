
// block 00419ef0
00419ef0  mov        eax, dword ptr [esp + 4]
00419ef4  add        eax, 0x2701
00419ef9  cmp        eax, 0x2d
00419efc  ja         0x419f98

// block 00419f02
00419f02  movzx      eax, byte ptr [eax + 0x419fd0]
00419f09  jmp        dword ptr [eax*4 + 0x419fa0]

// block 00419f10
00419f10  lea        eax, [ecx + 0x127c]
00419f16  ret        4

// block 00419f19
00419f19  lea        eax, [ecx + 0x1280]
00419f1f  ret        4

// block 00419f22
00419f22  lea        eax, [ecx + 0x1284]
00419f28  ret        4

// block 00419f2b
00419f2b  lea        eax, [ecx + 0x1288]
00419f31  ret        4

// block 00419f34
00419f34  mov        eax, dword ptr [0x4b43dc]
00419f39  add        eax, 0x10
00419f3c  ret        4

// block 00419f3f
00419f3f  mov        eax, dword ptr [0x4b43dc]
00419f44  add        eax, 0x14
00419f47  ret        4

// block 00419f4a
00419f4a  mov        eax, dword ptr [0x4b43dc]
00419f4f  add        eax, 0x18
00419f52  ret        4

// block 00419f55
00419f55  mov        ecx, dword ptr [0x4b43dc]
00419f5b  mov        eax, dword ptr [ecx + 0x1c]
00419f5e  add        eax, 0x127c
00419f63  ret        4

// block 00419f66
00419f66  mov        edx, dword ptr [0x4b43dc]
00419f6c  mov        eax, dword ptr [edx + 0x1c]
00419f6f  add        eax, 0x1280
00419f74  ret        4

// block 00419f77
00419f77  mov        eax, dword ptr [0x4b43dc]
00419f7c  mov        eax, dword ptr [eax + 0x1c]
00419f7f  add        eax, 0x1284
00419f84  ret        4

// block 00419f87
00419f87  mov        ecx, dword ptr [0x4b43dc]
00419f8d  mov        eax, dword ptr [ecx + 0x1c]
00419f90  add        eax, 0x1288
00419f95  ret        4

// block 00419f98
00419f98  xor        eax, eax
00419f9a  ret        4
