
// block 004086b0
004086b0  fld        dword ptr [esp + 0xc]
004086b4  fld        qword ptr [0x4a3cf8]
004086ba  fmul       st(1), st(0)
004086bc  fld        dword ptr [ecx]
004086be  fld        dword ptr [esp + 4]
004086c2  fld        st(0)
004086c4  fsub       st(4)
004086c6  fcomp      st(2)
004086c8  fnstsw     ax
004086ca  fstp       st(1)
004086cc  test       ah, 0x41
004086cf  je         0x408711

// block 004086d1
004086d1  fld        dword ptr [ecx]
004086d3  fxch       st(1)
004086d5  faddp      st(3)
004086d7  fcomp      st(2)
004086d9  fnstsw     ax
004086db  fstp       st(1)
004086dd  test       ah, 0x41
004086e0  je         0x408721

// block 004086e2
004086e2  fmul       dword ptr [esp + 0x10]
004086e6  fld        dword ptr [ecx + 4]
004086e9  fld        dword ptr [esp + 8]
004086ed  fld        st(0)
004086ef  fsub       st(3)
004086f1  fcomp      st(2)
004086f3  fnstsw     ax
004086f5  fstp       st(1)
004086f7  test       ah, 0x41
004086fa  je         0x40871f

// block 004086fc
004086fc  fld        dword ptr [ecx + 4]
004086ff  fxch       st(1)
00408701  faddp      st(2)
00408703  fcompp     
00408705  fnstsw     ax
00408707  test       ah, 0x41
0040870a  je         0x408723

// block 0040870c
0040870c  xor        eax, eax
0040870e  ret        0x10

// block 00408711
00408711  fstp       st(2)
00408713  mov        eax, 1
00408718  fstp       st(0)
0040871a  fstp       st(0)
0040871c  ret        0x10

// block 0040871f
0040871f  fstp       st(1)

// block 00408721
00408721  fstp       st(0)

// block 00408723
00408723  mov        eax, 1
00408728  ret        0x10
