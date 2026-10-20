
// block 0041c780
0041c780  fld        dword ptr [ecx]
0041c782  fld        dword ptr [esp + 4]
0041c786  fld        st(0)
0041c788  faddp      st(2)
0041c78a  fxch       st(1)
0041c78c  fcomp      qword ptr [0x4a3d60]
0041c792  fnstsw     ax
0041c794  test       ah, 0x41
0041c797  jnp        0x41c7d3

// block 0041c799
0041c799  fsubr      dword ptr [ecx]
0041c79b  fcomp      qword ptr [0x4a3d28]
0041c7a1  fnstsw     ax
0041c7a3  test       ah, 1
0041c7a6  je         0x41c7d5

// block 0041c7a8
0041c7a8  fld        dword ptr [esp + 8]
0041c7ac  fld        st(0)
0041c7ae  fadd       dword ptr [ecx + 4]
0041c7b1  fcomp      qword ptr [0x4a3d58]
0041c7b7  fnstsw     ax
0041c7b9  test       ah, 0x41
0041c7bc  jnp        0x41c7d3

// block 0041c7be
0041c7be  fsubr      dword ptr [ecx + 4]
0041c7c1  fcomp      qword ptr [0x4a3d50]
0041c7c7  fnstsw     ax
0041c7c9  test       ah, 1
0041c7cc  je         0x41c7d5

// block 0041c7ce
0041c7ce  xor        eax, eax
0041c7d0  ret        8

// block 0041c7d3
0041c7d3  fstp       st(0)

// block 0041c7d5
0041c7d5  mov        eax, 1
0041c7da  ret        8
