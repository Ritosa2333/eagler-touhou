
// block 00492212
00492212  mov        edi, edi
00492214  push       ebp
00492215  mov        ebp, esp
00492217  mov        eax, dword ptr [ebp + 8]
0049221a  push       esi
0049221b  push       edi
0049221c  mov        edi, dword ptr [ebp + 0xc]
0049221f  test       eax, eax
00492221  je         0x492294

// block 00492223
00492223  mov        esi, dword ptr [eax]
00492225  test       esi, esi
00492227  je         0x492294

// block 00492229
00492229  cmp        dword ptr [esi], 0xe06d7363
0049222f  jne        0x492260

// block 00492231
00492231  cmp        dword ptr [esi + 0x10], 3
00492235  jne        0x492260

// block 00492237
00492237  mov        eax, dword ptr [esi + 0x14]
0049223a  cmp        eax, 0x19930520
0049223f  je         0x49224f

// block 00492241
00492241  cmp        eax, 0x19930521
00492246  je         0x49224f

// block 00492248
00492248  cmp        eax, 0x19930522
0049224d  jne        0x492260

// block 0049224f
0049224f  cmp        dword ptr [esi + 0x1c], 0
00492253  jne        0x492260

// block 00492255
00492255  call       0x475467

// block 0049225a
0049225a  mov        esi, dword ptr [eax + 0x88]

// block 00492260
00492260  push       dword ptr [esi + 0x18]
00492263  push       edi
00492264  call       0x491d54

// block 00492269
00492269  pop        ecx
0049226a  pop        ecx
0049226b  call       0x475467

// block 00492270
00492270  mov        eax, dword ptr [eax + 0x88]
00492276  mov        dword ptr [edi + 8], eax
00492279  call       0x475467

// block 0049227e
0049227e  mov        eax, dword ptr [eax + 0x8c]
00492284  mov        dword ptr [edi + 0xc], eax
00492287  call       0x475467

// block 0049228c
0049228c  mov        dword ptr [eax + 0x88], esi
00492292  jmp        0x49229c

// block 00492294
00492294  or         dword ptr [edi + 8], 0xffffffff
00492298  or         dword ptr [edi + 0xc], 0xffffffff

// block 0049229c
0049229c  call       0x475467

// block 004922a1
004922a1  add        eax, 0x90
004922a6  dec        dword ptr [eax]
004922a8  call       0x475467

// block 004922ad
004922ad  cmp        dword ptr [eax + 0x90], 0
004922b4  pop        edi
004922b5  pop        esi
004922b6  jge        0x4922c4

// block 004922b8
004922b8  call       0x475467

// block 004922bd
004922bd  and        dword ptr [eax + 0x90], 0

// block 004922c4
004922c4  xor        eax, eax
004922c6  inc        eax
004922c7  pop        ebp
004922c8  ret        
