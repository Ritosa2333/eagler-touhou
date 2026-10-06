
// block 0044eab0
0044eab0  mov        eax, edx
0044eab2  push       esi
0044eab3  lea        esi, [eax + 1]

// block 0044eab6
0044eab6  mov        cl, byte ptr [eax]
0044eab8  inc        eax
0044eab9  test       cl, cl
0044eabb  jne        0x44eab6

// block 0044eabd
0044eabd  mov        ecx, dword ptr [esp + 8]
0044eac1  sub        eax, esi
0044eac3  push       eax
0044eac4  push       edx
0044eac5  call       0x44ec80

// block 0044eaca
0044eaca  pop        esi
0044eacb  ret        4
