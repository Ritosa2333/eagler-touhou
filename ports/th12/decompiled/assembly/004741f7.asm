
// block 004741f7
004741f7  mov        edi, edi
004741f9  push       ebp
004741fa  mov        ebp, esp
004741fc  push       esi
004741fd  push       edi
004741fe  call       0x475467

// block 00474203
00474203  mov        ecx, dword ptr [eax + 0x70]
00474206  test       cl, 2
00474209  push       0
0047420b  pop        edx
0047420c  sete       dl
0047420f  inc        edx
00474210  mov        edi, edx
00474212  mov        edx, dword ptr [ebp + 8]
00474215  cmp        edx, -1
00474218  je         0x474254

// block 0047421a
0047421a  xor        esi, esi
0047421c  cmp        edx, esi
0047421e  je         0x47425b

// block 00474220
00474220  cmp        edx, 1
00474223  je         0x47424c

// block 00474225
00474225  cmp        edx, 2
00474228  je         0x474247

// block 0047422a
0047422a  call       0x46e96f

// block 0047422f
0047422f  push       esi
00474230  push       esi
00474231  push       esi
00474232  push       esi
00474233  push       esi
00474234  mov        dword ptr [eax], 0x16
0047423a  call       0x470f2d

// block 0047423f
0047423f  add        esp, 0x14
00474242  or         eax, 0xffffffff
00474245  jmp        0x47425d

// block 00474247
00474247  and        ecx, 0xfffffffd
0047424a  jmp        0x47424f

// block 0047424c
0047424c  or         ecx, 2

// block 0047424f
0047424f  mov        dword ptr [eax + 0x70], ecx
00474252  jmp        0x47425b

// block 00474254
00474254  or         dword ptr [0x4ad9d4], 0xffffffff

// block 0047425b
0047425b  mov        eax, edi

// block 0047425d
0047425d  pop        edi
0047425e  pop        esi
0047425f  pop        ebp
00474260  ret        
