
// block 0040c170
0040c170  sub        esp, 8
0040c173  push       esi
0040c174  mov        esi, eax
0040c176  mov        eax, dword ptr [esi + 0x8a4]
0040c17c  cmp        eax, dword ptr [esi + 0x8c8]
0040c182  jl         0x40c198

// block 0040c184
0040c184  and        dword ptr [esi + 0x528], 0xff7fffff
0040c18e  mov        eax, 1
0040c193  pop        esi
0040c194  add        esp, 8
0040c197  ret        

// block 0040c198
0040c198  fld        dword ptr [esi + 0x4d8]
0040c19e  sub        esp, 8
0040c1a1  lea        ecx, [esi + 0x4bc]
0040c1a7  fstp       dword ptr [esp + 4]
0040c1ab  call       0x4377a0

// block 0040c1b0
0040c1b0  fstp       dword ptr [esp]
0040c1b3  fld        dword ptr [esi + 0x8b8]
0040c1b9  push       ecx
0040c1ba  fstp       dword ptr [esp]
0040c1bd  call       0x464640

// block 0040c1c2
0040c1c2  push       ecx
0040c1c3  fstp       dword ptr [esp]
0040c1c6  call       0x40d4f0

// block 0040c1cb
0040c1cb  fmul       dword ptr [esi + 0x8b4]
0040c1d1  sub        esp, 8
0040c1d4  fmul       dword ptr [0x4b2ed0]
0040c1da  fstp       dword ptr [esp + 0x10]
0040c1de  fld        dword ptr [esp + 0x10]
0040c1e2  fstp       dword ptr [esp + 4]
0040c1e6  fld        dword ptr [esi + 0x4d8]
0040c1ec  fstp       dword ptr [esp]
0040c1ef  call       0x464640

// block 0040c1f4
0040c1f4  fstp       dword ptr [esp + 8]
0040c1f8  sub        esp, 8
0040c1fb  fld        dword ptr [esp + 0x10]
0040c1ff  lea        ecx, [esi + 0x4c8]
0040c205  fst        dword ptr [esi + 0x4d8]
0040c20b  fld        dword ptr [esi + 0x4d4]
0040c211  fstp       dword ptr [esp + 4]
0040c215  fstp       dword ptr [esp]
0040c218  call       0x40d640

// block 0040c21d
0040c21d  add        esi, 0x8a0
0040c223  call       0x464a80

// block 0040c228
0040c228  xor        eax, eax
0040c22a  pop        esi
0040c22b  add        esp, 8
0040c22e  ret        
