
// block 0044f2c0
0044f2c0  cmp        dword ptr [0x4b2ec8], 0x4b0ec8
0044f2ca  je         0x44f321

// block 0044f2cc
0044f2cc  push       0x4a247c
0044f2d1  mov        ecx, 0x4b0ec8
0044f2d6  call       0x464220

// block 0044f2db
0044f2db  add        esp, 4
0044f2de  cmp        byte ptr [0x4b2ecc], 0
0044f2e5  je         0x44f2fb

// block 0044f2e7
0044f2e7  push       0x10
0044f2e9  push       0x4a24bc
0044f2ee  push       0x4b0ec8
0044f2f3  push       0
0044f2f5  call       dword ptr [0x49824c]

// block 0044f2fb
0044f2fb  mov        eax, 0x4b0ec8
0044f300  lea        edx, [eax + 1]

// block 0044f303
0044f303  mov        cl, byte ptr [eax]
0044f305  inc        eax
0044f306  test       cl, cl
0044f308  jne        0x44f303

// block 0044f30a
0044f30a  push       ebx
0044f30b  push       edi
0044f30c  sub        eax, edx
0044f30e  push       0x4b0ec8
0044f313  mov        ebx, eax
0044f315  mov        edi, 0x4a24c0
0044f31a  call       0x463e80

// block 0044f31f
0044f31f  pop        edi
0044f320  pop        ebx

// block 0044f321
0044f321  ret        
