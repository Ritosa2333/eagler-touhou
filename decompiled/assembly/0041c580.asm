
// block 0041c580
0041c580  push       ecx
0041c581  mov        dword ptr [esp], ecx
0041c584  mov        eax, dword ptr [esp]
0041c587  fld        dword ptr [esp + 8]
0041c58b  fsincos    
0041c58d  fmul       dword ptr [esp + 0xc]
0041c591  fstp       dword ptr [eax]
0041c593  fmul       dword ptr [esp + 0xc]
0041c597  fstp       dword ptr [eax + 4]
0041c59a  pop        ecx
0041c59b  ret        8
