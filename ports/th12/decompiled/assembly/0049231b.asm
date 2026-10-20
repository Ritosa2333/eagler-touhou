
// block 0049231b
0049231b  mov        edi, edi
0049231d  push       ebp
0049231e  mov        ebp, esp
00492320  push       esi
00492321  mov        esi, dword ptr [ebp + 8]
00492324  cmp        dword ptr [esi + 8], -1
00492328  je         0x492457

// block 0049232e
0049232e  push       ebx
0049232f  push       edi
00492330  push       esi
00492331  call       0x491da7

// block 00492336
00492336  cmp        dword ptr [ebp + 0xc], 0
0049233a  pop        ecx
0049233b  mov        edi, 0xe06d7363
00492340  mov        ebx, 0x19930520
00492345  jne        0x4923cf

// block 0049234b
0049234b  call       0x475467

// block 00492350
00492350  mov        eax, dword ptr [eax + 0x88]
00492356  cmp        dword ptr [eax], edi
00492358  jne        0x4923cf

// block 0049235a
0049235a  call       0x475467

// block 0049235f
0049235f  mov        eax, dword ptr [eax + 0x88]
00492365  cmp        dword ptr [eax + 0x10], 3
00492369  jne        0x4923cf

// block 0049236b
0049236b  call       0x475467

// block 00492370
00492370  mov        eax, dword ptr [eax + 0x88]
00492376  cmp        dword ptr [eax + 0x14], ebx
00492379  je         0x4923a3

// block 0049237b
0049237b  call       0x475467

// block 00492380
00492380  mov        eax, dword ptr [eax + 0x88]
00492386  cmp        dword ptr [eax + 0x14], 0x19930521
0049238d  je         0x4923a3

// block 0049238f
0049238f  call       0x475467

// block 00492394
00492394  mov        eax, dword ptr [eax + 0x88]
0049239a  cmp        dword ptr [eax + 0x14], 0x19930522
004923a1  jne        0x4923cf

// block 004923a3
004923a3  call       0x475467

// block 004923a8
004923a8  mov        eax, dword ptr [eax + 0x88]
004923ae  push       dword ptr [eax + 0x18]
004923b1  call       0x491d80

// block 004923b6
004923b6  pop        ecx
004923b7  test       eax, eax
004923b9  je         0x4923cf

// block 004923bb
004923bb  push       1
004923bd  call       0x475467

// block 004923c2
004923c2  push       dword ptr [eax + 0x88]
004923c8  call       0x492181

// block 004923cd
004923cd  pop        ecx
004923ce  pop        ecx

// block 004923cf
004923cf  call       0x475467

// block 004923d4
004923d4  mov        eax, dword ptr [eax + 0x88]
004923da  cmp        dword ptr [eax], edi
004923dc  jne        0x492439

// block 004923de
004923de  call       0x475467

// block 004923e3
004923e3  mov        eax, dword ptr [eax + 0x88]
004923e9  cmp        dword ptr [eax + 0x10], 3
004923ed  jne        0x492439

// block 004923ef
004923ef  call       0x475467

// block 004923f4
004923f4  mov        eax, dword ptr [eax + 0x88]
004923fa  cmp        dword ptr [eax + 0x14], ebx
004923fd  je         0x492427

// block 004923ff
004923ff  call       0x475467

// block 00492404
00492404  mov        eax, dword ptr [eax + 0x88]
0049240a  cmp        dword ptr [eax + 0x14], 0x19930521
00492411  je         0x492427

// block 00492413
00492413  call       0x475467

// block 00492418
00492418  mov        eax, dword ptr [eax + 0x88]
0049241e  cmp        dword ptr [eax + 0x14], 0x19930522
00492425  jne        0x492439

// block 00492427
00492427  cmp        dword ptr [ebp + 0xc], 0
0049242b  je         0x492439

// block 0049242d
0049242d  call       0x475467

// block 00492432
00492432  add        eax, 0x90
00492437  dec        dword ptr [eax]

// block 00492439
00492439  call       0x475467

// block 0049243e
0049243e  mov        ecx, dword ptr [esi + 8]
00492441  mov        dword ptr [eax + 0x88], ecx
00492447  call       0x475467

// block 0049244c
0049244c  mov        ecx, dword ptr [esi + 0xc]
0049244f  pop        edi
00492450  mov        dword ptr [eax + 0x8c], ecx
00492456  pop        ebx

// block 00492457
00492457  pop        esi
00492458  pop        ebp
00492459  ret        
