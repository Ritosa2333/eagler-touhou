
// block 0046a4c0
0046a4c0  push       ecx
0046a4c1  fld        dword ptr [eax]
0046a4c3  push       ecx
0046a4c4  fadd       dword ptr [ecx]
0046a4c6  fstp       dword ptr [esp + 4]
0046a4ca  fld        dword ptr [esp + 4]
0046a4ce  fstp       dword ptr [esp]
0046a4d1  call       0x4646e0

// block 0046a4d6
0046a4d6  fstp       dword ptr [esi]
0046a4d8  mov        eax, esi
0046a4da  pop        ecx
0046a4db  ret        
