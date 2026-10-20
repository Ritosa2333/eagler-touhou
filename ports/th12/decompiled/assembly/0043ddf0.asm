
// block 0043ddf0
0043ddf0  push       esi
0043ddf1  mov        esi, dword ptr [0x4b4524]
0043ddf7  test       esi, esi
0043ddf9  je         0x43de0a

// block 0043ddfb
0043ddfb  push       esi
0043ddfc  call       0x43dc30

// block 0043de01
0043de01  push       esi
0043de02  call       0x46ca4f

// block 0043de07
0043de07  add        esp, 4

// block 0043de0a
0043de0a  pop        esi
0043de0b  ret        
