
// block 0048fdce
0048fdce  mov        edi, edi
0048fdd0  push       ebp
0048fdd1  mov        ebp, esp
0048fdd3  push       ecx
0048fdd4  push       ecx
0048fdd5  fnstsw     word ptr [ebp - 4]
0048fdd8  fnclex     
0048fdda  cmp        dword ptr [0x4d52dc], 0
0048fde1  je         0x48fe69

// block 0048fde7
0048fde7  mov        al, byte ptr [ebp - 4]
0048fdea  xor        edx, edx
0048fdec  push       esi
0048fded  mov        esi, 0x80000
0048fdf2  test       al, 0x3f
0048fdf4  je         0x48fe1f

// block 0048fdf6
0048fdf6  test       al, 1
0048fdf8  je         0x48fdfd

// block 0048fdfa
0048fdfa  push       0x10
0048fdfc  pop        edx

// block 0048fdfd
0048fdfd  test       al, 4
0048fdff  je         0x48fe04

// block 0048fe01
0048fe01  or         edx, 8

// block 0048fe04
0048fe04  test       al, 8
0048fe06  je         0x48fe0b

// block 0048fe08
0048fe08  or         edx, 4

// block 0048fe0b
0048fe0b  test       al, 0x10
0048fe0d  je         0x48fe12

// block 0048fe0f
0048fe0f  or         edx, 2

// block 0048fe12
0048fe12  test       al, 0x20
0048fe14  je         0x48fe19

// block 0048fe16
0048fe16  or         edx, 1

// block 0048fe19
0048fe19  test       al, 2
0048fe1b  je         0x48fe1f

// block 0048fe1d
0048fe1d  or         edx, esi

// block 0048fe1f
0048fe1f  stmxcsr    dword ptr [ebp - 8]
0048fe23  and        dword ptr [ebp - 8], 0xffffffc0
0048fe27  ldmxcsr    dword ptr [ebp - 8]
0048fe2b  mov        cl, byte ptr [ebp - 8]
0048fe2e  xor        eax, eax
0048fe30  test       cl, 0x3f
0048fe33  je         0x48fe64

// block 0048fe35
0048fe35  test       cl, 1
0048fe38  je         0x48fe3d

// block 0048fe3a
0048fe3a  push       0x10
0048fe3c  pop        eax

// block 0048fe3d
0048fe3d  test       cl, 4
0048fe40  je         0x48fe45

// block 0048fe42
0048fe42  or         eax, 8

// block 0048fe45
0048fe45  test       cl, 8
0048fe48  je         0x48fe4d

// block 0048fe4a
0048fe4a  or         eax, 4

// block 0048fe4d
0048fe4d  test       cl, 0x10
0048fe50  je         0x48fe55

// block 0048fe52
0048fe52  or         eax, 2

// block 0048fe55
0048fe55  test       cl, 0x20
0048fe58  je         0x48fe5d

// block 0048fe5a
0048fe5a  or         eax, 1

// block 0048fe5d
0048fe5d  test       cl, 2
0048fe60  je         0x48fe64

// block 0048fe62
0048fe62  or         eax, esi

// block 0048fe64
0048fe64  or         eax, edx
0048fe66  pop        esi
0048fe67  leave      
0048fe68  ret        

// block 0048fe69
0048fe69  mov        cl, byte ptr [ebp - 4]
0048fe6c  xor        eax, eax
0048fe6e  test       cl, 0x3f
0048fe71  je         0x48fea5

// block 0048fe73
0048fe73  test       cl, 1
0048fe76  je         0x48fe7b

// block 0048fe78
0048fe78  push       0x10
0048fe7a  pop        eax

// block 0048fe7b
0048fe7b  test       cl, 4
0048fe7e  je         0x48fe83

// block 0048fe80
0048fe80  or         eax, 8

// block 0048fe83
0048fe83  test       cl, 8
0048fe86  je         0x48fe8b

// block 0048fe88
0048fe88  or         eax, 4

// block 0048fe8b
0048fe8b  test       cl, 0x10
0048fe8e  je         0x48fe93

// block 0048fe90
0048fe90  or         eax, 2

// block 0048fe93
0048fe93  test       cl, 0x20
0048fe96  je         0x48fe9b

// block 0048fe98
0048fe98  or         eax, 1

// block 0048fe9b
0048fe9b  test       cl, 2
0048fe9e  je         0x48fea5

// block 0048fea0
0048fea0  or         eax, 0x80000

// block 0048fea5
0048fea5  leave      
0048fea6  ret        
