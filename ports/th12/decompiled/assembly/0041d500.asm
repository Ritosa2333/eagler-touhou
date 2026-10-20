
// block 0041d500
0041d500  push       ebx
0041d501  mov        ebx, 0x49fce4
0041d506  mov        ecx, 5
0041d50b  call       0x45fe60

// block 0041d510
0041d510  pop        ebx
0041d511  test       eax, eax
0041d513  jne        0x41d52b

// block 0041d515
0041d515  push       0x49f434
0041d51a  mov        ecx, 0x4b0ec8
0041d51f  call       0x464220

// block 0041d524
0041d524  add        esp, 4
0041d527  or         eax, 0xffffffff
0041d52a  ret        

// block 0041d52b
0041d52b  xor        eax, eax
0041d52d  ret        
