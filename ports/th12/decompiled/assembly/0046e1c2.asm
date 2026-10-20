
// block 0046e1c2
0046e1c2  mov        edi, edi
0046e1c4  push       ebp
0046e1c5  mov        ebp, esp
0046e1c7  push       esi
0046e1c8  mov        esi, ecx
0046e1ca  mov        dword ptr [esi], 0x49cd54
0046e1d0  call       0x46e086

// block 0046e1d5
0046e1d5  test       byte ptr [ebp + 8], 1
0046e1d9  je         0x46e1e2

// block 0046e1db
0046e1db  push       esi
0046e1dc  call       0x46ca4f

// block 0046e1e1
0046e1e1  pop        ecx

// block 0046e1e2
0046e1e2  mov        eax, esi
0046e1e4  pop        esi
0046e1e5  pop        ebp
0046e1e6  ret        4
