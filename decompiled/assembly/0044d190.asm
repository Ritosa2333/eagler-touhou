
// block 0044d190
0044d190  push       ebp
0044d191  mov        ebp, esp
0044d193  and        esp, 0xfffffff8
0044d196  or         dword ptr [0x4b0d18], 2
0044d19d  push       ebx
0044d19e  mov        ebx, 1
0044d1a3  push       esi
0044d1a4  test       byte ptr [0x4b0d18], bl
0044d1aa  jne        0x44d23f

// block 0044d1b0
0044d1b0  mov        esi, dword ptr [0x498084]
0044d1b6  jmp        0x44d1c4

// block 0044d1c0
0044d1c0  fstp       st(1)
0044d1c2  fstp       st(0)

// block 0044d1c4
0044d1c4  call       0x4508b0

// block 0044d1c9
0044d1c9  fst        qword ptr [0x4b0d1c]
0044d1cf  fcom       qword ptr [0x4b0d24]
0044d1d5  fnstsw     ax
0044d1d7  test       ah, 5
0044d1da  jp         0x44d1e6

// block 0044d1dc
0044d1dc  fld        st(0)
0044d1de  fst        qword ptr [0x4b0d2c]
0044d1e4  jmp        0x44d1ec

// block 0044d1e6
0044d1e6  fld        qword ptr [0x4b0d2c]

// block 0044d1ec
0044d1ec  fxch       st(1)
0044d1ee  fst        qword ptr [0x4b0d24]
0044d1f4  fcom       st(1)
0044d1f6  fnstsw     ax
0044d1f8  test       ah, 0x41
0044d1fb  jne        0x44d1c0

// block 0044d1fd
0044d1fd  fcom       st(1)
0044d1ff  fnstsw     ax
0044d201  test       ah, 0x41
0044d204  jne        0x44d229

// block 0044d206
0044d206  fld        qword ptr [0x4a3de8]
0044d20c  jmp        0x44d210

// block 0044d20e
0044d20e  fxch       st(2)

// block 0044d210
0044d210  fadd       st(2), st(0)
0044d212  fxch       st(2)
0044d214  fcom       st(1)
0044d216  fnstsw     ax
0044d218  test       ah, 5
0044d21b  jnp        0x44d20e

// block 0044d21d
0044d21d  fstp       st(1)
0044d21f  fstp       st(1)
0044d221  fstp       qword ptr [0x4b0d2c]
0044d227  jmp        0x44d22d

// block 0044d229
0044d229  fstp       st(1)
0044d22b  fstp       st(0)

// block 0044d22d
0044d22d  add        dword ptr [0x4b0d44], ebx
0044d233  push       0xd
0044d235  call       esi

// block 0044d237
0044d237  test       byte ptr [0x4b0d18], bl
0044d23d  je         0x44d1c4

// block 0044d23f
0044d23f  pop        esi
0044d240  pop        ebx
0044d241  mov        esp, ebp
0044d243  pop        ebp
0044d244  ret        
