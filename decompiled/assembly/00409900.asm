
// block 00409900
00409900  fld        dword ptr [esp + 4]
00409904  fld        qword ptr [0x4a3cf8]
0040990a  fmul       st(1), st(0)
0040990c  fld        dword ptr [ecx]
0040990e  fadd       st(2)
00409910  fcomp      qword ptr [0x4a3d60]
00409916  fnstsw     ax
00409918  test       ah, 0x41
0040991b  jnp        0x40995b

// block 0040991d
0040991d  fld        dword ptr [ecx]
0040991f  fsubrp     st(2)
00409921  fxch       st(1)
00409923  fcomp      qword ptr [0x4a3d28]
00409929  fnstsw     ax
0040992b  test       ah, 1
0040992e  je         0x40995d

// block 00409930
00409930  fmul       dword ptr [esp + 8]
00409934  fld        dword ptr [ecx + 4]
00409937  fadd       st(1)
00409939  fcomp      qword ptr [0x4a3da0]
0040993f  fnstsw     ax
00409941  test       ah, 0x41
00409944  jnp        0x40995d

// block 00409946
00409946  fsubr      dword ptr [ecx + 4]
00409949  fcomp      qword ptr [0x4a3d50]
0040994f  fnstsw     ax
00409951  test       ah, 1
00409954  je         0x40995f

// block 00409956
00409956  xor        eax, eax
00409958  ret        8

// block 0040995b
0040995b  fstp       st(1)

// block 0040995d
0040995d  fstp       st(0)

// block 0040995f
0040995f  mov        eax, 1
00409964  ret        8
