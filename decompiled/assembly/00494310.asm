
// block 00494310
00494310  fld        xword ptr [esp + 0x10]
00494314  fld        xword ptr [esp + 4]

// block 00494318
00494318  mov        eax, dword ptr [esp + 8]
0049431c  add        eax, eax
0049431e  jae        0x4943aa

// block 00494324
00494324  xor        eax, 0xe000000
00494329  test       eax, 0xe000000
0049432e  je         0x494333

// block 00494330
00494330  fdivp      st(1)
00494332  ret        

// block 00494333
00494333  shr        eax, 0x1c
00494336  cmp        byte ptr [eax + 0x4b35f0], 0
0049433d  jne        0x494342

// block 0049433f
0049433f  fdivp      st(1)
00494341  ret        

// block 00494342
00494342  mov        eax, dword ptr [esp + 0xc]
00494346  and        eax, 0x7fff
0049434b  je         0x4943b4

// block 0049434d
0049434d  cmp        eax, 0x7fff
00494352  je         0x4943b4

// block 00494354
00494354  fnstcw     word ptr [esp + 0x1c]
00494358  mov        eax, dword ptr [esp + 0x1c]
0049435c  or         eax, 0x33f
00494361  and        eax, 0xf3ff
00494366  mov        dword ptr [esp + 0x20], eax
0049436a  fldcw      word ptr [esp + 0x20]
0049436e  mov        eax, dword ptr [esp + 0x18]
00494372  and        eax, 0x7fff
00494377  cmp        eax, 1
0049437a  je         0x494393

// block 0049437c
0049437c  fmul       dword ptr [0x4b3600]
00494382  fxch       st(1)
00494384  fmul       dword ptr [0x4b3600]
0049438a  fxch       st(1)
0049438c  fldcw      word ptr [esp + 0x1c]
00494390  fdivp      st(1)
00494392  ret        

// block 00494393
00494393  fmul       dword ptr [0x4b3604]
00494399  fxch       st(1)
0049439b  fmul       dword ptr [0x4b3604]
004943a1  fxch       st(1)
004943a3  fldcw      word ptr [esp + 0x1c]
004943a7  fdivp      st(1)
004943a9  ret        

// block 004943aa
004943aa  mov        eax, dword ptr [esp + 4]
004943ae  or         eax, dword ptr [esp + 8]
004943b2  jne        0x4943b7

// block 004943b4
004943b4  fdivp      st(1)
004943b6  ret        

// block 004943b7
004943b7  mov        eax, dword ptr [esp + 0xc]
004943bb  and        eax, 0x7fff
004943c0  jne        0x4943b4

// block 004943c2
004943c2  fnstcw     word ptr [esp + 0x1c]
004943c6  mov        eax, dword ptr [esp + 0x1c]
004943ca  or         eax, 0x33f
004943cf  and        eax, 0xf3ff
004943d4  mov        dword ptr [esp + 0x20], eax
004943d8  fldcw      word ptr [esp + 0x20]
004943dc  mov        eax, dword ptr [esp + 0x18]
004943e0  and        eax, 0x7fff
004943e5  je         0x4943f8

// block 004943e7
004943e7  cmp        eax, 0x7fff
004943ec  je         0x494420

// block 004943ee
004943ee  mov        eax, dword ptr [esp + 0x14]
004943f2  add        eax, eax
004943f4  jae        0x494420

// block 004943f6
004943f6  jmp        0x494400

// block 004943f8
004943f8  mov        eax, dword ptr [esp + 0x14]
004943fc  add        eax, eax
004943fe  jb         0x494420

// block 00494400
00494400  fxch       st(1)
00494402  fstp       st(0)
00494404  fld        st(0)
00494406  fmul       dword ptr [0x4b3608]
0049440c  fstp       xword ptr [esp + 4]
00494410  fld        xword ptr [esp + 0x10]
00494414  fxch       st(1)
00494416  wait       
00494417  fldcw      word ptr [esp + 0x1c]
0049441b  jmp        0x494318

// block 00494420
00494420  fldcw      word ptr [esp + 0x1c]
00494424  fdivp      st(1)
00494426  ret        
