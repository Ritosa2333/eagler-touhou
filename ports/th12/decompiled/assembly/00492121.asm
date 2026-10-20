
// block 00492121
00492121  call       0x475467

// block 00492126
00492126  cmp        dword ptr [eax + 0x90], 0
0049212d  jle        0x49213b

// block 0049212f
0049212f  call       0x475467

// block 00492134
00492134  add        eax, 0x90
00492139  dec        dword ptr [eax]

// block 0049213b
0049213b  ret        
