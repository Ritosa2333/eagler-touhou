
// block 0042f000
0042f000  push       esi
0042f001  mov        esi, ecx
0042f003  mov        eax, dword ptr [esi + 0x590]
0042f009  and        eax, 0x180
0042f00e  cmp        eax, 0x80
0042f013  jne        0x42f028

// block 0042f015
0042f015  cmp        dword ptr [esi + 0x800], 0
0042f01c  jne        0x42f028

// block 0042f01e
0042f01e  mov        dword ptr [0x4cee40], 3

// block 0042f028
0042f028  call       0x453fa0

// block 0042f02d
0042f02d  call       0x4319f0

// block 0042f032
0042f032  call       0x462ec0

// block 0042f037
0042f037  call       0x4603d0

// block 0042f03c
0042f03c  test       eax, eax
0042f03e  je         0x42f047

// block 0042f040
0042f040  mov        eax, 4
0042f045  pop        esi
0042f046  ret        

// block 0042f047
0042f047  mov        eax, dword ptr [0x4cf42c]
0042f04c  test       eax, eax
0042f04e  je         0x42f05a

// block 0042f050
0042f050  dec        eax
0042f051  mov        dword ptr [0x4cf42c], eax
0042f056  test       eax, eax
0042f058  jg         0x42f082

// block 0042f05a
0042f05a  test       dword ptr [0x4d48c4], 0x800000
0042f064  je         0x42f082

// block 0042f066
0042f066  mov        eax, dword ptr [0x4cf428]
0042f06b  or         eax, 2
0042f06e  mov        ecx, eax
0042f070  and        ecx, 0xfffffffc
0042f073  add        ecx, 4
0042f076  xor        ecx, eax
0042f078  and        ecx, 0xc
0042f07b  xor        eax, ecx
0042f07d  mov        dword ptr [0x4cf428], eax

// block 0042f082
0042f082  mov        eax, dword ptr [esi + 0x80c]
0042f088  test       eax, eax
0042f08a  je         0x42f09b

// block 0042f08c
0042f08c  sub        eax, 2
0042f08f  neg        eax
0042f091  sbb        eax, eax
0042f093  and        eax, 0xfffffffd
0042f096  add        eax, 4
0042f099  pop        esi
0042f09a  ret        

// block 0042f09b
0042f09b  mov        eax, esi
0042f09d  call       0x4311e0

// block 0042f0a2
0042f0a2  cmp        eax, 1
0042f0a5  jne        0x42f0a7

// block 0042f0a7
0042f0a7  pop        esi
0042f0a8  ret        
