
// block 0044d4d0
0044d4d0  mov        eax, edx
0044d4d2  push       esi
0044d4d3  lea        esi, [eax + 1]

// block 0044d4d6
0044d4d6  mov        cl, byte ptr [eax]
0044d4d8  inc        eax
0044d4d9  test       cl, cl
0044d4db  jne        0x44d4d6

// block 0044d4dd
0044d4dd  mov        ecx, dword ptr [esp + 8]
0044d4e1  sub        eax, esi
0044d4e3  push       eax
0044d4e4  push       edx
0044d4e5  add        ecx, 0xc
0044d4e8  call       0x44ec80

// block 0044d4ed
0044d4ed  pop        esi
0044d4ee  ret        4
