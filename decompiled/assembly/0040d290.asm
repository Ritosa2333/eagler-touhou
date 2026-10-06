
// block 0040d290
0040d290  fld        dword ptr [esp + 4]
0040d294  sub        esp, 0x30
0040d297  fmul       st(0), st(0)
0040d299  push       ebx
0040d29a  push       esi
0040d29b  xor        esi, esi
0040d29d  fstp       dword ptr [esp + 0x3c]
0040d2a1  add        ecx, 0x540
0040d2a7  fld        dword ptr [esp + 0x3c]
0040d2ab  mov        ebx, 0x7d0
0040d2b0  fld        qword ptr [0x4a3cf8]
0040d2b6  fld        dword ptr [0x4a3e54]
0040d2bc  fld        dword ptr [0x4a3e68]

// block 0040d2c2
0040d2c2  movzx      eax, word ptr [ecx + 0x56]
0040d2c6  test       ax, ax
0040d2c9  je         0x40d3cd

// block 0040d2cf
0040d2cf  cmp        ax, 3
0040d2d3  je         0x40d3cd

// block 0040d2d9
0040d2d9  fld        dword ptr [ecx]
0040d2db  fmul       st(3)
0040d2dd  fld        dword ptr [ecx - 0x20]
0040d2e0  fsub       st(1)
0040d2e2  fstp       dword ptr [esp + 8]
0040d2e6  fld        dword ptr [ecx + 4]
0040d2e9  fmul       st(4)
0040d2eb  fld        dword ptr [ecx - 0x1c]
0040d2ee  fsub       st(1)
0040d2f0  fstp       dword ptr [esp + 0xc]
0040d2f4  fld        dword ptr [esp + 0xc]
0040d2f8  fst        dword ptr [esp + 0x24]
0040d2fc  fld        dword ptr [ecx - 0x20]
0040d2ff  faddp      st(3)
0040d301  fxch       st(2)
0040d303  fstp       dword ptr [esp + 0x14]
0040d307  fadd       dword ptr [ecx - 0x1c]
0040d30a  fstp       dword ptr [esp + 0x18]
0040d30e  fld        dword ptr [esp + 0x18]
0040d312  fst        dword ptr [esp + 0x30]
0040d316  fld        dword ptr [ecx + 4]
0040d319  faddp      st(2)
0040d31b  fxch       st(1)
0040d31d  fstp       dword ptr [esp + 0xc]
0040d321  fsub       dword ptr [ecx + 4]
0040d324  fstp       dword ptr [esp + 0x18]
0040d328  fld        dword ptr [edi + 4]
0040d32b  fsub       dword ptr [esp + 0x24]
0040d32f  fld        dword ptr [edi]
0040d331  fsub       dword ptr [esp + 8]
0040d335  fmul       st(0), st(0)
0040d337  fld        st(1)
0040d339  fmulp      st(2)
0040d33b  fadd       st(1), st(0)
0040d33d  fxch       st(1)
0040d33f  fstp       dword ptr [esp + 0x3c]
0040d343  fld        dword ptr [esp + 0x3c]
0040d347  fcomp      st(5)
0040d349  fnstsw     ax
0040d34b  test       ah, 0x41
0040d34e  jne        0x40d3b2

// block 0040d350
0040d350  fld        dword ptr [edi + 4]
0040d353  fsub       dword ptr [esp + 0xc]
0040d357  fmul       st(0), st(0)
0040d359  faddp      st(1)
0040d35b  fstp       dword ptr [esp + 0x3c]
0040d35f  fld        dword ptr [esp + 0x3c]
0040d363  fcomp      st(4)
0040d365  fnstsw     ax
0040d367  test       ah, 0x41
0040d36a  jne        0x40d3b4

// block 0040d36c
0040d36c  fld        dword ptr [edi + 4]
0040d36f  fsub       dword ptr [esp + 0x30]
0040d373  fld        dword ptr [edi]
0040d375  fsub       dword ptr [esp + 0x14]
0040d379  fmul       st(0), st(0)
0040d37b  fld        st(1)
0040d37d  fmulp      st(2)
0040d37f  fadd       st(1), st(0)
0040d381  fxch       st(1)
0040d383  fstp       dword ptr [esp + 0x3c]
0040d387  fld        dword ptr [esp + 0x3c]
0040d38b  fcomp      st(5)
0040d38d  fnstsw     ax
0040d38f  test       ah, 0x41
0040d392  jne        0x40d3b2

// block 0040d394
0040d394  fld        dword ptr [edi + 4]
0040d397  fsub       dword ptr [esp + 0x18]
0040d39b  fmul       st(0), st(0)
0040d39d  faddp      st(1)
0040d39f  fstp       dword ptr [esp + 0x3c]
0040d3a3  fld        dword ptr [esp + 0x3c]
0040d3a7  fcomp      st(4)
0040d3a9  fnstsw     ax
0040d3ab  test       ah, 0x41
0040d3ae  je         0x40d3cd

// block 0040d3b0
0040d3b0  jmp        0x40d3b4

// block 0040d3b2
0040d3b2  fstp       st(0)

// block 0040d3b4
0040d3b4  mov        edx, dword ptr [ecx - 0xe0]
0040d3ba  test       edx, edx
0040d3bc  je         0x40d3cd

// block 0040d3be
0040d3be  fxch       st(1)
0040d3c0  fcom       dword ptr [edx + 0x38]
0040d3c3  fnstsw     ax
0040d3c5  test       ah, 1
0040d3c8  jne        0x40d3ee

// block 0040d3ca
0040d3ca  inc        esi
0040d3cb  fxch       st(1)

// block 0040d3cd
0040d3cd  add        ecx, 0x9f8
0040d3d3  sub        ebx, 1
0040d3d6  jne        0x40d2c2

// block 0040d3dc
0040d3dc  fstp       st(3)
0040d3de  mov        eax, esi
0040d3e0  fstp       st(1)
0040d3e2  pop        esi
0040d3e3  fstp       st(0)
0040d3e5  pop        ebx
0040d3e6  fstp       st(0)
0040d3e8  add        esp, 0x30
0040d3eb  ret        4

// block 0040d3ee
0040d3ee  fxch       st(1)
0040d3f0  fcom       dword ptr [edx + 0x38]
0040d3f3  fnstsw     ax
0040d3f5  test       ah, 1
0040d3f8  jne        0x40d3fd

// block 0040d3fa
0040d3fa  inc        esi
0040d3fb  jmp        0x40d3cd

// block 0040d3fd
0040d3fd  fld        dword ptr [0x4a3d78]
0040d403  fcomp      dword ptr [edx + 0x38]
0040d406  fnstsw     ax
0040d408  test       ah, 1
0040d40b  jne        0x40d412

// block 0040d40d
0040d40d  add        esi, 4
0040d410  jmp        0x40d3cd

// block 0040d412
0040d412  fld        dword ptr [0x4a3e04]
0040d418  fcomp      dword ptr [edx + 0x38]
0040d41b  fnstsw     ax
0040d41d  test       ah, 1
0040d420  jne        0x40d3cd

// block 0040d422
0040d422  add        esi, 0xa
0040d425  jmp        0x40d3cd
