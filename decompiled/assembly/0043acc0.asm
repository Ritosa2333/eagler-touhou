
// block 0043acc0
0043acc0  push       ecx
0043acc1  mov        dword ptr [esp], ecx
0043acc4  mov        eax, dword ptr [esp]
0043acc7  fld        dword ptr [esp + 8]
0043accb  fsincos    
0043accd  fmul       dword ptr [esp + 0xc]
0043acd1  fstp       dword ptr [eax]
0043acd3  fmul       dword ptr [esp + 0xc]
0043acd7  fstp       dword ptr [eax + 4]
0043acda  pop        ecx
0043acdb  ret        8
