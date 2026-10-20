
// block 004064c0
004064c0  fld        dword ptr [ecx]
004064c2  fadd       dword ptr [edx]
004064c4  fstp       dword ptr [eax]
004064c6  fld        dword ptr [ecx + 4]
004064c9  fadd       dword ptr [edx + 4]
004064cc  fstp       dword ptr [eax + 4]
004064cf  fld        dword ptr [ecx + 8]
004064d2  fadd       dword ptr [edx + 8]
004064d5  fstp       dword ptr [eax + 8]
004064d8  ret        
