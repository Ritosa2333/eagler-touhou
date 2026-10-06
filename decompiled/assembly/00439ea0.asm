
// block 00439ea0
00439ea0  fld        dword ptr [eax]
00439ea2  fld        qword ptr [0x4a3cf8]
00439ea8  fmul       st(1), st(0)
00439eaa  fxch       st(1)
00439eac  fsubr      dword ptr [ecx]
00439eae  fstp       dword ptr [esi]
00439eb0  fld        dword ptr [ecx + 4]
00439eb3  fld        dword ptr [eax + 4]
00439eb6  fmul       st(2)
00439eb8  fsubp      st(1)
00439eba  fstp       dword ptr [esi + 4]
00439ebd  fld        dword ptr [eax]
00439ebf  fmul       st(1)
00439ec1  fadd       dword ptr [ecx]
00439ec3  fstp       dword ptr [edx]
00439ec5  fmul       dword ptr [eax + 4]
00439ec8  fadd       dword ptr [ecx + 4]
00439ecb  fstp       dword ptr [edx + 4]
00439ece  ret        
