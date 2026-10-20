
// block 0042e770
0042e770  fld        dword ptr [esp + 4]
0042e774  fld        st(0)
0042e776  fld        dword ptr [esp + 8]
0042e77a  fld        st(0)
0042e77c  fsubp      st(2)
0042e77e  fld        qword ptr [0x4a3cd8]
0042e784  fcom       st(2)
0042e786  fnstsw     ax
0042e788  test       ah, 5
0042e78b  jp         0x42e7a4

// block 0042e78d
0042e78d  fstp       st(2)
0042e78f  fstp       st(1)
0042e791  fadd       qword ptr [0x4a3cd0]
0042e797  fsubp      st(1)
0042e799  fstp       dword ptr [esp + 4]
0042e79d  fld        dword ptr [esp + 4]
0042e7a1  ret        8

// block 0042e7a4
0042e7a4  fld        st(1)
0042e7a6  fsub       st(4)
0042e7a8  fcompp     
0042e7aa  fnstsw     ax
0042e7ac  test       ah, 0x41
0042e7af  jne        0x42e7c6

// block 0042e7b1
0042e7b1  fstp       st(1)
0042e7b3  fsub       qword ptr [0x4a3cd0]
0042e7b9  fsubp      st(1)
0042e7bb  fstp       dword ptr [esp + 4]
0042e7bf  fld        dword ptr [esp + 4]
0042e7c3  ret        8

// block 0042e7c6
0042e7c6  fstp       st(2)
0042e7c8  fstp       st(1)
0042e7ca  fstp       dword ptr [esp + 4]
0042e7ce  fld        dword ptr [esp + 4]
0042e7d2  ret        8
