
// block 00409670
00409670  push       ebx
00409671  mov        ebx, 0x49f69c
00409676  mov        ecx, 6
0040967b  call       0x45fe60

// block 00409680
00409680  pop        ebx
00409681  test       eax, eax
00409683  jne        0x40969b

// block 00409685
00409685  push       0x49f6a8
0040968a  mov        ecx, 0x4b0ec8
0040968f  call       0x464220

// block 00409694
00409694  add        esp, 4
00409697  or         eax, 0xffffffff
0040969a  ret        

// block 0040969b
0040969b  xor        eax, eax
0040969d  ret        
