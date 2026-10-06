
// block 0042c080
0042c080  push       ebp
0042c081  mov        ebp, esp
0042c083  and        esp, 0xfffffff8
0042c086  sub        esp, 0x24
0042c089  mov        eax, dword ptr [ebp + 8]
0042c08c  fld        dword ptr [eax]
0042c08e  push       esi
0042c08f  mov        esi, ecx
0042c091  fsub       dword ptr [esi + 0x50]
0042c094  fstp       dword ptr [esp + 0x10]
0042c098  fld        dword ptr [eax + 4]
0042c09b  fsub       dword ptr [esi + 0x54]
0042c09e  fstp       dword ptr [esp + 0x14]
0042c0a2  fld        dword ptr [esi + 0x68]
0042c0a5  fchs       
0042c0a7  fstp       dword ptr [esp + 4]
0042c0ab  fld        dword ptr [esp + 4]
0042c0af  call       0x4938c0

// block 0042c0b4
0042c0b4  fstp       dword ptr [esp + 8]
0042c0b8  fld        dword ptr [esp + 8]
0042c0bc  fstp       dword ptr [esp + 0xc]
0042c0c0  fld        dword ptr [esp + 4]
0042c0c4  call       0x4939f0

// block 0042c0c9
0042c0c9  fstp       dword ptr [esp + 8]
0042c0cd  fld        dword ptr [esp + 8]
0042c0d1  fstp       dword ptr [esp + 8]
0042c0d5  fld        dword ptr [esp + 8]
0042c0d9  fld        st(0)
0042c0db  fld        dword ptr [esp + 0x10]
0042c0df  fld        st(0)
0042c0e1  fmulp      st(2)
0042c0e3  fld        dword ptr [esp + 0xc]
0042c0e7  fld        st(0)
0042c0e9  fld        dword ptr [esp + 0x14]
0042c0ed  fld        st(0)
0042c0ef  fmulp      st(2)
0042c0f1  fxch       st(4)
0042c0f3  fsubrp     st(1)
0042c0f5  fstp       dword ptr [esp + 0x10]
0042c0f9  fmulp      st(1)
0042c0fb  fxch       st(2)
0042c0fd  fmulp      st(1)
0042c0ff  faddp      st(1)
0042c101  fstp       dword ptr [esp + 0x14]
0042c105  fld        dword ptr [esp + 0x10]
0042c109  fld        st(0)
0042c10b  fld        dword ptr [ebp + 0xc]
0042c10e  fld        st(0)
0042c110  fsubp      st(2)
0042c112  fxch       st(1)
0042c114  fstp       dword ptr [esp + 0x1c]
0042c118  fld        dword ptr [esp + 0x14]
0042c11c  fld        st(0)
0042c11e  fsub       st(2)
0042c120  fstp       dword ptr [esp + 0x20]
0042c124  fld        st(1)
0042c126  faddp      st(3)
0042c128  fxch       st(2)
0042c12a  fstp       dword ptr [esp + 0x10]
0042c12e  faddp      st(1)
0042c130  fstp       dword ptr [esp + 0x14]
0042c134  fld        dword ptr [esp + 0x1c]
0042c138  fld        dword ptr [esi + 0x6c]
0042c13b  fcompp     
0042c13d  fnstsw     ax
0042c13f  test       ah, 5
0042c142  jnp        0x42c18d

// block 0042c144
0042c144  fld        dword ptr [esp + 0x20]
0042c148  fld        dword ptr [esi + 0x70]
0042c14b  fld        qword ptr [0x4a3cf8]
0042c151  fmul       st(1), st(0)
0042c153  fxch       st(2)
0042c155  fcompp     
0042c157  fnstsw     ax
0042c159  test       ah, 0x41
0042c15c  je         0x42c18b

// block 0042c15e
0042c15e  fldz       
0042c160  fcomp      dword ptr [esp + 0x10]
0042c164  fnstsw     ax
0042c166  test       ah, 0x41
0042c169  je         0x42c18b

// block 0042c16b
0042c16b  fld        dword ptr [esp + 0x14]
0042c16f  fld        dword ptr [esi + 0x70]
0042c172  fchs       
0042c174  fmulp      st(2)
0042c176  fcompp     
0042c178  fnstsw     ax
0042c17a  test       ah, 5
0042c17d  jnp        0x42c18d

// block 0042c17f
0042c17f  mov        eax, 2
0042c184  pop        esi
0042c185  mov        esp, ebp
0042c187  pop        ebp
0042c188  ret        8

// block 0042c18b
0042c18b  fstp       st(0)

// block 0042c18d
0042c18d  xor        eax, eax
0042c18f  pop        esi
0042c190  mov        esp, ebp
0042c192  pop        ebp
0042c193  ret        8
