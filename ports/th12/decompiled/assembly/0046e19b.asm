
// block 0046e19b
0046e19b  mov        edi, edi
0046e19d  push       ebp
0046e19e  mov        ebp, esp
0046e1a0  push       esi
0046e1a1  mov        esi, ecx
0046e1a3  mov        dword ptr [esi], 0x49cd48
0046e1a9  call       0x46e086

// block 0046e1ae
0046e1ae  test       byte ptr [ebp + 8], 1
0046e1b2  je         0x46e1bb

// block 0046e1b4
0046e1b4  push       esi
0046e1b5  call       0x46ca4f

// block 0046e1ba
0046e1ba  pop        ecx

// block 0046e1bb
0046e1bb  mov        eax, esi
0046e1bd  pop        esi
0046e1be  pop        ebp
0046e1bf  ret        4
