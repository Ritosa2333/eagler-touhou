
// block 004067e0
004067e0  mov        ecx, dword ptr [eax + 0x10]
004067e3  mov        edx, dword ptr [esp + 4]
004067e7  test       cl, 1
004067ea  jne        0x40680b

// block 004067ec
004067ec  fldz       
004067ee  or         ecx, 1
004067f1  fstp       dword ptr [eax + 8]
004067f4  mov        dword ptr [eax + 4], 0
004067fb  mov        dword ptr [eax], 0xfff0bdc1
00406801  mov        dword ptr [eax + 0xc], 0x4b2ed0
00406808  mov        dword ptr [eax + 0x10], ecx

// block 0040680b
0040680b  fild       dword ptr [esp + 4]
0040680f  mov        dword ptr [eax + 4], edx
00406812  dec        edx
00406813  mov        dword ptr [eax], edx
00406815  fstp       dword ptr [eax + 8]
00406818  ret        4
