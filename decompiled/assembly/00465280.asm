
// block 00465280
00465280  fld        dword ptr [esp + 4]
00465284  fld        st(0)
00465286  fld        dword ptr [esp + 8]
0046528a  fld        st(0)
0046528c  fsubp      st(2)
0046528e  fld        qword ptr [0x4a3cd8]
00465294  fcom       st(2)
00465296  fnstsw     ax
00465298  test       ah, 5
0046529b  jp         0x4652b4

// block 0046529d
0046529d  fstp       st(2)
0046529f  fstp       st(1)
004652a1  fadd       qword ptr [0x4a3cd0]
004652a7  fsubp      st(1)
004652a9  fstp       dword ptr [esp + 4]
004652ad  fld        dword ptr [esp + 4]
004652b1  ret        8

// block 004652b4
004652b4  fld        st(1)
004652b6  fsub       st(4)
004652b8  fcompp     
004652ba  fnstsw     ax
004652bc  test       ah, 0x41
004652bf  jne        0x4652d6

// block 004652c1
004652c1  fstp       st(1)
004652c3  fsub       qword ptr [0x4a3cd0]
004652c9  fsubp      st(1)
004652cb  fstp       dword ptr [esp + 4]
004652cf  fld        dword ptr [esp + 4]
004652d3  ret        8

// block 004652d6
004652d6  fstp       st(2)
004652d8  fstp       st(1)
004652da  fstp       dword ptr [esp + 4]
004652de  fld        dword ptr [esp + 4]
004652e2  ret        8
