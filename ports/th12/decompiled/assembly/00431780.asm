
// block 00431780
00431780  sub        esp, 0xc
00431783  fld        dword ptr [edx + 4]
00431786  fmul       dword ptr [ecx + 8]
00431789  fld        dword ptr [edx + 8]
0043178c  fmul       dword ptr [ecx + 4]
0043178f  fsubp      st(1)
00431791  fstp       dword ptr [esp]
00431794  fld        dword ptr [edx + 8]
00431797  fmul       dword ptr [ecx]
00431799  fld        dword ptr [edx]
0043179b  fmul       dword ptr [ecx + 8]
0043179e  fsubp      st(1)
004317a0  fstp       dword ptr [esp + 4]
004317a4  fld        dword ptr [ecx + 4]
004317a7  fmul       dword ptr [edx]
004317a9  fld        dword ptr [ecx]
004317ab  mov        ecx, dword ptr [esp]
004317ae  fmul       dword ptr [edx + 4]
004317b1  mov        edx, dword ptr [esp + 4]
004317b5  mov        dword ptr [eax], ecx
004317b7  mov        dword ptr [eax + 4], edx
004317ba  fsubp      st(1)
004317bc  fstp       dword ptr [esp + 8]
004317c0  mov        ecx, dword ptr [esp + 8]
004317c4  mov        dword ptr [eax + 8], ecx
004317c7  add        esp, 0xc
004317ca  ret        
