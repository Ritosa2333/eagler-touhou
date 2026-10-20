
// block 0042e820
0042e820  fld        dword ptr [ecx]
0042e822  fld        dword ptr [esp + 4]
0042e826  fld        st(0)
0042e828  faddp      st(2)
0042e82a  fxch       st(1)
0042e82c  fcomp      qword ptr [0x4a3d60]
0042e832  fnstsw     ax
0042e834  test       ah, 0x41
0042e837  jnp        0x42e873

// block 0042e839
0042e839  fsubr      dword ptr [ecx]
0042e83b  fcomp      qword ptr [0x4a3d28]
0042e841  fnstsw     ax
0042e843  test       ah, 1
0042e846  je         0x42e875

// block 0042e848
0042e848  fld        dword ptr [esp + 8]
0042e84c  fld        st(0)
0042e84e  fadd       dword ptr [ecx + 4]
0042e851  fcomp      qword ptr [0x4a3d58]
0042e857  fnstsw     ax
0042e859  test       ah, 0x41
0042e85c  jnp        0x42e873

// block 0042e85e
0042e85e  fsubr      dword ptr [ecx + 4]
0042e861  fcomp      qword ptr [0x4a3d50]
0042e867  fnstsw     ax
0042e869  test       ah, 1
0042e86c  je         0x42e875

// block 0042e86e
0042e86e  xor        eax, eax
0042e870  ret        8

// block 0042e873
0042e873  fstp       st(0)

// block 0042e875
0042e875  mov        eax, 1
0042e87a  ret        8
