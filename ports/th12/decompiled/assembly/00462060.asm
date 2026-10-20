
// block 00462060
00462060  mov        eax, dword ptr [esi]
00462062  mov        edx, dword ptr [0x4ce8cc]
00462068  push       eax
00462069  call       0x461920

// block 0046206e
0046206e  test       eax, eax
00462070  jne        0x462074

// block 00462072
00462072  mov        dword ptr [esi], eax

// block 00462074
00462074  add        eax, 0x10
00462077  je         0x462099

// block 00462079
00462079  lea        esp, [esp]

// block 00462080
00462080  mov        ecx, dword ptr [eax]
00462082  movsx      edx, word ptr [ecx + 0x3ea]
00462089  cmp        edx, edi
0046208b  je         0x4620a2

// block 0046208d
0046208d  cmp        edi, -1
00462090  je         0x4620a2

// block 00462092
00462092  mov        eax, dword ptr [eax + 4]
00462095  test       eax, eax
00462097  jne        0x462080

// block 00462099
00462099  mov        dword ptr [ebx], 0
0046209f  mov        eax, ebx
004620a1  ret        

// block 004620a2
004620a2  mov        eax, ecx
004620a4  mov        ecx, dword ptr [eax]
004620a6  mov        dword ptr [ebx], ecx
004620a8  mov        eax, ebx
004620aa  ret        
