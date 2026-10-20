
// block 00422770
00422770  push       esi
00422771  mov        esi, dword ptr [0x4b44e8]
00422777  mov        dword ptr [0x4cf468], 0
00422781  test       esi, esi
00422783  je         0x422794

// block 00422785
00422785  push       esi
00422786  call       0x422290

// block 0042278b
0042278b  push       esi
0042278c  call       0x46ca4f

// block 00422791
00422791  add        esp, 4

// block 00422794
00422794  pop        esi
00422795  ret        
