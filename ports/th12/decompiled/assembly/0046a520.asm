
// block 0046a520
0046a520  fld        dword ptr [esp + 4]
0046a524  fld        st(0)
0046a526  fld        dword ptr [esp + 8]
0046a52a  fld        st(0)
0046a52c  fsubp      st(2)
0046a52e  fld        qword ptr [0x4a3cd8]
0046a534  fcom       st(2)
0046a536  fnstsw     ax
0046a538  test       ah, 5
0046a53b  jp         0x46a554

// block 0046a53d
0046a53d  fstp       st(2)
0046a53f  fstp       st(1)
0046a541  fadd       qword ptr [0x4a3cd0]
0046a547  fsubp      st(1)
0046a549  fstp       dword ptr [esp + 4]
0046a54d  fld        dword ptr [esp + 4]
0046a551  ret        8

// block 0046a554
0046a554  fld        st(1)
0046a556  fsub       st(4)
0046a558  fcompp     
0046a55a  fnstsw     ax
0046a55c  test       ah, 0x41
0046a55f  jne        0x46a576

// block 0046a561
0046a561  fstp       st(1)
0046a563  fsub       qword ptr [0x4a3cd0]
0046a569  fsubp      st(1)
0046a56b  fstp       dword ptr [esp + 4]
0046a56f  fld        dword ptr [esp + 4]
0046a573  ret        8

// block 0046a576
0046a576  fstp       st(2)
0046a578  fstp       st(1)
0046a57a  fstp       dword ptr [esp + 4]
0046a57e  fld        dword ptr [esp + 4]
0046a582  ret        8
