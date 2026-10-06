
// block 00465390
00465390  push       ecx
00465391  mov        dword ptr [esp], ecx
00465394  mov        eax, dword ptr [esp]
00465397  fld        dword ptr [esp + 8]
0046539b  fsincos    
0046539d  fmul       dword ptr [esp + 0xc]
004653a1  fstp       dword ptr [eax]
004653a3  fmul       dword ptr [esp + 0xc]
004653a7  fstp       dword ptr [eax + 4]
004653aa  pop        ecx
004653ab  ret        8
