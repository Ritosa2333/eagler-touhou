
// block 0045c260
0045c260  sub        esp, 0x10
0045c263  push       esi
0045c264  mov        esi, eax
0045c266  cmp        esi, 3
0045c269  jge        0x45c273

// block 0045c26b
0045c26b  or         eax, 0xffffffff
0045c26e  pop        esi
0045c26f  add        esp, 0x10
0045c272  ret        

// block 0045c273
0045c273  fld        dword ptr [ecx + 0x64]
0045c276  lea        eax, [esi + 1]
0045c279  fadd       dword ptr [ecx + 0x90]
0045c27f  cdq        
0045c280  sub        eax, edx
0045c282  sar        eax, 1
0045c284  fstp       dword ptr [esp + 0xc]
0045c288  dec        eax
0045c289  fld        dword ptr [ecx + 0x7c]
0045c28c  fadd       dword ptr [ecx + 0x60]
0045c28f  fstp       dword ptr [esp + 8]
0045c293  fld        dword ptr [ecx + 0x90]
0045c299  fsub       dword ptr [ecx + 0x80]
0045c29f  fstp       dword ptr [esp + 4]
0045c2a3  fld        dword ptr [esp + 4]
0045c2a7  mov        dword ptr [esp + 4], eax
0045c2ab  fidiv      dword ptr [esp + 4]
0045c2af  fstp       dword ptr [esp + 0x10]
0045c2b3  fld        dword ptr [esp + 0xc]
0045c2b7  fst        dword ptr [esp + 4]
0045c2bb  fld        dword ptr [esp + 0x10]
0045c2bf  fld1       
0045c2c1  test       esi, esi
0045c2c3  jle        0x45c306

// block 0045c2c5
0045c2c5  fld        dword ptr [esp + 8]
0045c2c9  lea        edx, [esi - 1]
0045c2cc  shr        edx, 1
0045c2ce  lea        eax, [edi + 0x14]
0045c2d1  inc        edx
0045c2d2  push       ebx
0045c2d3  jmp        0x45c2d7

// block 0045c2d5
0045c2d5  fxch       st(1)

// block 0045c2d7
0045c2d7  fld        dword ptr [esp + 8]
0045c2db  add        eax, 0x38
0045c2de  sub        edx, 1
0045c2e1  fst        dword ptr [eax - 0x34]
0045c2e4  fxch       st(1)
0045c2e6  fst        dword ptr [eax - 0x38]
0045c2e9  mov        ebx, dword ptr [ecx + 0x3bc]
0045c2ef  fxch       st(2)
0045c2f1  mov        dword ptr [eax - 0x3c], ebx
0045c2f4  fst        dword ptr [eax - 0x40]
0045c2f7  fld        st(3)
0045c2f9  fsubp      st(2)
0045c2fb  fxch       st(1)
0045c2fd  fstp       dword ptr [esp + 8]
0045c301  jne        0x45c2d5

// block 0045c303
0045c303  fstp       st(1)
0045c305  pop        ebx

// block 0045c306
0045c306  cmp        esi, 1
0045c309  fld        dword ptr [ecx + 0x84]
0045c30f  fadd       dword ptr [ecx + 0x60]
0045c312  fstp       dword ptr [esp + 8]
0045c316  fxch       st(2)
0045c318  fstp       dword ptr [esp + 4]
0045c31c  jle        0x45c35d

// block 0045c31e
0045c31e  fld        dword ptr [esp + 8]
0045c322  lea        edx, [esi - 2]
0045c325  shr        edx, 1
0045c327  lea        eax, [edi + 0x30]
0045c32a  inc        edx
0045c32b  jmp        0x45c32f

// block 0045c32d
0045c32d  fxch       st(2)

// block 0045c32f
0045c32f  fld        dword ptr [esp + 4]
0045c333  add        eax, 0x38
0045c336  sub        edx, 1
0045c339  fst        dword ptr [eax - 0x34]
0045c33c  fxch       st(1)
0045c33e  fst        dword ptr [eax - 0x38]
0045c341  mov        esi, dword ptr [ecx + 0x3bc]
0045c347  fxch       st(3)
0045c349  mov        dword ptr [eax - 0x3c], esi
0045c34c  fst        dword ptr [eax - 0x40]
0045c34f  fld        st(2)
0045c351  fsubp      st(2)
0045c353  fxch       st(1)
0045c355  fstp       dword ptr [esp + 4]
0045c359  jne        0x45c32d

// block 0045c35b
0045c35b  fstp       st(1)

// block 0045c35d
0045c35d  fstp       st(0)
0045c35f  xor        eax, eax
0045c361  fstp       st(0)
0045c363  pop        esi
0045c364  add        esp, 0x10
0045c367  ret        
