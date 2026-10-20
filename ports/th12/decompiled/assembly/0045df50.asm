
// block 0045df50
0045df50  push       ecx
0045df51  mov        dword ptr [esp], ecx
0045df54  mov        eax, dword ptr [esp]
0045df57  fld        dword ptr [esp + 8]
0045df5b  fsincos    
0045df5d  fmul       dword ptr [esp + 0xc]
0045df61  fstp       dword ptr [eax]
0045df63  fmul       dword ptr [esp + 0xc]
0045df67  fstp       dword ptr [eax + 4]
0045df6a  pop        ecx
0045df6b  ret        8
