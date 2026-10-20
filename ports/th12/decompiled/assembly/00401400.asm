
// block 00401400
00401400  push       esi
00401401  push       0x18fc4
00401406  call       0x46c9ea

// block 0040140b
0040140b  add        esp, 4
0040140e  test       eax, eax
00401410  je         0x40141d

// block 00401412
00401412  mov        esi, eax
00401414  call       0x401000

// block 00401419
00401419  mov        esi, eax
0040141b  jmp        0x40141f

// block 0040141d
0040141d  xor        esi, esi

// block 0040141f
0040141f  mov        eax, esi
00401421  call       0x401090

// block 00401426
00401426  test       eax, eax
00401428  je         0x401441

// block 0040142a
0040142a  test       esi, esi
0040142c  je         0x40143d

// block 0040142e
0040142e  push       esi
0040142f  call       0x401220

// block 00401434
00401434  push       esi
00401435  call       0x46ca4f

// block 0040143a
0040143a  add        esp, 4

// block 0040143d
0040143d  xor        eax, eax
0040143f  pop        esi
00401440  ret        

// block 00401441
00401441  mov        eax, esi
00401443  pop        esi
00401444  ret        
