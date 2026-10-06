
// block 004393d0
004393d0  push       ecx
004393d1  mov        dword ptr [esp], ecx
004393d4  mov        eax, dword ptr [esp]
004393d7  fld        dword ptr [esp + 8]
004393db  fsincos    
004393dd  fmul       dword ptr [esp + 0xc]
004393e1  fstp       dword ptr [eax]
004393e3  fmul       dword ptr [esp + 0xc]
004393e7  fstp       dword ptr [eax + 4]
004393ea  pop        ecx
004393eb  ret        8
