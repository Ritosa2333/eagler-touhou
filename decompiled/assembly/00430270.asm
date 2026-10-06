
// block 00430270
00430270  fldz       
00430272  fld        dword ptr [0x4b2ed0]
00430278  fucom      st(1)
0043027a  fnstsw     ax
0043027c  fstp       st(1)
0043027e  test       ah, 0x44
00430281  jp         0x43028b

// block 00430283
00430283  fstp       st(0)
00430285  fld        dword ptr [esp + 4]
00430289  jmp        0x43029a

// block 0043028b
0043028b  fld1       
0043028d  fcomp      st(1)
0043028f  fnstsw     ax
00430291  test       ah, 5
00430294  jnp        0x430283

// block 00430296
00430296  fdivr      dword ptr [esp + 4]

// block 0043029a
0043029a  fstp       dword ptr [esp + 4]
0043029e  push       edi
0043029f  fld        dword ptr [esp + 8]
004302a3  call       0x4931e0

// block 004302a8
004302a8  push       eax
004302a9  push       5
004302ab  mov        edi, 0x49fe19
004302b0  mov        eax, 0x4cf4e8
004302b5  call       0x454960

// block 004302ba
004302ba  xor        eax, eax
004302bc  pop        edi
004302bd  ret        4
