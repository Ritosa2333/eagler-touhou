
// block 0041a000
0041a000  mov        eax, dword ptr [esp + 4]
0041a004  add        eax, 0x2710
0041a009  push       esi
0041a00a  cmp        eax, 0x46
0041a00d  ja         0x41a431

// block 0041a013
0041a013  jmp        dword ptr [eax*4 + 0x41a438]

// block 0041a01a
0041a01a  mov        esi, 0x4ce568
0041a01f  call       0x464440

// block 0041a024
0041a024  mov        dword ptr [esp + 8], eax
0041a028  fild       dword ptr [esp + 8]
0041a02c  test       eax, eax
0041a02e  jge        0x41a433

// block 0041a034
0041a034  fadd       dword ptr [0x4a3e10]
0041a03a  pop        esi
0041a03b  ret        4

// block 0041a03e
0041a03e  mov        esi, 0x4ce568
0041a043  call       0x4645b0

// block 0041a048
0041a048  pop        esi
0041a049  ret        4

// block 0041a04c
0041a04c  mov        esi, 0x4ce568
0041a051  call       0x4645e0

// block 0041a056
0041a056  pop        esi
0041a057  ret        4

// block 0041a05a
0041a05a  mov        esi, 0x4ce568
0041a05f  call       0x4645e0

// block 0041a064
0041a064  fmul       qword ptr [0x4a3cd8]
0041a06a  pop        esi
0041a06b  fstp       dword ptr [esp + 4]
0041a06f  fld        dword ptr [esp + 4]
0041a073  ret        4

// block 0041a076
0041a076  fld        dword ptr [ecx + 0x1074]
0041a07c  pop        esi
0041a07d  ret        4

// block 0041a080
0041a080  fld        dword ptr [ecx + 0x1078]
0041a086  pop        esi
0041a087  ret        4

// block 0041a08a
0041a08a  fld        dword ptr [ecx + 0x10a8]
0041a090  pop        esi
0041a091  ret        4

// block 0041a094
0041a094  fld        dword ptr [ecx + 0x10ac]
0041a09a  pop        esi
0041a09b  ret        4

// block 0041a09e
0041a09e  fld        dword ptr [ecx + 0x10dc]
0041a0a4  pop        esi
0041a0a5  ret        4

// block 0041a0a8
0041a0a8  fld        dword ptr [ecx + 0x10e0]
0041a0ae  pop        esi
0041a0af  ret        4

// block 0041a0b2
0041a0b2  mov        eax, dword ptr [0x4b4514]
0041a0b7  fld        dword ptr [eax + 0x97c]
0041a0bd  pop        esi
0041a0be  ret        4

// block 0041a0c1
0041a0c1  mov        ecx, dword ptr [0x4b4514]
0041a0c7  fld        dword ptr [ecx + 0x980]
0041a0cd  pop        esi
0041a0ce  ret        4

// block 0041a0d1
0041a0d1  add        ecx, 0x1074
0041a0d7  call       0x4377a0

// block 0041a0dc
0041a0dc  pop        esi
0041a0dd  ret        4

// block 0041a0e0
0041a0e0  fld        dword ptr [ecx + 0x12b4]
0041a0e6  pop        esi
0041a0e7  ret        4

// block 0041a0ea
0041a0ea  mov        edx, dword ptr [ecx + 0x26f8]
0041a0f0  shr        edx, 0x17
0041a0f3  and        edx, 1
0041a0f6  mov        dword ptr [esp + 8], edx
0041a0fa  fild       dword ptr [esp + 8]
0041a0fe  test       edx, edx
0041a100  jge        0x41a433

// block 0041a106
0041a106  fadd       dword ptr [0x4a3e10]
0041a10c  pop        esi
0041a10d  ret        4

// block 0041a110
0041a110  fild       dword ptr [ecx + 0x1280]
0041a116  pop        esi
0041a117  ret        4

// block 0041a11a
0041a11a  fild       dword ptr [ecx + 0x1284]
0041a120  pop        esi
0041a121  ret        4

// block 0041a124
0041a124  fld        dword ptr [ecx + 0x128c]
0041a12a  pop        esi
0041a12b  ret        4

// block 0041a12e
0041a12e  fld        dword ptr [ecx + 0x1290]
0041a134  pop        esi
0041a135  ret        4

// block 0041a138
0041a138  fld        dword ptr [ecx + 0x1298]
0041a13e  pop        esi
0041a13f  ret        4

// block 0041a142
0041a142  fld        dword ptr [ecx + 0x129c]
0041a148  pop        esi
0041a149  ret        4

// block 0041a14c
0041a14c  fld        dword ptr [ecx + 0x12a0]
0041a152  pop        esi
0041a153  ret        4

// block 0041a156
0041a156  fld        dword ptr [ecx + 0x12a4]
0041a15c  pop        esi
0041a15d  ret        4

// block 0041a160
0041a160  fld        dword ptr [ecx + 0x12a8]
0041a166  pop        esi
0041a167  ret        4

// block 0041a16a
0041a16a  mov        eax, dword ptr [0x4b43dc]
0041a16f  mov        ecx, dword ptr [eax + 0x1c]

// block 0041a172
0041a172  fild       dword ptr [ecx + 0x127c]
0041a178  pop        esi
0041a179  ret        4

// block 0041a17c
0041a17c  mov        edx, dword ptr [0x4b43dc]
0041a182  mov        eax, dword ptr [edx + 0x1c]
0041a185  fild       dword ptr [eax + 0x1280]
0041a18b  pop        esi
0041a18c  ret        4

// block 0041a18f
0041a18f  mov        ecx, dword ptr [0x4b43dc]
0041a195  mov        edx, dword ptr [ecx + 0x1c]
0041a198  fild       dword ptr [edx + 0x1284]
0041a19e  pop        esi
0041a19f  ret        4

// block 0041a1a2
0041a1a2  mov        eax, dword ptr [0x4b43dc]
0041a1a7  mov        ecx, dword ptr [eax + 0x1c]

// block 0041a1aa
0041a1aa  fild       dword ptr [ecx + 0x1288]
0041a1b0  pop        esi
0041a1b1  ret        4

// block 0041a1b4
0041a1b4  mov        edx, dword ptr [0x4b43dc]
0041a1ba  mov        eax, dword ptr [edx + 0x1c]
0041a1bd  fld        dword ptr [eax + 0x128c]
0041a1c3  pop        esi
0041a1c4  ret        4

// block 0041a1c7
0041a1c7  mov        ecx, dword ptr [0x4b43dc]
0041a1cd  mov        edx, dword ptr [ecx + 0x1c]
0041a1d0  fld        dword ptr [edx + 0x1290]
0041a1d6  pop        esi
0041a1d7  ret        4

// block 0041a1da
0041a1da  mov        eax, dword ptr [0x4b43dc]
0041a1df  mov        ecx, dword ptr [eax + 0x1c]

// block 0041a1e2
0041a1e2  fld        dword ptr [ecx + 0x1294]
0041a1e8  pop        esi
0041a1e9  ret        4

// block 0041a1ec
0041a1ec  mov        edx, dword ptr [0x4b43dc]
0041a1f2  mov        eax, dword ptr [edx + 0x1c]
0041a1f5  fld        dword ptr [eax + 0x1298]
0041a1fb  pop        esi
0041a1fc  ret        4

// block 0041a1ff
0041a1ff  fld        dword ptr [ecx + 0x10c4]
0041a205  pop        esi
0041a206  ret        4

// block 0041a209
0041a209  fld        dword ptr [ecx + 0x10f8]
0041a20f  pop        esi
0041a210  ret        4

// block 0041a213
0041a213  lea        eax, [ecx + 0x1074]
0041a219  call       0x41c530

// block 0041a21e
0041a21e  pop        esi
0041a21f  ret        4

// block 0041a222
0041a222  fld        dword ptr [ecx + 0x10c0]
0041a228  pop        esi
0041a229  ret        4

// block 0041a22c
0041a22c  fld        dword ptr [ecx + 0x10f4]
0041a232  pop        esi
0041a233  ret        4

// block 0041a236
0041a236  fld        dword ptr [ecx + 0x10c8]
0041a23c  pop        esi
0041a23d  ret        4

// block 0041a240
0041a240  fld        dword ptr [ecx + 0x10fc]
0041a246  pop        esi
0041a247  ret        4

// block 0041a24a
0041a24a  mov        ecx, dword ptr [0x4b4514]
0041a250  fld        dword ptr [ecx + 0x97c]
0041a256  pop        esi
0041a257  ret        4

// block 0041a25a
0041a25a  mov        edx, dword ptr [0x4b4514]
0041a260  fld        dword ptr [edx + 0x980]
0041a266  pop        esi
0041a267  ret        4

// block 0041a26a
0041a26a  mov        eax, dword ptr [0x4b43dc]
0041a26f  mov        eax, dword ptr [eax + 0x1c]
0041a272  test       eax, eax
0041a274  je         0x41a288

// block 0041a276
0041a276  fld        dword ptr [eax + 0x1074]
0041a27c  pop        esi
0041a27d  fstp       dword ptr [esp + 4]
0041a281  fld        dword ptr [esp + 4]
0041a285  ret        4

// block 0041a288
0041a288  fldz       
0041a28a  pop        esi
0041a28b  fstp       dword ptr [esp + 4]
0041a28f  fld        dword ptr [esp + 4]
0041a293  ret        4

// block 0041a296
0041a296  mov        ecx, dword ptr [0x4b43dc]
0041a29c  mov        eax, dword ptr [ecx + 0x1c]
0041a29f  test       eax, eax
0041a2a1  je         0x41a2b5

// block 0041a2a3
0041a2a3  fld        dword ptr [eax + 0x1078]
0041a2a9  pop        esi
0041a2aa  fstp       dword ptr [esp + 4]
0041a2ae  fld        dword ptr [esp + 4]
0041a2b2  ret        4

// block 0041a2b5
0041a2b5  fld        dword ptr [0x4a3db8]
0041a2bb  pop        esi
0041a2bc  fstp       dword ptr [esp + 4]
0041a2c0  fld        dword ptr [esp + 4]
0041a2c4  ret        4

// block 0041a2c7
0041a2c7  fild       dword ptr [0x4b0ccc]
0041a2cd  pop        esi
0041a2ce  ret        4

// block 0041a2d1
0041a2d1  fild       dword ptr [0x4b0ca8]
0041a2d7  pop        esi
0041a2d8  ret        4

// block 0041a2db
0041a2db  fld1       
0041a2dd  pop        esi
0041a2de  ret        4

// block 0041a2e1
0041a2e1  add        ecx, 0x10a8
0041a2e7  call       0x4377a0

// block 0041a2ec
0041a2ec  pop        esi
0041a2ed  ret        4

// block 0041a2f0
0041a2f0  add        ecx, 0x10dc
0041a2f6  call       0x4377a0

// block 0041a2fb
0041a2fb  pop        esi
0041a2fc  ret        4

// block 0041a2ff
0041a2ff  fild       dword ptr [ecx + 0x2648]
0041a305  pop        esi
0041a306  ret        4

// block 0041a309
0041a309  fild       dword ptr [0x4b0ca8]
0041a30f  mov        dword ptr [esp + 8], 1
0041a317  fcomp      dword ptr [0x4a3cf4]
0041a31d  fnstsw     ax
0041a31f  test       ah, 0x44
0041a322  jnp        0x41a32c

// block 0041a324
0041a324  mov        dword ptr [esp + 8], 0

// block 0041a32c
0041a32c  fild       dword ptr [esp + 8]
0041a330  pop        esi
0041a331  ret        4

// block 0041a334
0041a334  fild       dword ptr [0x4b0ca8]
0041a33a  mov        dword ptr [esp + 8], 1
0041a342  fcomp      dword ptr [0x4a3cf0]
0041a348  fnstsw     ax
0041a34a  test       ah, 0x44
0041a34d  jnp        0x41a357

// block 0041a34f
0041a34f  mov        dword ptr [esp + 8], 0

// block 0041a357
0041a357  fild       dword ptr [esp + 8]
0041a35b  pop        esi
0041a35c  ret        4

// block 0041a35f
0041a35f  fild       dword ptr [0x4b0ca8]
0041a365  mov        dword ptr [esp + 8], 1
0041a36d  fcomp      dword ptr [0x4a4014]
0041a373  fnstsw     ax
0041a375  test       ah, 0x44
0041a378  jnp        0x41a382

// block 0041a37a
0041a37a  mov        dword ptr [esp + 8], 0

// block 0041a382
0041a382  fild       dword ptr [esp + 8]
0041a386  pop        esi
0041a387  ret        4

// block 0041a38a
0041a38a  fild       dword ptr [0x4b0ca8]
0041a390  mov        dword ptr [esp + 8], 1
0041a398  fcomp      dword ptr [0x4a4020]
0041a39e  fnstsw     ax
0041a3a0  test       ah, 0x44
0041a3a3  jnp        0x41a3ad

// block 0041a3a5
0041a3a5  mov        dword ptr [esp + 8], 0

// block 0041a3ad
0041a3ad  fild       dword ptr [esp + 8]
0041a3b1  pop        esi
0041a3b2  ret        4

// block 0041a3b5
0041a3b5  mov        edx, dword ptr [0x4b43dc]
0041a3bb  fild       dword ptr [edx + 0x10]
0041a3be  pop        esi
0041a3bf  ret        4

// block 0041a3c2
0041a3c2  mov        eax, dword ptr [0x4b43dc]
0041a3c7  fild       dword ptr [eax + 0x14]
0041a3ca  pop        esi
0041a3cb  ret        4

// block 0041a3ce
0041a3ce  mov        ecx, dword ptr [0x4b43dc]
0041a3d4  fild       dword ptr [ecx + 0x18]
0041a3d7  pop        esi
0041a3d8  ret        4

// block 0041a3db
0041a3db  mov        edx, dword ptr [0x4b43dc]
0041a3e1  fild       dword ptr [edx + 0x70]
0041a3e4  pop        esi
0041a3e5  ret        4

// block 0041a3e8
0041a3e8  mov        eax, dword ptr [0x4b0c94]
0041a3ed  mov        ecx, dword ptr [0x4b0c90]
0041a3f3  lea        edx, [eax + ecx*2]
0041a3f6  mov        dword ptr [esp + 8], edx
0041a3fa  fild       dword ptr [esp + 8]
0041a3fe  pop        esi
0041a3ff  ret        4

// block 0041a402
0041a402  mov        eax, dword ptr [0x4b4514]
0041a407  add        eax, 0x97c
0041a40c  add        ecx, 0x1074
0041a412  call       0x41c4c0

// block 0041a417
0041a417  pop        esi
0041a418  ret        4

// block 0041a41b
0041a41b  mov        eax, dword ptr [0x4b43dc]
0041a420  fild       dword ptr [eax + 0x78]
0041a423  pop        esi
0041a424  ret        4

// block 0041a427
0041a427  fild       dword ptr [0x4b0c48]
0041a42d  pop        esi
0041a42e  ret        4

// block 0041a431
0041a431  fldz       

// block 0041a433
0041a433  pop        esi
0041a434  ret        4
