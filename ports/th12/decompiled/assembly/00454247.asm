
// block 00454247
00454247  cmp        byte ptr [0x4ceacb], 1
0045424e  jne        0x454282

// block 00454250
00454250  mov        eax, dword ptr [0x4d4754]
00454255  cmp        dword ptr [eax + 0x78], ebx
00454258  jne        0x4543a4

// block 0045425e
0045425e  call       0x4664f0

// block 00454263
00454263  jmp        0x454282
