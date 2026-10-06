
// block 00469270
00469270  push       ecx
00469271  mov        dword ptr [esp], ecx
00469274  mov        eax, dword ptr [esp]
00469277  fld        dword ptr [esp + 8]
0046927b  fsincos    
0046927d  fmul       dword ptr [esp + 0xc]
00469281  fstp       dword ptr [eax]
00469283  fmul       dword ptr [esp + 0xc]
00469287  fstp       dword ptr [eax + 4]
0046928a  pop        ecx
0046928b  ret        8
