
// block 00461250
00461250  mov        ecx, dword ptr [0x4ce8cc]
00461256  lea        edx, [ebx + 4]
00461259  mov        dword ptr [edx], ebx
0046125b  mov        dword ptr [edx + 4], 0
00461262  mov        dword ptr [edx + 8], 0
00461269  cmp        dword ptr [ecx + 0x8856b8], 0
00461270  jne        0x46127a

// block 00461272
00461272  mov        dword ptr [ecx + 0x8856b8], edx
00461278  jmp        0x46129a

// block 0046127a
0046127a  push       esi
0046127b  mov        esi, dword ptr [ecx + 0x8856bc]
00461281  push       edi
00461282  mov        edi, dword ptr [esi + 4]
00461285  test       edi, edi
00461287  je         0x461292

// block 00461289
00461289  mov        dword ptr [edx + 4], edi
0046128c  mov        edi, dword ptr [esi + 4]
0046128f  mov        dword ptr [edi + 8], edx

// block 00461292
00461292  mov        dword ptr [esi + 4], edx
00461295  pop        edi
00461296  mov        dword ptr [edx + 8], esi
00461299  pop        esi

// block 0046129a
0046129a  mov        dword ptr [ecx + 0x8856bc], edx
004612a0  mov        edx, 1
004612a5  add        dword ptr [ecx + 0x88ed48], edx
004612ab  cmp        dword ptr [ecx + 0x88ed48], 0
004612b2  jne        0x4612ba

// block 004612b4
004612b4  add        dword ptr [ecx + 0x88ed48], edx

// block 004612ba
004612ba  mov        edx, dword ptr [ecx + 0x88ed48]
004612c0  mov        dword ptr [ebx], edx
004612c2  mov        ecx, dword ptr [ecx + 0x88ed48]
004612c8  mov        dword ptr [eax], ecx
004612ca  ret        
