
// block 0042e8f0
0042e8f0  push       ebx
0042e8f1  mov        ebx, 0x49fce4
0042e8f6  mov        ecx, 5
0042e8fb  call       0x45fe60

// block 0042e900
0042e900  test       eax, eax
0042e902  jne        0x42e91b

// block 0042e904
0042e904  push       0x49f434
0042e909  mov        ecx, 0x4b0ec8
0042e90e  call       0x464220

// block 0042e913
0042e913  add        esp, 4
0042e916  or         eax, 0xffffffff
0042e919  pop        ebx
0042e91a  ret        

// block 0042e91b
0042e91b  mov        ebx, 0x49f69c
0042e920  mov        ecx, 6
0042e925  call       0x45fe60

// block 0042e92a
0042e92a  test       eax, eax
0042e92c  jne        0x42e945

// block 0042e92e
0042e92e  push       0x49f6a8
0042e933  mov        ecx, 0x4b0ec8
0042e938  call       0x464220

// block 0042e93d
0042e93d  add        esp, 4
0042e940  or         eax, 0xffffffff
0042e943  pop        ebx
0042e944  ret        

// block 0042e945
0042e945  xor        eax, eax
0042e947  pop        ebx
0042e948  ret        
