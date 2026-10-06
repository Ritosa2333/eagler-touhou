
// block 00432020
00432020  push       esi
00432021  mov        esi, dword ptr [0x4b4510]
00432027  test       esi, esi
00432029  je         0x43203b

// block 0043202b
0043202b  mov        eax, esi
0043202d  call       0x431ed0

// block 00432032
00432032  push       esi
00432033  call       0x46ca4f

// block 00432038
00432038  add        esp, 4

// block 0043203b
0043203b  pop        esi
0043203c  ret        
