
// block 0046e1e9
0046e1e9  mov        edi, edi
0046e1eb  push       ebp
0046e1ec  mov        ebp, esp
0046e1ee  push       esi
0046e1ef  mov        esi, ecx
0046e1f1  mov        dword ptr [esi], 0x49cd54
0046e1f7  call       0x46e086

// block 0046e1fc
0046e1fc  test       byte ptr [ebp + 8], 1
0046e200  je         0x46e209

// block 0046e202
0046e202  push       esi
0046e203  call       0x46ca4f

// block 0046e208
0046e208  pop        ecx

// block 0046e209
0046e209  mov        eax, esi
0046e20b  pop        esi
0046e20c  pop        ebp
0046e20d  ret        4
