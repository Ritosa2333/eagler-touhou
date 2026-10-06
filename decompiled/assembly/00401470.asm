
// block 00401470
00401470  push       esi
00401471  mov        esi, dword ptr [0x4b43b8]
00401477  test       esi, esi
00401479  je         0x40148a

// block 0040147b
0040147b  push       esi
0040147c  call       0x401220

// block 00401481
00401481  push       esi
00401482  call       0x46ca4f

// block 00401487
00401487  add        esp, 4

// block 0040148a
0040148a  pop        esi
0040148b  ret        
