
// block 0048fb73
0048fb73  call       0x491202

// block 0048fb78
0048fb78  xor        ecx, ecx
0048fb7a  test       al, 0x3f
0048fb7c  je         0x48fbab

// block 0048fb7e
0048fb7e  test       al, 1
0048fb80  je         0x48fb85

// block 0048fb82
0048fb82  push       0x10
0048fb84  pop        ecx

// block 0048fb85
0048fb85  test       al, 4
0048fb87  je         0x48fb8c

// block 0048fb89
0048fb89  or         ecx, 8

// block 0048fb8c
0048fb8c  test       al, 8
0048fb8e  je         0x48fb93

// block 0048fb90
0048fb90  or         ecx, 4

// block 0048fb93
0048fb93  test       al, 0x10
0048fb95  je         0x48fb9a

// block 0048fb97
0048fb97  or         ecx, 2

// block 0048fb9a
0048fb9a  test       al, 0x20
0048fb9c  je         0x48fba1

// block 0048fb9e
0048fb9e  or         ecx, 1

// block 0048fba1
0048fba1  test       al, 2
0048fba3  je         0x48fbab

// block 0048fba5
0048fba5  or         ecx, 0x80000

// block 0048fbab
0048fbab  mov        eax, ecx
0048fbad  ret        
