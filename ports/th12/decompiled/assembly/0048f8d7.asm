
// block 0048f8d7
0048f8d7  mov        edi, edi
0048f8d9  push       ebp
0048f8da  mov        ebp, esp
0048f8dc  mov        cl, byte ptr [ebp + 8]
0048f8df  xor        eax, eax
0048f8e1  test       cl, 0x3f
0048f8e4  je         0x48f918

// block 0048f8e6
0048f8e6  test       cl, 1
0048f8e9  je         0x48f8ee

// block 0048f8eb
0048f8eb  push       0x10
0048f8ed  pop        eax

// block 0048f8ee
0048f8ee  test       cl, 4
0048f8f1  je         0x48f8f6

// block 0048f8f3
0048f8f3  or         eax, 8

// block 0048f8f6
0048f8f6  test       cl, 8
0048f8f9  je         0x48f8fe

// block 0048f8fb
0048f8fb  or         eax, 4

// block 0048f8fe
0048f8fe  test       cl, 0x10
0048f901  je         0x48f906

// block 0048f903
0048f903  or         eax, 2

// block 0048f906
0048f906  test       cl, 0x20
0048f909  je         0x48f90e

// block 0048f90b
0048f90b  or         eax, 1

// block 0048f90e
0048f90e  test       cl, 2
0048f911  je         0x48f918

// block 0048f913
0048f913  or         eax, 0x80000

// block 0048f918
0048f918  pop        ebp
0048f919  ret        
