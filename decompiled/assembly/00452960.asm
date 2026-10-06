
// block 00452960
00452960  push       esi
00452961  mov        esi, ecx
00452963  test       esi, esi
00452965  je         0x452977

// block 00452967
00452967  mov        eax, esi
00452969  call       0x452f00

// block 0045296e
0045296e  push       esi
0045296f  call       0x46ca4f

// block 00452974
00452974  add        esp, 4

// block 00452977
00452977  xor        eax, eax
00452979  pop        esi
0045297a  ret        
