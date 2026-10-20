
// block 00462380
00462380  mov        eax, dword ptr [esi + 0xc]
00462383  push       ebp
00462384  xor        ebp, ebp
00462386  push       edi
00462387  mov        edi, dword ptr [0x4ce89c]
0046238d  test       eax, eax
0046238f  je         0x46239f

// block 00462391
00462391  mov        ecx, dword ptr [esi + 0x20]
00462394  call       eax

// block 00462396
00462396  mov        ebp, eax
00462398  mov        dword ptr [esi + 0xc], 0

// block 0046239f
0046239f  test       dword ptr [0x4cee78], 0x8000
004623a9  je         0x4623bc

// block 004623ab
004623ab  push       0x4cf0f8
004623b0  call       dword ptr [0x498088]

// block 004623b6
004623b6  inc        byte ptr [0x4cf218]

// block 004623bc
004623bc  lea        eax, [edi + 0x14]
004623bf  mov        dword ptr [esi], ebx
004623c1  cmp        dword ptr [eax + 4], 0
004623c5  je         0x4623d8

// block 004623c7
004623c7  mov        ecx, dword ptr [eax + 4]
004623ca  mov        edx, dword ptr [ecx]
004623cc  cmp        dword ptr [edx], ebx
004623ce  jge        0x4623d8

// block 004623d0
004623d0  mov        eax, ecx
004623d2  cmp        dword ptr [eax + 4], 0
004623d6  jne        0x4623c7

// block 004623d8
004623d8  mov        edx, dword ptr [eax + 4]
004623db  lea        ecx, [esi + 0x14]
004623de  test       edx, edx
004623e0  je         0x4623eb

// block 004623e2
004623e2  mov        dword ptr [ecx + 4], edx
004623e5  mov        edx, dword ptr [eax + 4]
004623e8  mov        dword ptr [edx + 8], ecx

// block 004623eb
004623eb  mov        dword ptr [eax + 4], ecx
004623ee  mov        dword ptr [ecx + 8], eax
004623f1  test       dword ptr [0x4cee78], 0x8000
004623fb  je         0x46240e

// block 004623fd
004623fd  push       0x4cf0f8
00462402  call       dword ptr [0x49808c]

// block 00462408
00462408  dec        byte ptr [0x4cf218]

// block 0046240e
0046240e  pop        edi
0046240f  mov        eax, ebp
00462411  pop        ebp
00462412  ret        
