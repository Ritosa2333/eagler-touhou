
// block 00468f70
00468f70  mov        eax, dword ptr [esp + 4]
00468f74  test       eax, eax
00468f76  jl         0x468f89

// block 00468f78
00468f78  mov        edx, dword ptr [ecx + 0x100c]
00468f7e  add        ecx, 8
00468f81  add        edx, ecx
00468f83  mov        eax, dword ptr [edx + eax]

// block 00468f86
00468f86  ret        4

// block 00468f89
00468f89  cmp        eax, -1
00468f8c  jne        0x468fc3

// block 00468f8e
00468f8e  mov        edx, dword ptr [ecx + 0x1008]
00468f94  add        edx, -4
00468f97  js         0x468f86

// block 00468f99
00468f99  mov        dword ptr [ecx + 0x1008], edx
00468f9f  mov        eax, dword ptr [edx + ecx + 8]
00468fa3  add        edx, -4
00468fa6  mov        dword ptr [ecx + 0x1008], edx
00468fac  cmp        byte ptr [edx + ecx + 8], 0x66
00468fb1  mov        dword ptr [esp + 4], eax
00468fb5  jne        0x468f86

// block 00468fb7
00468fb7  fld        dword ptr [esp + 4]
00468fbb  call       0x4931e0

// block 00468fc0
00468fc0  ret        4

// block 00468fc3
00468fc3  mov        ecx, dword ptr [ecx + 0x1014]
00468fc9  mov        edx, dword ptr [ecx]
00468fcb  mov        dword ptr [esp + 4], eax
00468fcf  mov        eax, dword ptr [edx + 4]
00468fd2  jmp        eax
