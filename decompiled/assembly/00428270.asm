
// block 00428270
00428270  push       ecx
00428271  push       esi
00428272  mov        esi, dword ptr [edi + 0x18]
00428275  test       esi, esi
00428277  je         0x428390

// block 0042827d
0042827d  push       ebx
0042827e  push       ebp
0042827f  or         ebp, 0xffffffff

// block 00428282
00428282  mov        al, byte ptr [esi + 0x7c]
00428285  mov        ebx, dword ptr [esi + 8]
00428288  test       al, al
0042828a  je         0x4282d9

// block 0042828c
0042828c  inc        al
0042828e  mov        byte ptr [esi + 0x7c], al
00428291  cmp        al, 2
00428293  jb         0x4282d9

// block 00428295
00428295  mov        eax, dword ptr [esi]
00428297  mov        edx, dword ptr [eax + 0x10]
0042829a  mov        ecx, esi
0042829c  call       edx

// block 0042829e
0042829e  add        dword ptr [edi + 0x468], ebp
004282a4  mov        eax, dword ptr [esi + 4]
004282a7  mov        ecx, dword ptr [esi + 8]
004282aa  mov        dword ptr [eax + 8], ecx
004282ad  mov        eax, dword ptr [esi + 8]
004282b0  test       eax, eax
004282b2  je         0x4282ba

// block 004282b4
004282b4  mov        edx, dword ptr [esi + 4]
004282b7  mov        dword ptr [eax + 4], edx

// block 004282ba
004282ba  cmp        dword ptr [edi + 0x464], esi
004282c0  jne        0x4282cb

// block 004282c2
004282c2  mov        eax, dword ptr [esi + 4]
004282c5  mov        dword ptr [edi + 0x464], eax

// block 004282cb
004282cb  push       esi
004282cc  call       0x46ca4f

// block 004282d1
004282d1  add        esp, 4
004282d4  jmp        0x428384

// block 004282d9
004282d9  cmp        dword ptr [esi + 0xc], 1
004282dd  mov        ecx, esi
004282df  jne        0x428320

// block 004282e1
004282e1  mov        edx, dword ptr [esi]
004282e3  mov        eax, dword ptr [edx + 0x10]
004282e6  call       eax

// block 004282e8
004282e8  add        dword ptr [edi + 0x468], ebp
004282ee  mov        ecx, dword ptr [esi + 4]
004282f1  mov        edx, dword ptr [esi + 8]
004282f4  mov        dword ptr [ecx + 8], edx
004282f7  mov        eax, dword ptr [esi + 8]
004282fa  test       eax, eax
004282fc  je         0x428304

// block 004282fe
004282fe  mov        ecx, dword ptr [esi + 4]
00428301  mov        dword ptr [eax + 4], ecx

// block 00428304
00428304  cmp        dword ptr [edi + 0x464], esi
0042830a  jne        0x4282cb

// block 0042830c
0042830c  mov        edx, dword ptr [esi + 4]
0042830f  push       esi
00428310  mov        dword ptr [edi + 0x464], edx
00428316  call       0x46ca4f

// block 0042831b
0042831b  add        esp, 4
0042831e  jmp        0x428384

// block 00428320
00428320  mov        eax, dword ptr [esi]
00428322  mov        edx, dword ptr [eax + 8]
00428325  call       edx

// block 00428327
00428327  test       eax, eax
00428329  jne        0x428295

// block 0042832f
0042832f  mov        edx, dword ptr [esi + 0x18]
00428332  mov        ecx, dword ptr [esi + 0x20]
00428335  mov        dword ptr [esi + 0x14], edx
00428338  fld        dword ptr [ecx]
0042833a  fcomp      qword ptr [0x4a3db0]
00428340  fnstsw     ax
00428342  test       ah, 0x41
00428345  jne        0x428368

// block 00428347
00428347  fld        dword ptr [0x4a3dac]
0042834d  fcomp      dword ptr [ecx]
0042834f  fnstsw     ax
00428351  test       ah, 0x41
00428354  jne        0x428368

// block 00428356
00428356  fld        dword ptr [esi + 0x1c]
00428359  inc        edx
0042835a  fadd       qword ptr [0x4a3ce0]
00428360  mov        dword ptr [esi + 0x18], edx
00428363  fstp       dword ptr [esi + 0x1c]
00428366  jmp        0x428380

// block 00428368
00428368  fld        dword ptr [ecx]
0042836a  fadd       dword ptr [esi + 0x1c]
0042836d  fstp       dword ptr [esp + 0xc]
00428371  fld        dword ptr [esp + 0xc]
00428375  fst        dword ptr [esi + 0x1c]
00428378  call       0x4931e0

// block 0042837d
0042837d  mov        dword ptr [esi + 0x18], eax

// block 00428380
00428380  mov        byte ptr [esi + 0x7d], 1

// block 00428384
00428384  mov        esi, ebx
00428386  test       ebx, ebx
00428388  jne        0x428282

// block 0042838e
0042838e  pop        ebp
0042838f  pop        ebx

// block 00428390
00428390  mov        eax, 1
00428395  pop        esi
00428396  pop        ecx
00428397  ret        
