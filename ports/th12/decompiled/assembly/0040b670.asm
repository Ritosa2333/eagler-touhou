
// block 0040b670
0040b670  push       ecx
0040b671  cmp        dword ptr [edx + 0x704], 0x10
0040b678  jg         0x40b6d4

// block 0040b67a
0040b67a  fld        dword ptr [edx + 0x708]
0040b680  push       esi
0040b681  fld        qword ptr [0x4a3fd0]
0040b687  sub        esp, 8
0040b68a  fmul       st(1), st(0)
0040b68c  lea        ecx, [edx + 0x4c8]
0040b692  fxch       st(1)
0040b694  fmul       qword ptr [0x4a4038]
0040b69a  fsubp      st(1)
0040b69c  fstp       dword ptr [esp + 0xc]
0040b6a0  fld        dword ptr [esp + 0xc]
0040b6a4  fadd       dword ptr [edx + 0x4d4]
0040b6aa  fstp       dword ptr [esp + 0xc]
0040b6ae  fld        dword ptr [esp + 0xc]
0040b6b2  fstp       dword ptr [esp + 4]
0040b6b6  fld        dword ptr [edx + 0x4d8]
0040b6bc  fstp       dword ptr [esp]
0040b6bf  call       0x40d640

// block 0040b6c4
0040b6c4  lea        esi, [edx + 0x700]
0040b6ca  call       0x464a80

// block 0040b6cf
0040b6cf  xor        eax, eax
0040b6d1  pop        esi
0040b6d2  pop        ecx
0040b6d3  ret        

// block 0040b6d4
0040b6d4  mov        eax, 1
0040b6d9  xor        dword ptr [edx + 0x528], eax
0040b6df  pop        ecx
0040b6e0  ret        
