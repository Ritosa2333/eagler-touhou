
// block 0042e6f0
0042e6f0  fld        dword ptr [eax]
0042e6f2  sub        esp, 8
0042e6f5  fstp       dword ptr [esp + 4]
0042e6f9  fld        dword ptr [ecx]
0042e6fb  fstp       dword ptr [esp]
0042e6fe  call       0x42e770

// block 0042e703
0042e703  push       ecx
0042e704  fstp       dword ptr [esp]
0042e707  call       0x4646e0

// block 0042e70c
0042e70c  fstp       dword ptr [esi]
0042e70e  mov        eax, esi
0042e710  ret        
