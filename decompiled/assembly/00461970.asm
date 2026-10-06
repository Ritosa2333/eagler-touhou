
// block 00461970
00461970  mov        eax, dword ptr [esp + 4]
00461974  mov        edx, dword ptr [0x4ce8cc]
0046197a  push       esi
0046197b  push       eax
0046197c  call       0x461920

// block 00461981
00461981  mov        esi, eax
00461983  test       esi, esi
00461985  je         0x4619d2

// block 00461987
00461987  mov        eax, dword ptr [esi + 0x494]
0046198d  test       eax, eax
0046198f  je         0x461998

// block 00461991
00461991  movsx      edx, bx
00461994  mov        ecx, esi
00461996  call       eax

// block 00461998
00461998  cmp        dword ptr [esi + 0x18], 0
0046199c  mov        word ptr [esi + 0x3c4], bx
004619a3  jne        0x4619d2

// block 004619a5
004619a5  mov        esi, dword ptr [esi + 0x14]
004619a8  test       esi, esi
004619aa  je         0x4619d2

// block 004619ac
004619ac  push       edi
004619ad  lea        ecx, [ecx]

// block 004619b0
004619b0  mov        edi, dword ptr [esi]
004619b2  mov        eax, dword ptr [edi + 0x494]
004619b8  test       eax, eax
004619ba  je         0x4619c3

// block 004619bc
004619bc  movsx      edx, bx
004619bf  mov        ecx, edi
004619c1  call       eax

// block 004619c3
004619c3  mov        word ptr [edi + 0x3c4], bx
004619ca  mov        esi, dword ptr [esi + 4]
004619cd  test       esi, esi
004619cf  jne        0x4619b0

// block 004619d1
004619d1  pop        edi

// block 004619d2
004619d2  pop        esi
004619d3  ret        4
