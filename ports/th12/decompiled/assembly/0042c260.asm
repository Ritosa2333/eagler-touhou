
// block 0042c260
0042c260  mov        ecx, dword ptr [0x4b44f4]
0042c266  sub        esp, 0x10
0042c269  push       esi
0042c26a  mov        esi, dword ptr [ecx + 0x18]
0042c26d  mov        dword ptr [ecx + 0x48c], esi
0042c273  test       esi, esi
0042c275  je         0x42c3de

// block 0042c27b
0042c27b  jmp        0x42c280

// block 0042c280
0042c280  cmp        dword ptr [esi + 0x10], 1
0042c284  jne        0x42c3c3

// block 0042c28a
0042c28a  cmp        dword ptr [esi + 0xc], 3
0042c28e  jne        0x42c3c3

// block 0042c294
0042c294  fld        dword ptr [esi + 0x6c]
0042c297  sub        esp, 8
0042c29a  fdiv       qword ptr [0x4a3e40]
0042c2a0  lea        ecx, [esp + 0x10]
0042c2a4  fstp       dword ptr [esp + 0xc]
0042c2a8  fld        dword ptr [esp + 0xc]
0042c2ac  fstp       dword ptr [esp + 4]
0042c2b0  fld        dword ptr [esi + 0x68]
0042c2b3  fstp       dword ptr [esp]
0042c2b6  call       0x42e880

// block 0042c2bb
0042c2bb  fld        dword ptr [esi + 0x50]
0042c2be  fadd       dword ptr [esp + 8]
0042c2c2  mov        eax, dword ptr [esp + 0x18]
0042c2c6  push       ecx
0042c2c7  fstp       dword ptr [esp + 0xc]
0042c2cb  fld        dword ptr [esi + 0x54]
0042c2ce  fadd       dword ptr [esp + 0x10]
0042c2d2  fstp       dword ptr [esp + 0x10]
0042c2d6  fld        dword ptr [esi + 0x58]
0042c2d9  fadd       dword ptr [esp + 0x14]
0042c2dd  fstp       dword ptr [esp + 0x14]
0042c2e1  fld        dword ptr [esi + 0x46c]
0042c2e7  fsub       dword ptr [eax + 0x24c]
0042c2ed  fstp       dword ptr [esp + 8]
0042c2f1  fld        dword ptr [esp + 8]
0042c2f5  fstp       dword ptr [esp]
0042c2f8  call       0x4646e0

// block 0042c2fd
0042c2fd  fstp       dword ptr [esi + 0x46c]
0042c303  sub        esp, 0xc
0042c306  fld        dword ptr [esi + 0x6c]
0042c309  lea        eax, [esp + 0x14]
0042c30d  fstp       dword ptr [esp + 8]
0042c311  fld        dword ptr [0x4a3e38]
0042c317  fstp       dword ptr [esp + 4]
0042c31b  fld        dword ptr [esi + 0x68]
0042c31e  fstp       dword ptr [esp]
0042c321  call       0x437d00

// block 0042c326
0042c326  cmp        eax, 1
0042c329  je         0x42c3a9

// block 0042c32b
0042c32b  mov        ecx, dword ptr [esi + 0x484]
0042c331  sub        ecx, 0x3c
0042c334  cmp        dword ptr [esi + 0x18], ecx
0042c337  jl         0x42c33f

// block 0042c339
0042c339  cmp        dword ptr [esi + 0xc], 3
0042c33d  je         0x42c3a9

// block 0042c33f
0042c33f  fld        dword ptr [esi + 0x68]
0042c342  and        dword ptr [esi + 0xadc], 0xfffeffff
0042c34c  sub        esp, 8
0042c34f  fstp       dword ptr [esp + 4]
0042c353  mov        dword ptr [esi + 0x65c], 0
0042c35d  fld        dword ptr [esi + 0x46c]
0042c363  fstp       dword ptr [esp]
0042c366  call       0x42e770

// block 0042c36b
0042c36b  fstp       dword ptr [esp + 4]
0042c36f  fld        dword ptr [esp + 4]
0042c373  fld        st(0)
0042c375  fabs       
0042c377  fstp       dword ptr [esp + 4]
0042c37b  fld        dword ptr [esp + 4]
0042c37f  fcomp      qword ptr [0x4a3e30]
0042c385  fnstsw     ax
0042c387  test       ah, 5
0042c38a  jnp        0x42c392

// block 0042c38c
0042c38c  fmul       qword ptr [0x4a3e28]

// block 0042c392
0042c392  fadd       dword ptr [esi + 0x68]
0042c395  push       ecx
0042c396  fstp       dword ptr [esi + 0x68]
0042c399  fld        dword ptr [esi + 0x68]
0042c39c  fstp       dword ptr [esp]
0042c39f  call       0x4646e0

// block 0042c3a4
0042c3a4  fstp       dword ptr [esi + 0x68]
0042c3a7  jmp        0x42c3bd

// block 0042c3a9
0042c3a9  or         dword ptr [esi + 0xadc], 0x10000
0042c3b3  mov        dword ptr [esi + 0xa20], 0xffff4040

// block 0042c3bd
0042c3bd  mov        ecx, dword ptr [0x4b44f4]

// block 0042c3c3
0042c3c3  mov        eax, dword ptr [ecx + 0x48c]
0042c3c9  test       eax, eax
0042c3cb  je         0x42c3de

// block 0042c3cd
0042c3cd  mov        esi, dword ptr [eax + 8]
0042c3d0  mov        dword ptr [ecx + 0x48c], esi
0042c3d6  test       esi, esi
0042c3d8  jne        0x42c280

// block 0042c3de
0042c3de  xor        eax, eax
0042c3e0  pop        esi
0042c3e1  add        esp, 0x10
0042c3e4  ret        4
