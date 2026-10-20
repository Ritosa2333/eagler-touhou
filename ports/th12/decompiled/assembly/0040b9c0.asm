
// block 0040b9c0
0040b9c0  push       ecx
0040b9c1  push       esi
0040b9c2  mov        esi, eax
0040b9c4  mov        eax, dword ptr [esi + 0x7c4]
0040b9ca  cmp        dword ptr [esi + 0x7a0], eax
0040b9d0  mov        dword ptr [esp + 4], eax
0040b9d4  jl         0x40baa8

// block 0040b9da
0040b9da  mov        edx, dword ptr [esi + 0x544]
0040b9e0  test       edx, edx
0040b9e2  jl         0x40b9e9

// block 0040b9e4
0040b9e4  call       0x453d90

// block 0040b9e9
0040b9e9  fld        dword ptr [esi + 0x7b4]
0040b9ef  inc        dword ptr [esi + 0x7cc]
0040b9f5  fadd       dword ptr [esi + 0x4d8]
0040b9fb  fstp       dword ptr [esi + 0x4d8]
0040ba01  fld        dword ptr [esi + 0x7b0]
0040ba07  fstp       dword ptr [esp + 4]
0040ba0b  fld        dword ptr [esp + 4]
0040ba0f  fst        dword ptr [esi + 0x4d4]
0040ba15  mov        eax, dword ptr [esi + 0x7ac]
0040ba1b  fstp       dword ptr [esp + 4]
0040ba1f  fldz       
0040ba21  test       al, 1
0040ba23  jne        0x40ba52

// block 0040ba25
0040ba25  or         eax, 1
0040ba28  fst        dword ptr [esi + 0x7a4]
0040ba2e  mov        dword ptr [esi + 0x7a0], 0
0040ba38  mov        dword ptr [esi + 0x79c], 0xfff0bdc1
0040ba42  mov        dword ptr [esi + 0x7a8], 0x4b2ed0
0040ba4c  mov        dword ptr [esi + 0x7ac], eax

// block 0040ba52
0040ba52  fstp       dword ptr [esi + 0x7a4]
0040ba58  mov        dword ptr [esi + 0x7a0], 0
0040ba62  mov        dword ptr [esi + 0x79c], 0xffffffff
0040ba6c  mov        eax, dword ptr [esi + 0x7cc]
0040ba72  cmp        eax, dword ptr [esi + 0x7c8]
0040ba78  jl         0x40bac0

// block 0040ba7a
0040ba7a  fld        dword ptr [esp + 4]
0040ba7e  sub        esp, 8
0040ba81  fstp       dword ptr [esp + 4]
0040ba85  lea        ecx, [esi + 0x4c8]
0040ba8b  fld        dword ptr [esi + 0x4d8]
0040ba91  fstp       dword ptr [esp]
0040ba94  call       0x40d640

// block 0040ba99
0040ba99  and        dword ptr [esi + 0x528], 0xffffffef
0040baa0  mov        eax, 1
0040baa5  pop        esi
0040baa6  pop        ecx
0040baa7  ret        

// block 0040baa8
0040baa8  fld        dword ptr [esi + 0x4d4]
0040baae  fld        dword ptr [esi + 0x7a4]
0040bab4  fmul       st(1)
0040bab6  fidiv      dword ptr [esp + 4]
0040baba  fsubp      st(1)
0040babc  fstp       dword ptr [esp + 4]

// block 0040bac0
0040bac0  fld        dword ptr [esp + 4]
0040bac4  sub        esp, 8
0040bac7  fstp       dword ptr [esp + 4]
0040bacb  lea        ecx, [esi + 0x4c8]
0040bad1  fld        dword ptr [esi + 0x4d8]
0040bad7  fstp       dword ptr [esp]
0040bada  call       0x40d640

// block 0040badf
0040badf  add        esi, 0x79c
0040bae5  call       0x464a80

// block 0040baea
0040baea  xor        eax, eax
0040baec  pop        esi
0040baed  pop        ecx
0040baee  ret        
