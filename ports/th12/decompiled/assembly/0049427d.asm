
// block 0049427d
0049427d  fxch       st(1)
0049427f  fstp       xword ptr [ebp - 0x9e]
00494285  fld        xword ptr [ebp - 0x9e]
0049428b  test       byte ptr [ebp - 0x97], 0x40
00494292  je         0x49429d

// block 00494294
00494294  mov        byte ptr [ebp - 0x90], 7
0049429b  jmp        0x4942a4

// block 0049429d
0049429d  mov        byte ptr [ebp - 0x90], 1

// block 004942a4
004942a4  faddp      st(1)
004942a6  ret        
