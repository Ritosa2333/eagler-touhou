
// block 0040b920
0040b920  push       ecx
0040b921  push       esi
0040b922  mov        esi, eax
0040b924  mov        eax, dword ptr [esi + 0x76c]
0040b92a  cmp        eax, dword ptr [esi + 0x790]
0040b930  jl         0x40b941

// block 0040b932
0040b932  and        dword ptr [esi + 0x528], 0xfffffff7
0040b939  mov        eax, 1
0040b93e  pop        esi
0040b93f  pop        ecx
0040b940  ret        

// block 0040b941
0040b941  fld        dword ptr [esi + 0x780]
0040b947  sub        esp, 8
0040b94a  fmul       dword ptr [0x4b2ed0]
0040b950  fstp       dword ptr [esp + 0xc]
0040b954  fld        dword ptr [esp + 0xc]
0040b958  fstp       dword ptr [esp + 4]
0040b95c  fld        dword ptr [esi + 0x4d8]
0040b962  fstp       dword ptr [esp]
0040b965  call       0x464640

// block 0040b96a
0040b96a  fstp       dword ptr [esi + 0x4d8]
0040b970  sub        esp, 8
0040b973  fld        dword ptr [esi + 0x77c]
0040b979  lea        ecx, [esi + 0x4c8]
0040b97f  fmul       dword ptr [0x4b2ed0]
0040b985  fadd       dword ptr [esi + 0x4d4]
0040b98b  fstp       dword ptr [esp + 0xc]
0040b98f  fld        dword ptr [esp + 0xc]
0040b993  fst        dword ptr [esi + 0x4d4]
0040b999  fstp       dword ptr [esp + 4]
0040b99d  fld        dword ptr [esi + 0x4d8]
0040b9a3  fstp       dword ptr [esp]
0040b9a6  call       0x40d640

// block 0040b9ab
0040b9ab  add        esi, 0x768
0040b9b1  call       0x464a80

// block 0040b9b6
0040b9b6  xor        eax, eax
0040b9b8  pop        esi
0040b9b9  pop        ecx
0040b9ba  ret        
