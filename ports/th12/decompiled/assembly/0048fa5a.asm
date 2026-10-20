
// block 0048fa5a
0048fa5a  mov        edi, edi
0048fa5c  push       ebp
0048fa5d  mov        ebp, esp
0048fa5f  mov        cl, byte ptr [ebp + 8]
0048fa62  xor        eax, eax
0048fa64  test       cl, 0x3f
0048fa67  je         0x48fa9b

// block 0048fa69
0048fa69  test       cl, 1
0048fa6c  je         0x48fa71

// block 0048fa6e
0048fa6e  push       0x10
0048fa70  pop        eax

// block 0048fa71
0048fa71  test       cl, 4
0048fa74  je         0x48fa79

// block 0048fa76
0048fa76  or         eax, 8

// block 0048fa79
0048fa79  test       cl, 8
0048fa7c  je         0x48fa81

// block 0048fa7e
0048fa7e  or         eax, 4

// block 0048fa81
0048fa81  test       cl, 0x10
0048fa84  je         0x48fa89

// block 0048fa86
0048fa86  or         eax, 2

// block 0048fa89
0048fa89  test       cl, 0x20
0048fa8c  je         0x48fa91

// block 0048fa8e
0048fa8e  or         eax, 1

// block 0048fa91
0048fa91  test       cl, 2
0048fa94  je         0x48fa9b

// block 0048fa96
0048fa96  or         eax, 0x80000

// block 0048fa9b
0048fa9b  pop        ebp
0048fa9c  ret        
