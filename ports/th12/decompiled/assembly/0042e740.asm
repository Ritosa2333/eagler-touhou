
// block 0042e740
0042e740  push       ecx
0042e741  fld        dword ptr [eax]
0042e743  push       ecx
0042e744  fadd       dword ptr [esi]
0042e746  fstp       dword ptr [esp + 4]
0042e74a  fld        dword ptr [esp + 4]
0042e74e  fstp       dword ptr [esp]
0042e751  call       0x4646e0

// block 0042e756
0042e756  fstp       dword ptr [esi]
0042e758  mov        eax, esi
0042e75a  pop        ecx
0042e75b  ret        
