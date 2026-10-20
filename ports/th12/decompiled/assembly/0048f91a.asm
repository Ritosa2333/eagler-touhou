
// block 0048f91a
0048f91a  xor        eax, eax
0048f91c  test       dl, dl
0048f91e  jns        0x48f923

// block 0048f920
0048f920  push       0x10
0048f922  pop        eax

// block 0048f923
0048f923  push       ebx
0048f924  mov        ebx, 0x200
0048f929  push       esi
0048f92a  push       edi
0048f92b  test       ebx, edx
0048f92d  je         0x48f932

// block 0048f92f
0048f92f  or         eax, 8

// block 0048f932
0048f932  test       edx, 0x400
0048f938  je         0x48f93d

// block 0048f93a
0048f93a  or         eax, 4

// block 0048f93d
0048f93d  test       edx, 0x800
0048f943  je         0x48f948

// block 0048f945
0048f945  or         eax, 2

// block 0048f948
0048f948  test       edx, 0x1000
0048f94e  je         0x48f953

// block 0048f950
0048f950  or         eax, 1

// block 0048f953
0048f953  mov        edi, 0x100
0048f958  test       edi, edx
0048f95a  je         0x48f961

// block 0048f95c
0048f95c  or         eax, 0x80000

// block 0048f961
0048f961  mov        ecx, edx
0048f963  mov        esi, 0x6000
0048f968  and        ecx, esi
0048f96a  je         0x48f98d

// block 0048f96c
0048f96c  cmp        ecx, 0x2000
0048f972  je         0x48f98b

// block 0048f974
0048f974  cmp        ecx, 0x4000
0048f97a  je         0x48f987

// block 0048f97c
0048f97c  cmp        ecx, esi
0048f97e  jne        0x48f98d

// block 0048f980
0048f980  or         eax, 0x300
0048f985  jmp        0x48f98d

// block 0048f987
0048f987  or         eax, ebx
0048f989  jmp        0x48f98d

// block 0048f98b
0048f98b  or         eax, edi

// block 0048f98d
0048f98d  pop        edi
0048f98e  and        edx, 0x8040
0048f994  sub        edx, 0x40
0048f997  pop        esi
0048f998  pop        ebx
0048f999  je         0x48f9b4

// block 0048f99b
0048f99b  sub        edx, 0x7fc0
0048f9a1  je         0x48f9ae

// block 0048f9a3
0048f9a3  sub        edx, 0x40
0048f9a6  jne        0x48f9b9

// block 0048f9a8
0048f9a8  or         eax, 0x1000000
0048f9ad  ret        

// block 0048f9ae
0048f9ae  or         eax, 0x3000000
0048f9b3  ret        

// block 0048f9b4
0048f9b4  or         eax, 0x2000000

// block 0048f9b9
0048f9b9  ret        
