
// block 0040d4f0
0040d4f0  fld        dword ptr [esp + 4]
0040d4f4  fld        st(0)
0040d4f6  fld        dword ptr [esp + 8]
0040d4fa  fld        st(0)
0040d4fc  fsubp      st(2)
0040d4fe  fld        qword ptr [0x4a3cd8]
0040d504  fcom       st(2)
0040d506  fnstsw     ax
0040d508  test       ah, 5
0040d50b  jp         0x40d524

// block 0040d50d
0040d50d  fstp       st(2)
0040d50f  fstp       st(1)
0040d511  fadd       qword ptr [0x4a3cd0]
0040d517  fsubp      st(1)
0040d519  fstp       dword ptr [esp + 4]
0040d51d  fld        dword ptr [esp + 4]
0040d521  ret        8

// block 0040d524
0040d524  fld        st(1)
0040d526  fsub       st(4)
0040d528  fcompp     
0040d52a  fnstsw     ax
0040d52c  test       ah, 0x41
0040d52f  jne        0x40d546

// block 0040d531
0040d531  fstp       st(1)
0040d533  fsub       qword ptr [0x4a3cd0]
0040d539  fsubp      st(1)
0040d53b  fstp       dword ptr [esp + 4]
0040d53f  fld        dword ptr [esp + 4]
0040d543  ret        8

// block 0040d546
0040d546  fstp       st(2)
0040d548  fstp       st(1)
0040d54a  fstp       dword ptr [esp + 4]
0040d54e  fld        dword ptr [esp + 4]
0040d552  ret        8
