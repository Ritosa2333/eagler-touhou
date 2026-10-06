
// block 00423750
00423750  push       edi
00423751  push       0x1a8
00423756  call       0x46c9ea

// block 0042375b
0042375b  add        esp, 4
0042375e  test       eax, eax
00423760  je         0x42376f

// block 00423762
00423762  push       esi
00423763  mov        esi, eax
00423765  call       0x423380

// block 0042376a
0042376a  mov        edi, eax
0042376c  pop        esi
0042376d  jmp        0x423771

// block 0042376f
0042376f  xor        edi, edi

// block 00423771
00423771  call       0x423480

// block 00423776
00423776  test       eax, eax
00423778  je         0x423791

// block 0042377a
0042377a  test       edi, edi
0042377c  je         0x42378d

// block 0042377e
0042377e  push       edi
0042377f  call       0x423580

// block 00423784
00423784  push       edi
00423785  call       0x46ca4f

// block 0042378a
0042378a  add        esp, 4

// block 0042378d
0042378d  xor        eax, eax
0042378f  pop        edi
00423790  ret        

// block 00423791
00423791  mov        eax, edi
00423793  pop        edi
00423794  ret        
