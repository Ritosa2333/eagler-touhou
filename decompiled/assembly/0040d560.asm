
// block 0040d560
0040d560  fld        dword ptr [ecx]
0040d562  fld        dword ptr [esp + 4]
0040d566  fld        st(0)
0040d568  faddp      st(2)
0040d56a  fxch       st(1)
0040d56c  fcomp      qword ptr [0x4a3d60]
0040d572  fnstsw     ax
0040d574  test       ah, 0x41
0040d577  jnp        0x40d5b3

// block 0040d579
0040d579  fsubr      dword ptr [ecx]
0040d57b  fcomp      qword ptr [0x4a3d28]
0040d581  fnstsw     ax
0040d583  test       ah, 1
0040d586  je         0x40d5b5

// block 0040d588
0040d588  fld        dword ptr [esp + 8]
0040d58c  fld        st(0)
0040d58e  fadd       dword ptr [ecx + 4]
0040d591  fcomp      qword ptr [0x4a3d58]
0040d597  fnstsw     ax
0040d599  test       ah, 0x41
0040d59c  jnp        0x40d5b3

// block 0040d59e
0040d59e  fsubr      dword ptr [ecx + 4]
0040d5a1  fcomp      qword ptr [0x4a3d50]
0040d5a7  fnstsw     ax
0040d5a9  test       ah, 1
0040d5ac  je         0x40d5b5

// block 0040d5ae
0040d5ae  xor        eax, eax
0040d5b0  ret        8

// block 0040d5b3
0040d5b3  fstp       st(0)

// block 0040d5b5
0040d5b5  mov        eax, 1
0040d5ba  ret        8
