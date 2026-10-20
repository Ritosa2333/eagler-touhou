
// block 0040d600
0040d600  fld        dword ptr [eax]
0040d602  fsub       dword ptr [ecx]
0040d604  fstp       dword ptr [eax]
0040d606  fld        dword ptr [eax + 4]
0040d609  fsub       dword ptr [ecx + 4]
0040d60c  fstp       dword ptr [eax + 4]
0040d60f  fld        dword ptr [eax + 8]
0040d612  fsub       dword ptr [ecx + 8]
0040d615  fstp       dword ptr [eax + 8]
0040d618  ret        
