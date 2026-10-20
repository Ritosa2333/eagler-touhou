
// block 004087b0
004087b0  fld        dword ptr [esp + 4]
004087b4  fld        st(0)
004087b6  fld        dword ptr [esp + 8]
004087ba  fld        st(0)
004087bc  fsubp      st(2)
004087be  fld        qword ptr [0x4a3cd8]
004087c4  fcom       st(2)
004087c6  fnstsw     ax
004087c8  test       ah, 5
004087cb  jp         0x4087e4

// block 004087cd
004087cd  fstp       st(2)
004087cf  fstp       st(1)
004087d1  fadd       qword ptr [0x4a3cd0]
004087d7  fsubp      st(1)
004087d9  fstp       dword ptr [esp + 4]
004087dd  fld        dword ptr [esp + 4]
004087e1  ret        8

// block 004087e4
004087e4  fld        st(1)
004087e6  fsub       st(4)
004087e8  fcompp     
004087ea  fnstsw     ax
004087ec  test       ah, 0x41
004087ef  jne        0x408806

// block 004087f1
004087f1  fstp       st(1)
004087f3  fsub       qword ptr [0x4a3cd0]
004087f9  fsubp      st(1)
004087fb  fstp       dword ptr [esp + 4]
004087ff  fld        dword ptr [esp + 4]
00408803  ret        8

// block 00408806
00408806  fstp       st(2)
00408808  fstp       st(1)
0040880a  fstp       dword ptr [esp + 4]
0040880e  fld        dword ptr [esp + 4]
00408812  ret        8
