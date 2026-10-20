
// block 0040d4d0
0040d4d0  push       ecx
0040d4d1  mov        dword ptr [esp], ecx
0040d4d4  mov        eax, dword ptr [esp]
0040d4d7  fld        dword ptr [esp + 8]
0040d4db  fsincos    
0040d4dd  fmul       dword ptr [esp + 0xc]
0040d4e1  fstp       dword ptr [eax]
0040d4e3  fmul       dword ptr [esp + 0xc]
0040d4e7  fstp       dword ptr [eax + 4]
0040d4ea  pop        ecx
0040d4eb  ret        8
