
// block 0043ace0
0043ace0  fld        dword ptr [esp + 4]
0043ace4  fld        st(0)
0043ace6  fld        dword ptr [esp + 8]
0043acea  fld        st(0)
0043acec  fsubp      st(2)
0043acee  fld        qword ptr [0x4a3cd8]
0043acf4  fcom       st(2)
0043acf6  fnstsw     ax
0043acf8  test       ah, 5
0043acfb  jp         0x43ad14

// block 0043acfd
0043acfd  fstp       st(2)
0043acff  fstp       st(1)
0043ad01  fadd       qword ptr [0x4a3cd0]
0043ad07  fsubp      st(1)
0043ad09  fstp       dword ptr [esp + 4]
0043ad0d  fld        dword ptr [esp + 4]
0043ad11  ret        8

// block 0043ad14
0043ad14  fld        st(1)
0043ad16  fsub       st(4)
0043ad18  fcompp     
0043ad1a  fnstsw     ax
0043ad1c  test       ah, 0x41
0043ad1f  jne        0x43ad36

// block 0043ad21
0043ad21  fstp       st(1)
0043ad23  fsub       qword ptr [0x4a3cd0]
0043ad29  fsubp      st(1)
0043ad2b  fstp       dword ptr [esp + 4]
0043ad2f  fld        dword ptr [esp + 4]
0043ad33  ret        8

// block 0043ad36
0043ad36  fstp       st(2)
0043ad38  fstp       st(1)
0043ad3a  fstp       dword ptr [esp + 4]
0043ad3e  fld        dword ptr [esp + 4]
0043ad42  ret        8
