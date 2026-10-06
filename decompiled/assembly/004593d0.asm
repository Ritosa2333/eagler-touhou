
// block 004593d0
004593d0  fld        dword ptr [ecx]
004593d2  fld        dword ptr [esp + 4]
004593d6  fld        st(0)
004593d8  fmulp      st(2)
004593da  fxch       st(1)
004593dc  fstp       dword ptr [eax]
004593de  fmul       dword ptr [ecx + 4]
004593e1  fstp       dword ptr [eax + 4]
004593e4  ret        4
