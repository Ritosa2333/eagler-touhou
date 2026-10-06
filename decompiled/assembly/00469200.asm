
// block 00469200
00469200  fld        dword ptr [esp + 4]
00469204  fld        st(0)
00469206  fld        dword ptr [esp + 8]
0046920a  fld        st(0)
0046920c  fsubp      st(2)
0046920e  fld        qword ptr [0x4a3cd8]
00469214  fcom       st(2)
00469216  fnstsw     ax
00469218  test       ah, 5
0046921b  jp         0x469234

// block 0046921d
0046921d  fstp       st(2)
0046921f  fstp       st(1)
00469221  fadd       qword ptr [0x4a3cd0]
00469227  fsubp      st(1)
00469229  fstp       dword ptr [esp + 4]
0046922d  fld        dword ptr [esp + 4]
00469231  ret        8

// block 00469234
00469234  fld        st(1)
00469236  fsub       st(4)
00469238  fcompp     
0046923a  fnstsw     ax
0046923c  test       ah, 0x41
0046923f  jne        0x469256

// block 00469241
00469241  fstp       st(1)
00469243  fsub       qword ptr [0x4a3cd0]
00469249  fsubp      st(1)
0046924b  fstp       dword ptr [esp + 4]
0046924f  fld        dword ptr [esp + 4]
00469253  ret        8

// block 00469256
00469256  fstp       st(2)
00469258  fstp       st(1)
0046925a  fstp       dword ptr [esp + 4]
0046925e  fld        dword ptr [esp + 4]
00469262  ret        8
