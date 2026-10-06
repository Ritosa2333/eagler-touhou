
// block 0041c9d0
0041c9d0  push       ecx
0041c9d1  mov        dword ptr [esp], ecx
0041c9d4  mov        eax, dword ptr [esp]
0041c9d7  fld        dword ptr [esp + 8]
0041c9db  fsincos    
0041c9dd  fmul       dword ptr [esp + 0xc]
0041c9e1  fstp       dword ptr [eax]
0041c9e3  fmul       dword ptr [esp + 0x10]
0041c9e7  fstp       dword ptr [eax + 4]
0041c9ea  pop        ecx
0041c9eb  ret        0xc
