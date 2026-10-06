
// block 0045d890
0045d890  push       ecx
0045d891  mov        dword ptr [esp], ecx
0045d894  mov        eax, dword ptr [esp]
0045d897  fld        dword ptr [esp + 8]
0045d89b  fsincos    
0045d89d  fmul       dword ptr [esp + 0xc]
0045d8a1  fstp       dword ptr [eax]
0045d8a3  fmul       dword ptr [esp + 0xc]
0045d8a7  fstp       dword ptr [eax + 4]
0045d8aa  pop        ecx
0045d8ab  ret        8
