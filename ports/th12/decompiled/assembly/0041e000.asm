
// block 0041e000
0041e000  push       esi
0041e001  mov        esi, dword ptr [0x4b43e4]
0041e007  test       esi, esi
0041e009  je         0x41e01a

// block 0041e00b
0041e00b  push       esi
0041e00c  call       0x41dc50

// block 0041e011
0041e011  push       esi
0041e012  call       0x46ca4f

// block 0041e017
0041e017  add        esp, 4

// block 0041e01a
0041e01a  pop        esi
0041e01b  ret        
