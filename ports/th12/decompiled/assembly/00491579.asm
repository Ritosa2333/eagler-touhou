
// block 00491579
00491579  mov        edi, edi
0049157b  push       ebp
0049157c  mov        ebp, esp
0049157e  push       esi
0049157f  mov        esi, ecx
00491581  call       0x49155a

// block 00491586
00491586  test       byte ptr [ebp + 8], 1
0049158a  je         0x491593

// block 0049158c
0049158c  push       esi
0049158d  call       0x46ca4f

// block 00491592
00491592  pop        ecx

// block 00491593
00491593  mov        eax, esi
00491595  pop        esi
00491596  pop        ebp
00491597  ret        4
