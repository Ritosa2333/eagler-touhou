
// block 0046dfb1
0046dfb1  mov        edi, edi
0046dfb3  push       ebp
0046dfb4  mov        ebp, esp
0046dfb6  mov        eax, ecx
0046dfb8  mov        ecx, dword ptr [ebp + 8]
0046dfbb  mov        dword ptr [eax], 0x49cd28
0046dfc1  mov        ecx, dword ptr [ecx]
0046dfc3  and        dword ptr [eax + 8], 0
0046dfc7  mov        dword ptr [eax + 4], ecx
0046dfca  pop        ebp
0046dfcb  ret        8
