
// block 00454207
00454207  mov        eax, dword ptr [ebp + 4]
0045420a  push       eax
0045420b  push       0x4a3110
00454210  call       0x454b00

// block 00454215
00454215  fild       dword ptr [ebp + 4]
00454218  mov        esi, dword ptr [0x4d4754]
0045421e  add        esp, 8
00454221  fstp       dword ptr [esp + 0x14]
00454225  cmp        esi, ebx
00454227  je         0x454282

// block 00454229
00454229  fld        dword ptr [esp + 0x14]
0045422d  mov        dword ptr [esi + 0x1c], 1
00454234  fmul       qword ptr [0x4a3d20]
0045423a  call       0x4931e0

// block 0045423f
0045423f  mov        dword ptr [esi + 0x14], eax
00454242  mov        dword ptr [esi + 0x18], eax
00454245  jmp        0x454282
