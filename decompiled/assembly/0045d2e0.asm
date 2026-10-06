
// block 0045d2e0
0045d2e0  push       ecx
0045d2e1  fld        dword ptr [esp + 0x18]
0045d2e5  push       edi
0045d2e6  sub        esp, 0x14
0045d2e9  fstp       dword ptr [esp + 0x10]
0045d2ed  mov        edi, eax
0045d2ef  fld        dword ptr [esp + 0x2c]
0045d2f3  mov        edx, esi
0045d2f5  fld1       
0045d2f7  shr        edx, 1
0045d2f9  fadd       st(1), st(0)
0045d2fb  mov        eax, esi
0045d2fd  fxch       st(1)
0045d2ff  and        edx, 0x7f000000
0045d305  and        eax, 0xffffff
0045d30a  or         edx, eax
0045d30c  fstp       dword ptr [esp + 0x18]
0045d310  fld        dword ptr [esp + 0x18]
0045d314  fstp       dword ptr [esp + 0xc]
0045d318  fadd       dword ptr [esp + 0x28]
0045d31c  fstp       dword ptr [esp + 0x18]
0045d320  fld        dword ptr [esp + 0x18]
0045d324  fstp       dword ptr [esp + 8]
0045d328  fld        dword ptr [esp + 0x24]
0045d32c  fstp       dword ptr [esp + 4]
0045d330  fld        dword ptr [esp + 0x20]
0045d334  fstp       dword ptr [esp]
0045d337  call       0x45ce60

// block 0045d33c
0045d33c  fld        dword ptr [esp + 0x1c]
0045d340  sub        esp, 0x14
0045d343  fstp       dword ptr [esp + 0x10]
0045d347  mov        edx, esi
0045d349  fld        dword ptr [esp + 0x2c]
0045d34d  fstp       dword ptr [esp + 0xc]
0045d351  fld        dword ptr [esp + 0x28]
0045d355  fstp       dword ptr [esp + 8]
0045d359  fld        dword ptr [esp + 0x24]
0045d35d  fstp       dword ptr [esp + 4]
0045d361  fld        dword ptr [esp + 0x20]
0045d365  fstp       dword ptr [esp]
0045d368  call       0x45ce60

// block 0045d36d
0045d36d  xor        eax, eax
0045d36f  pop        edi
0045d370  pop        ecx
0045d371  ret        0x14
