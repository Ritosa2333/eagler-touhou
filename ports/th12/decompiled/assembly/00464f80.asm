
// block 00464f80
00464f80  jmp        dword ptr [eax*4 + 0x46523c]

// block 00464f87
00464f87  fld        dword ptr [esp + 4]
00464f8b  fmul       st(0), st(0)
00464f8d  fstp       dword ptr [esp + 4]
00464f91  fld        dword ptr [esp + 4]
00464f95  ret        4

// block 00464f98
00464f98  fld        dword ptr [esp + 4]
00464f9c  fld1       
00464f9e  fld        st(0)
00464fa0  fsubrp     st(2)
00464fa2  fld        st(1)
00464fa4  fmulp      st(2)
00464fa6  fsubrp     st(1)
00464fa8  fstp       dword ptr [esp + 4]
00464fac  fld        dword ptr [esp + 4]
00464fb0  ret        4

// block 00464fb3
00464fb3  fld        dword ptr [esp + 4]
00464fb7  fld        st(0)
00464fb9  fmul       st(0), st(0)
00464fbb  fmulp      st(1)
00464fbd  fstp       dword ptr [esp + 4]
00464fc1  fld        dword ptr [esp + 4]
00464fc5  ret        4

// block 00464fc8
00464fc8  fld        dword ptr [esp + 4]
00464fcc  fld1       
00464fce  fld        st(0)
00464fd0  fsubrp     st(2)
00464fd2  fld        st(1)
00464fd4  fmul       st(0), st(0)
00464fd6  fmulp      st(2)
00464fd8  fsubrp     st(1)
00464fda  fstp       dword ptr [esp + 4]
00464fde  fld        dword ptr [esp + 4]
00464fe2  ret        4

// block 00464fe5
00464fe5  fld        dword ptr [esp + 4]
00464fe9  fld        st(0)
00464feb  fmul       st(0), st(0)
00464fed  fmul       st(1)
00464fef  fmulp      st(1)
00464ff1  fstp       dword ptr [esp + 4]
00464ff5  fld        dword ptr [esp + 4]
00464ff9  ret        4

// block 00464ffc
00464ffc  fld        dword ptr [esp + 4]
00465000  fld1       
00465002  fld        st(0)
00465004  fsubrp     st(2)
00465006  fld        st(1)
00465008  fmul       st(0), st(0)
0046500a  fmul       st(2)
0046500c  fmulp      st(2)
0046500e  fsubrp     st(1)
00465010  fstp       dword ptr [esp + 4]
00465014  fld        dword ptr [esp + 4]
00465018  ret        4

// block 0046501b
0046501b  fld        dword ptr [esp + 4]
0046501f  fld        qword ptr [0x4a3d00]
00465025  fmul       st(1), st(0)
00465027  fxch       st(1)
00465029  fstp       dword ptr [esp + 4]
0046502d  fld1       
0046502f  fld        dword ptr [esp + 4]
00465033  fcom       st(1)
00465035  fnstsw     ax
00465037  fstp       st(1)
00465039  test       ah, 5
0046503c  jp         0x465053

// block 0046503e
0046503e  fstp       st(1)
00465040  fmul       st(0), st(0)
00465042  fmul       qword ptr [0x4a3cf8]
00465048  fstp       dword ptr [esp + 4]
0046504c  fld        dword ptr [esp + 4]
00465050  ret        4

// block 00465053
00465053  fsubr      st(1)
00465055  fmul       st(0), st(0)
00465057  fsubp      st(1)
00465059  fmul       qword ptr [0x4a3cf8]
0046505f  fstp       dword ptr [esp + 4]
00465063  fld        dword ptr [esp + 4]
00465067  ret        4

// block 0046506a
0046506a  fld        dword ptr [esp + 4]
0046506e  fadd       st(0), st(0)
00465070  fstp       dword ptr [esp + 4]
00465074  fld1       
00465076  fld        dword ptr [esp + 4]
0046507a  fcom       st(1)
0046507c  fnstsw     ax
0046507e  fstp       st(1)
00465080  test       ah, 5
00465083  jp         0x4650a0

// block 00465085
00465085  fld1       
00465087  fsubrp     st(1)
00465089  fmul       st(0), st(0)
0046508b  fld        qword ptr [0x4a3cf8]
00465091  fmul       st(1), st(0)
00465093  fsubrp     st(1)
00465095  fstp       dword ptr [esp + 4]
00465099  fld        dword ptr [esp + 4]
0046509d  ret        4

// block 004650a0
004650a0  fsub       qword ptr [0x4a3ce0]
004650a6  fmul       st(0), st(0)
004650a8  fld        qword ptr [0x4a3cf8]
004650ae  fmul       st(1), st(0)
004650b0  faddp      st(1)
004650b2  fstp       dword ptr [esp + 4]
004650b6  fld        dword ptr [esp + 4]
004650ba  ret        4

// block 004650bd
004650bd  fld        dword ptr [esp + 4]
004650c1  fld        qword ptr [0x4a3d00]
004650c7  fmul       st(1), st(0)
004650c9  fxch       st(1)
004650cb  fstp       dword ptr [esp + 4]
004650cf  fld1       
004650d1  fld        dword ptr [esp + 4]
004650d5  fcom       st(1)
004650d7  fnstsw     ax
004650d9  fstp       st(1)
004650db  test       ah, 5
004650de  jp         0x4650f9

// block 004650e0
004650e0  fstp       st(1)
004650e2  fld        st(0)
004650e4  fmul       st(0), st(0)
004650e6  fmulp      st(1)
004650e8  fmul       qword ptr [0x4a3cf8]
004650ee  fstp       dword ptr [esp + 4]
004650f2  fld        dword ptr [esp + 4]
004650f6  ret        4

// block 004650f9
004650f9  fsubr      st(1)
004650fb  fld        st(0)
004650fd  fmul       st(0), st(0)
004650ff  fmulp      st(1)
00465101  fsubp      st(1)
00465103  fmul       qword ptr [0x4a3cf8]
00465109  fstp       dword ptr [esp + 4]
0046510d  fld        dword ptr [esp + 4]
00465111  ret        4

// block 00465114
00465114  fld        dword ptr [esp + 4]
00465118  fadd       st(0), st(0)
0046511a  fstp       dword ptr [esp + 4]
0046511e  fld1       
00465120  fld        dword ptr [esp + 4]
00465124  fcom       st(1)
00465126  fnstsw     ax
00465128  fstp       st(1)
0046512a  test       ah, 5
0046512d  jp         0x46514e

// block 0046512f
0046512f  fld1       
00465131  fsubrp     st(1)
00465133  fld        st(0)
00465135  fmul       st(0), st(0)
00465137  fmulp      st(1)
00465139  fld        qword ptr [0x4a3cf8]
0046513f  fmul       st(1), st(0)
00465141  fsubrp     st(1)
00465143  fstp       dword ptr [esp + 4]
00465147  fld        dword ptr [esp + 4]
0046514b  ret        4

// block 0046514e
0046514e  fsub       qword ptr [0x4a3ce0]
00465154  fld        st(0)
00465156  fmul       st(0), st(0)
00465158  fmulp      st(1)
0046515a  fld        qword ptr [0x4a3cf8]
00465160  fmul       st(1), st(0)
00465162  faddp      st(1)
00465164  fstp       dword ptr [esp + 4]
00465168  fld        dword ptr [esp + 4]
0046516c  ret        4

// block 0046516f
0046516f  fld        dword ptr [esp + 4]
00465173  fld        qword ptr [0x4a3d00]
00465179  fmul       st(1), st(0)
0046517b  fxch       st(1)
0046517d  fstp       dword ptr [esp + 4]
00465181  fld1       
00465183  fld        dword ptr [esp + 4]
00465187  fcom       st(1)
00465189  fnstsw     ax
0046518b  fstp       st(1)
0046518d  test       ah, 5
00465190  jp         0x4651ad

// block 00465192
00465192  fstp       st(1)
00465194  fld        st(0)
00465196  fmul       st(0), st(0)
00465198  fmul       st(1)
0046519a  fmulp      st(1)
0046519c  fmul       qword ptr [0x4a3cf8]
004651a2  fstp       dword ptr [esp + 4]
004651a6  fld        dword ptr [esp + 4]
004651aa  ret        4

// block 004651ad
004651ad  fsubr      st(1)
004651af  fld        st(0)
004651b1  fmul       st(0), st(0)
004651b3  fmul       st(1)
004651b5  fmulp      st(1)
004651b7  fsubp      st(1)
004651b9  fmul       qword ptr [0x4a3cf8]
004651bf  fstp       dword ptr [esp + 4]
004651c3  fld        dword ptr [esp + 4]
004651c7  ret        4

// block 004651ca
004651ca  fld        dword ptr [esp + 4]
004651ce  fadd       st(0), st(0)
004651d0  fstp       dword ptr [esp + 4]
004651d4  fld1       
004651d6  fld        dword ptr [esp + 4]
004651da  fcom       st(1)
004651dc  fnstsw     ax
004651de  fstp       st(1)
004651e0  test       ah, 5
004651e3  jp         0x465206

// block 004651e5
004651e5  fld1       
004651e7  fsubrp     st(1)
004651e9  fld        st(0)
004651eb  fmul       st(0), st(0)
004651ed  fmul       st(1)
004651ef  fmulp      st(1)
004651f1  fld        qword ptr [0x4a3cf8]
004651f7  fmul       st(1), st(0)
004651f9  fsubrp     st(1)
004651fb  fstp       dword ptr [esp + 4]
004651ff  fld        dword ptr [esp + 4]
00465203  ret        4

// block 00465206
00465206  fsub       qword ptr [0x4a3ce0]
0046520c  fld        st(0)
0046520e  fmul       st(0), st(0)
00465210  fmul       st(1)
00465212  fmulp      st(1)
00465214  fld        qword ptr [0x4a3cf8]
0046521a  fmul       st(1), st(0)
0046521c  faddp      st(1)
0046521e  fstp       dword ptr [esp + 4]
00465222  fld        dword ptr [esp + 4]
00465226  ret        4

// block 00465229
00465229  fldz       
0046522b  ret        4

// block 0046522e
0046522e  fld1       
00465230  ret        4

// block 00465233
00465233  fld        dword ptr [esp + 4]
00465237  ret        4
