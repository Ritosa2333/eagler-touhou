
// block 00409970
00409970  fld        dword ptr [esp + 4]
00409974  fld        qword ptr [0x4a3cf8]
0040997a  fmul       st(1), st(0)
0040997c  fld        dword ptr [ecx]
0040997e  fadd       st(2)
00409980  fcomp      qword ptr [0x4a3d60]
00409986  fnstsw     ax
00409988  test       ah, 0x41
0040998b  jnp        0x4099cb

// block 0040998d
0040998d  fld        dword ptr [ecx]
0040998f  fsubrp     st(2)
00409991  fxch       st(1)
00409993  fcomp      qword ptr [0x4a3d28]
00409999  fnstsw     ax
0040999b  test       ah, 1
0040999e  je         0x4099cd

// block 004099a0
004099a0  fmul       dword ptr [esp + 8]
004099a4  fld        dword ptr [ecx + 4]
004099a7  fadd       st(1)
004099a9  fcomp      qword ptr [0x4a3d58]
004099af  fnstsw     ax
004099b1  test       ah, 0x41
004099b4  jnp        0x4099cd

// block 004099b6
004099b6  fsubr      dword ptr [ecx + 4]
004099b9  fcomp      qword ptr [0x4a3d50]
004099bf  fnstsw     ax
004099c1  test       ah, 1
004099c4  je         0x4099cf

// block 004099c6
004099c6  xor        eax, eax
004099c8  ret        8

// block 004099cb
004099cb  fstp       st(1)

// block 004099cd
004099cd  fstp       st(0)

// block 004099cf
004099cf  mov        eax, 1
004099d4  ret        8
