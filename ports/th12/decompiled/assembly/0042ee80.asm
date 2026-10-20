
// block 0042ee80
0042ee80  push       esi
0042ee81  mov        esi, dword ptr [0x4b44f8]
0042ee87  test       esi, esi
0042ee89  je         0x42ee9a

// block 0042ee8b
0042ee8b  push       esi
0042ee8c  call       0x42ec00

// block 0042ee91
0042ee91  push       esi
0042ee92  call       0x46ca4f

// block 0042ee97
0042ee97  add        esp, 4

// block 0042ee9a
0042ee9a  pop        esi
0042ee9b  ret        
