
// block 0040c0b0
0040c0b0  mov        edx, dword ptr [edi + 0x3fc]
0040c0b6  fld        dword ptr [edx + 0x34]
0040c0b9  sub        esp, 8
0040c0bc  fstp       dword ptr [esp + 4]
0040c0c0  lea        ecx, [edi + 0x4bc]
0040c0c6  fld        dword ptr [edx + 0x38]
0040c0c9  fstp       dword ptr [esp]
0040c0cc  call       0x409970

// block 0040c0d1
0040c0d1  test       eax, eax
0040c0d3  je         0x40c164

// block 0040c0d9
0040c0d9  fldz       
0040c0db  push       esi
0040c0dc  fcomp      dword ptr [edi + 0x4c0]
0040c0e2  fnstsw     ax
0040c0e4  test       ah, 0x41
0040c0e7  jne        0x40c124

// block 0040c0e9
0040c0e9  fld        dword ptr [edx + 0x34]
0040c0ec  fadd       qword ptr [0x4a3d50]
0040c0f2  fadd       dword ptr [edi + 0x4c0]

// block 0040c0f8
0040c0f8  fstp       dword ptr [edi + 0x4c0]
0040c0fe  push       ecx
0040c0ff  fld        dword ptr [0x4a3da8]
0040c105  lea        esi, [edi + 0x86c]
0040c10b  fstp       dword ptr [esp]
0040c10e  call       0x464a20

// block 0040c113
0040c113  mov        edx, dword ptr [edi + 0x544]
0040c119  test       edx, edx
0040c11b  jl         0x40c14a

// block 0040c11d
0040c11d  call       0x453d90

// block 0040c122
0040c122  jmp        0x40c14a

// block 0040c124
0040c124  fld        dword ptr [edi + 0x4c0]
0040c12a  fld        qword ptr [0x4a3d50]
0040c130  fcom       st(1)
0040c132  fnstsw     ax
0040c134  fstp       st(1)
0040c136  test       ah, 5
0040c139  jp         0x40c148

// block 0040c13b
0040c13b  fld        dword ptr [edx + 0x34]
0040c13e  faddp      st(1)
0040c140  fsubr      dword ptr [edi + 0x4c0]
0040c146  jmp        0x40c0f8

// block 0040c148
0040c148  fstp       st(0)

// block 0040c14a
0040c14a  cmp        dword ptr [edi + 0x870], 0
0040c151  pop        esi
0040c152  jg         0x40c164

// block 0040c154
0040c154  xor        dword ptr [edi + 0x528], 0x40000
0040c15e  mov        eax, 1
0040c163  ret        

// block 0040c164
0040c164  xor        eax, eax
0040c166  ret        
