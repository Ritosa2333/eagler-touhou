
// block 004065f0
004065f0  push       ecx
004065f1  fld        dword ptr [eax + 4]
004065f4  fld        dword ptr [eax]
004065f6  fmul       st(0), st(0)
004065f8  fld        st(1)
004065fa  fmulp      st(2)
004065fc  faddp      st(1)
004065fe  fstp       dword ptr [esp]
00406601  fld        dword ptr [esp]
00406604  pop        ecx
00406605  ret        
