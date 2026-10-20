
// block 0042e880
0042e880  push       ecx
0042e881  mov        dword ptr [esp], ecx
0042e884  mov        eax, dword ptr [esp]
0042e887  fld        dword ptr [esp + 8]
0042e88b  fsincos    
0042e88d  fmul       dword ptr [esp + 0xc]
0042e891  fstp       dword ptr [eax]
0042e893  fmul       dword ptr [esp + 0xc]
0042e897  fstp       dword ptr [eax + 4]
0042e89a  pop        ecx
0042e89b  ret        8
