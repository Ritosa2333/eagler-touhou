
// block 004594b0
004594b0  push       ecx
004594b1  mov        dword ptr [esp], ecx
004594b4  mov        eax, dword ptr [esp]
004594b7  fld        dword ptr [esp + 8]
004594bb  fsincos    
004594bd  fmul       dword ptr [esp + 0xc]
004594c1  fstp       dword ptr [eax]
004594c3  fmul       dword ptr [esp + 0xc]
004594c7  fstp       dword ptr [eax + 4]
004594ca  pop        ecx
004594cb  ret        8
