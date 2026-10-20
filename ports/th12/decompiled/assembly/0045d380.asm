
// block 0045d380
0045d380  push       ecx
0045d381  fld        dword ptr [esp + 0x18]
0045d385  push       edi
0045d386  sub        esp, 0x14
0045d389  fstp       dword ptr [esp + 0x10]
0045d38d  mov        edi, eax
0045d38f  fld        dword ptr [esp + 0x2c]
0045d393  mov        edx, esi
0045d395  fld1       
0045d397  shr        edx, 1
0045d399  fadd       st(1), st(0)
0045d39b  mov        eax, esi
0045d39d  fxch       st(1)
0045d39f  and        eax, 0xffffff
0045d3a4  and        edx, 0x7f000000
0045d3aa  or         edx, eax
0045d3ac  fstp       dword ptr [esp + 0x18]
0045d3b0  mov        eax, ebx
0045d3b2  fld        dword ptr [esp + 0x18]
0045d3b6  shr        eax, 1
0045d3b8  fstp       dword ptr [esp + 0xc]
0045d3bc  mov        ecx, ebx
0045d3be  and        eax, 0x7f000000
0045d3c3  fadd       dword ptr [esp + 0x28]
0045d3c7  and        ecx, 0xffffff
0045d3cd  or         eax, ecx
0045d3cf  fstp       dword ptr [esp + 0x18]
0045d3d3  fld        dword ptr [esp + 0x18]
0045d3d7  fstp       dword ptr [esp + 8]
0045d3db  fld        dword ptr [esp + 0x24]
0045d3df  fstp       dword ptr [esp + 4]
0045d3e3  fld        dword ptr [esp + 0x20]
0045d3e7  fstp       dword ptr [esp]
0045d3ea  call       0x45d0a0

// block 0045d3ef
0045d3ef  fld        dword ptr [esp + 0x1c]
0045d3f3  sub        esp, 0x14
0045d3f6  fstp       dword ptr [esp + 0x10]
0045d3fa  mov        edx, esi
0045d3fc  fld        dword ptr [esp + 0x2c]
0045d400  mov        eax, ebx
0045d402  fstp       dword ptr [esp + 0xc]
0045d406  fld        dword ptr [esp + 0x28]
0045d40a  fstp       dword ptr [esp + 8]
0045d40e  fld        dword ptr [esp + 0x24]
0045d412  fstp       dword ptr [esp + 4]
0045d416  fld        dword ptr [esp + 0x20]
0045d41a  fstp       dword ptr [esp]
0045d41d  call       0x45d0a0

// block 0045d422
0045d422  xor        eax, eax
0045d424  pop        edi
0045d425  pop        ecx
0045d426  ret        0x14
