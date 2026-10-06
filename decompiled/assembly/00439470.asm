
// block 00439470
00439470  fild       dword ptr [ecx]
00439472  fld        qword ptr [0x4a3d40]
00439478  fmul       st(1), st(0)
0043947a  fxch       st(1)
0043947c  fstp       dword ptr [eax]
0043947e  fimul      dword ptr [ecx + 4]
00439481  fstp       dword ptr [eax + 4]
00439484  fldz       
00439486  fstp       dword ptr [eax + 8]
00439489  ret        
