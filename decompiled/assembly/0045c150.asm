
// block 0045c150
0045c150  sub        esp, 0x10
0045c153  push       esi
0045c154  mov        esi, eax
0045c156  cmp        esi, 3
0045c159  jge        0x45c163

// block 0045c15b
0045c15b  or         eax, 0xffffffff
0045c15e  pop        esi
0045c15f  add        esp, 0x10
0045c162  ret        

// block 0045c163
0045c163  fld        dword ptr [ecx + 0x60]
0045c166  lea        eax, [esi + 1]
0045c169  fadd       dword ptr [ecx + 0x90]
0045c16f  cdq        
0045c170  sub        eax, edx
0045c172  sar        eax, 1
0045c174  fstp       dword ptr [esp + 0xc]
0045c178  dec        eax
0045c179  fld        dword ptr [ecx + 0x7c]
0045c17c  fadd       dword ptr [ecx + 0x64]
0045c17f  fstp       dword ptr [esp + 8]
0045c183  fld        dword ptr [ecx + 0x90]
0045c189  fsub       dword ptr [ecx + 0x80]
0045c18f  fstp       dword ptr [esp + 4]
0045c193  fld        dword ptr [esp + 4]
0045c197  mov        dword ptr [esp + 4], eax
0045c19b  fidiv      dword ptr [esp + 4]
0045c19f  fstp       dword ptr [esp + 0x10]
0045c1a3  fld        dword ptr [esp + 0xc]
0045c1a7  fst        dword ptr [esp + 4]
0045c1ab  fld        dword ptr [esp + 0x10]
0045c1af  fld1       
0045c1b1  test       esi, esi
0045c1b3  jle        0x45c1f6

// block 0045c1b5
0045c1b5  fld        dword ptr [esp + 8]
0045c1b9  lea        edx, [esi - 1]
0045c1bc  shr        edx, 1
0045c1be  lea        eax, [edi + 0x18]
0045c1c1  inc        edx
0045c1c2  push       ebx
0045c1c3  jmp        0x45c1c7

// block 0045c1c5
0045c1c5  fxch       st(1)

// block 0045c1c7
0045c1c7  fld        dword ptr [esp + 8]
0045c1cb  add        eax, 0x38
0045c1ce  sub        edx, 1
0045c1d1  fst        dword ptr [eax - 0x3c]
0045c1d4  fxch       st(1)
0045c1d6  fst        dword ptr [eax - 0x38]
0045c1d9  mov        ebx, dword ptr [ecx + 0x3bc]
0045c1df  fxch       st(2)
0045c1e1  mov        dword ptr [eax - 0x40], ebx
0045c1e4  fst        dword ptr [eax - 0x44]
0045c1e7  fld        st(3)
0045c1e9  fsubp      st(2)
0045c1eb  fxch       st(1)
0045c1ed  fstp       dword ptr [esp + 8]
0045c1f1  jne        0x45c1c5

// block 0045c1f3
0045c1f3  fstp       st(1)
0045c1f5  pop        ebx

// block 0045c1f6
0045c1f6  cmp        esi, 1
0045c1f9  fld        dword ptr [ecx + 0x84]
0045c1ff  fadd       dword ptr [ecx + 0x64]
0045c202  fstp       dword ptr [esp + 8]
0045c206  fxch       st(2)
0045c208  fstp       dword ptr [esp + 4]
0045c20c  jle        0x45c24d

// block 0045c20e
0045c20e  fld        dword ptr [esp + 8]
0045c212  lea        edx, [esi - 2]
0045c215  shr        edx, 1
0045c217  lea        eax, [edi + 0x34]
0045c21a  inc        edx
0045c21b  jmp        0x45c21f

// block 0045c21d
0045c21d  fxch       st(2)

// block 0045c21f
0045c21f  fld        dword ptr [esp + 4]
0045c223  add        eax, 0x38
0045c226  sub        edx, 1
0045c229  fst        dword ptr [eax - 0x3c]
0045c22c  fxch       st(1)
0045c22e  fst        dword ptr [eax - 0x38]
0045c231  mov        esi, dword ptr [ecx + 0x3bc]
0045c237  fxch       st(3)
0045c239  mov        dword ptr [eax - 0x40], esi
0045c23c  fst        dword ptr [eax - 0x44]
0045c23f  fld        st(2)
0045c241  fsubp      st(2)
0045c243  fxch       st(1)
0045c245  fstp       dword ptr [esp + 4]
0045c249  jne        0x45c21d

// block 0045c24b
0045c24b  fstp       st(1)

// block 0045c24d
0045c24d  fstp       st(0)
0045c24f  xor        eax, eax
0045c251  fstp       st(0)
0045c253  pop        esi
0045c254  add        esp, 0x10
0045c257  ret        
