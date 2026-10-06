
// block 0045d940
0045d940  push       ecx
0045d941  fld        dword ptr [eax + 4]
0045d944  push       ecx
0045d945  fld        dword ptr [eax]
0045d947  fld        dword ptr [eax + 8]
0045d94a  fld        st(1)
0045d94c  fmulp      st(2)
0045d94e  fld        st(2)
0045d950  fmulp      st(3)
0045d952  fxch       st(1)
0045d954  faddp      st(2)
0045d956  fmul       st(0), st(0)
0045d958  faddp      st(1)
0045d95a  fstp       dword ptr [esp + 4]
0045d95e  fld        dword ptr [esp + 4]
0045d962  fstp       dword ptr [esp]
0045d965  call       0x408860

// block 0045d96a
0045d96a  pop        ecx
0045d96b  ret        
