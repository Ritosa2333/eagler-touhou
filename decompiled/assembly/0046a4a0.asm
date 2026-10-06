
// block 0046a4a0
0046a4a0  push       ecx
0046a4a1  mov        dword ptr [esp], ecx
0046a4a4  mov        eax, dword ptr [esp]
0046a4a7  fld        dword ptr [esp + 8]
0046a4ab  fsincos    
0046a4ad  fmul       dword ptr [esp + 0xc]
0046a4b1  fstp       dword ptr [eax]
0046a4b3  fmul       dword ptr [esp + 0xc]
0046a4b7  fstp       dword ptr [eax + 4]
0046a4ba  pop        ecx
0046a4bb  ret        8
