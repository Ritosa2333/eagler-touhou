
// block 00494252
00494252  fstp       xword ptr [ebp - 0x9e]
00494258  fld        xword ptr [ebp - 0x9e]
0049425e  test       byte ptr [ebp - 0x97], 0x40
00494265  je         0x49426f

// block 00494267
00494267  mov        byte ptr [ebp - 0x90], 7
0049426e  ret        

// block 0049426f
0049426f  mov        byte ptr [ebp - 0x90], 1
00494276  fadd       qword ptr [0x4b35d4]
0049427c  ret        
