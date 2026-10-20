
// block 0040c3b0
0040c3b0  mov        ecx, dword ptr [eax + 0x90c]
0040c3b6  sub        esp, 0xc
0040c3b9  cmp        ecx, dword ptr [eax + 0x930]
0040c3bf  jl         0x40c3d1

// block 0040c3c1
0040c3c1  and        dword ptr [eax + 0x528], 0xfffffff5
0040c3c8  mov        eax, 1
0040c3cd  add        esp, 0xc
0040c3d0  ret        

// block 0040c3d1
0040c3d1  fld        dword ptr [eax + 0x924]
0040c3d7  xor        edx, edx
0040c3d9  fld        dword ptr [0x4b2ed0]
0040c3df  fld        st(0)
0040c3e1  fmulp      st(2)
0040c3e3  fxch       st(1)
0040c3e5  fstp       dword ptr [esp]
0040c3e8  fld        dword ptr [eax + 0x928]
0040c3ee  fmul       st(1)
0040c3f0  fstp       dword ptr [esp + 4]
0040c3f4  fmul       dword ptr [eax + 0x92c]
0040c3fa  fstp       dword ptr [esp + 8]
0040c3fe  fld        dword ptr [eax + 0x4bc]
0040c404  fadd       dword ptr [esp]
0040c407  fstp       dword ptr [eax + 0x4bc]
0040c40d  fld        dword ptr [eax + 0x4c0]
0040c413  fadd       dword ptr [esp + 4]
0040c417  fstp       dword ptr [eax + 0x4c0]
0040c41d  fld        dword ptr [esp + 8]
0040c421  fadd       dword ptr [eax + 0x4c4]
0040c427  fstp       dword ptr [eax + 0x4c4]
0040c42d  mov        ecx, dword ptr [eax + 0x918]
0040c433  fldz       
0040c435  test       cl, 1
0040c438  jne        0x40c463

// block 0040c43a
0040c43a  or         ecx, 1
0040c43d  fst        dword ptr [eax + 0x910]
0040c443  mov        dword ptr [eax + 0x90c], edx
0040c449  mov        dword ptr [eax + 0x908], 0xfff0bdc1
0040c453  mov        dword ptr [eax + 0x914], 0x4b2ed0
0040c45d  mov        dword ptr [eax + 0x918], ecx

// block 0040c463
0040c463  fstp       dword ptr [eax + 0x910]
0040c469  mov        dword ptr [eax + 0x90c], edx
0040c46f  mov        dword ptr [eax + 0x908], 0xffffffff
0040c479  xor        eax, eax
0040c47b  add        esp, 0xc
0040c47e  ret        
