
// block 00459100
00459100  sub        esp, 0xc
00459103  mov        eax, dword ptr [edi + 0x24]
00459106  push       esi
00459107  mov        dword ptr [esp + 4], eax
0045910b  test       eax, eax
0045910d  jle        0x45916f

// block 0045910f
0045910f  lea        esi, [edi + 0x10]
00459112  call       0x464a80

// block 00459117
00459117  mov        ecx, dword ptr [edi + 0x24]
0045911a  cmp        dword ptr [edi + 0x14], ecx
0045911d  mov        dword ptr [esp + 4], ecx
00459121  jl         0x45916f

// block 00459123
00459123  mov        eax, dword ptr [esi + 0x10]
00459126  mov        dword ptr [esp + 4], ecx
0045912a  test       al, 1
0045912c  jne        0x45914d

// block 0045912e
0045912e  fldz       
00459130  or         eax, 1
00459133  fstp       dword ptr [esi + 8]
00459136  mov        dword ptr [esi + 4], 0
0045913d  mov        dword ptr [esi], 0xfff0bdc1
00459143  mov        dword ptr [esi + 0xc], 0x4b2ed0
0045914a  mov        dword ptr [esi + 0x10], eax

// block 0045914d
0045914d  fild       dword ptr [esp + 4]
00459151  mov        dword ptr [esi + 4], ecx
00459154  dec        ecx
00459155  mov        dword ptr [esi], ecx
00459157  fstp       dword ptr [esi + 8]
0045915a  cmp        dword ptr [edi + 0x28], 7
0045915e  mov        dword ptr [edi + 0x24], 0
00459165  je         0x45919e

// block 00459167
00459167  fld        dword ptr [edi + 4]
0045916a  pop        esi
0045916b  add        esp, 0xc
0045916e  ret        

// block 0045916f
0045916f  mov        eax, dword ptr [edi + 0x28]
00459172  cmp        eax, 7
00459175  jne        0x459189

// block 00459177
00459177  fld        dword ptr [edi + 4]
0045917a  pop        esi
0045917b  fadd       dword ptr [edi]
0045917d  fstp       dword ptr [esp]
00459180  fld        dword ptr [esp]
00459183  fst        dword ptr [edi]
00459185  add        esp, 0xc
00459188  ret        

// block 00459189
00459189  cmp        eax, 0x11
0045918c  jne        0x4591a5

// block 0045918e
0045918e  fld        dword ptr [edi + 0xc]
00459191  fadd       dword ptr [edi]
00459193  fstp       dword ptr [edi]
00459195  fld        dword ptr [edi + 4]
00459198  fadd       dword ptr [edi + 0xc]
0045919b  fstp       dword ptr [edi + 0xc]

// block 0045919e
0045919e  fld        dword ptr [edi]
004591a0  pop        esi
004591a1  add        esp, 0xc
004591a4  ret        

// block 004591a5
004591a5  cmp        eax, 8
004591a8  jne        0x459230

// block 004591ae
004591ae  fld        dword ptr [edi + 0x18]
004591b1  pop        esi
004591b2  fidiv      dword ptr [esp]
004591b5  fstp       dword ptr [esp]
004591b8  fld        dword ptr [esp]
004591bb  fld        st(0)
004591bd  fld1       
004591bf  fsub       st(1), st(0)
004591c1  fld        st(1)
004591c3  fld        st(3)
004591c5  fadd       st(0), st(0)
004591c7  fld        st(4)
004591c9  fld        st(5)
004591cb  fsubr      st(4)
004591cd  fstp       qword ptr [esp + 4]
004591d1  fld        qword ptr [0x4a3fd8]
004591d7  fsub       st(2)
004591d9  fld        st(1)
004591db  fmulp      st(2)
004591dd  fmulp      st(1)
004591df  fstp       dword ptr [esp]
004591e2  fld        dword ptr [esp]
004591e5  fmul       dword ptr [edi + 4]
004591e8  fxch       st(1)
004591ea  faddp      st(3)
004591ec  fld        st(1)
004591ee  fmulp      st(2)
004591f0  fxch       st(2)
004591f2  fmulp      st(1)
004591f4  fstp       dword ptr [esp]
004591f7  fld        dword ptr [esp]
004591fa  fmul       dword ptr [edi]
004591fc  faddp      st(1)
004591fe  fld        qword ptr [esp + 4]
00459202  fmul       st(0), st(0)
00459204  fmul       st(3)
00459206  fstp       dword ptr [esp]
00459209  fld        dword ptr [esp]
0045920c  fmul       dword ptr [edi + 8]
0045920f  faddp      st(1)
00459211  fld        st(2)
00459213  fmulp      st(2)
00459215  fxch       st(1)
00459217  fmulp      st(2)
00459219  fxch       st(1)
0045921b  fstp       dword ptr [esp]
0045921e  fld        dword ptr [esp]
00459221  fmul       dword ptr [edi + 0xc]
00459224  faddp      st(1)
00459226  fstp       dword ptr [esp]
00459229  fld        dword ptr [esp]
0045922c  add        esp, 0xc
0045922f  ret        

// block 00459230
00459230  mov        eax, edi
00459232  call       0x459390

// block 00459237
00459237  fld        dword ptr [edi + 4]
0045923a  fsub       dword ptr [edi]
0045923c  pop        esi
0045923d  fmulp      st(1)
0045923f  fadd       dword ptr [edi]
00459241  fstp       dword ptr [esp]
00459244  fld        dword ptr [esp]
00459247  add        esp, 0xc
0045924a  ret        
