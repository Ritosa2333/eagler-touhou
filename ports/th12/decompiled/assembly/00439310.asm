
// block 00439310
00439310  fld        dword ptr [esp + 4]
00439314  fld        st(0)
00439316  fld        dword ptr [esp + 8]
0043931a  fld        st(0)
0043931c  fsubp      st(2)
0043931e  fld        qword ptr [0x4a3cd8]
00439324  fcom       st(2)
00439326  fnstsw     ax
00439328  test       ah, 5
0043932b  jp         0x439344

// block 0043932d
0043932d  fstp       st(2)
0043932f  fstp       st(1)
00439331  fadd       qword ptr [0x4a3cd0]
00439337  fsubp      st(1)
00439339  fstp       dword ptr [esp + 4]
0043933d  fld        dword ptr [esp + 4]
00439341  ret        8

// block 00439344
00439344  fld        st(1)
00439346  fsub       st(4)
00439348  fcompp     
0043934a  fnstsw     ax
0043934c  test       ah, 0x41
0043934f  jne        0x439366

// block 00439351
00439351  fstp       st(1)
00439353  fsub       qword ptr [0x4a3cd0]
00439359  fsubp      st(1)
0043935b  fstp       dword ptr [esp + 4]
0043935f  fld        dword ptr [esp + 4]
00439363  ret        8

// block 00439366
00439366  fstp       st(2)
00439368  fstp       st(1)
0043936a  fstp       dword ptr [esp + 4]
0043936e  fld        dword ptr [esp + 4]
00439372  ret        8
