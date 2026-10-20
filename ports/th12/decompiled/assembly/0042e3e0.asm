
// block 0042e3e0
0042e3e0  sub        esp, 0x14
0042e3e3  cmp        dword ptr [esp + 0x1c], 0
0042e3e8  push       esi
0042e3e9  mov        esi, ecx
0042e3eb  mov        dword ptr [esp + 4], esi
0042e3ef  je         0x42e3fe

// block 0042e3f1
0042e3f1  cmp        dword ptr [esi + 0x44c], 0
0042e3f8  jne        0x42e5a3

// block 0042e3fe
0042e3fe  xor        ecx, ecx
0042e400  cmp        dword ptr [esi + 0x470], ecx
0042e406  push       ebp
0042e407  mov        ebp, dword ptr [esi + 0xf9c]
0042e40d  mov        dword ptr [esp + 0x24], ecx
0042e411  jle        0x42e59b

// block 0042e417
0042e417  fld        qword ptr [0x4a3d70]
0042e41d  push       ebx
0042e41e  push       edi

// block 0042e41f
0042e41f  mov        eax, dword ptr [ebp]
0042e422  mov        edx, dword ptr [ebp + 4]
0042e425  mov        dword ptr [esp + 0x18], eax
0042e429  mov        eax, dword ptr [ebp + 8]
0042e42c  mov        dword ptr [esp + 0x20], eax
0042e430  mov        dword ptr [esp + 0x1c], edx
0042e434  mov        eax, 0x55555556
0042e439  imul       ecx
0042e43b  mov        eax, edx
0042e43d  shr        eax, 0x1f
0042e440  add        eax, edx
0042e442  lea        edx, [eax + eax*2]
0042e445  mov        eax, ecx
0042e447  sub        eax, edx
0042e449  jne        0x42e583

// block 0042e44f
0042e44f  fld        dword ptr [esp + 0x1c]
0042e453  fld        dword ptr [esp + 0x18]
0042e457  cmp        dword ptr [esp + 0x28], eax
0042e45b  je         0x42e4c1

// block 0042e45d
0042e45d  fld        st(0)
0042e45f  fadd       st(3)
0042e461  fcomp      qword ptr [0x4a3d60]
0042e467  fnstsw     ax
0042e469  test       ah, 0x41
0042e46c  jnp        0x42e4c1

// block 0042e46e
0042e46e  fsub       st(2)
0042e470  fcomp      qword ptr [0x4a3d28]
0042e476  fnstsw     ax
0042e478  test       ah, 1
0042e47b  je         0x42e4c3

// block 0042e47d
0042e47d  fld        st(0)
0042e47f  fadd       st(2)
0042e481  fcomp      qword ptr [0x4a3d58]
0042e487  fnstsw     ax
0042e489  test       ah, 0x41
0042e48c  jnp        0x42e4c3

// block 0042e48e
0042e48e  fsubrp     st(1)
0042e490  fcomp      qword ptr [0x4a3d50]
0042e496  fnstsw     ax
0042e498  test       ah, 1
0042e49b  je         0x42e4c7

// block 0042e49d
0042e49d  fld        dword ptr [0x4a41f4]
0042e4a3  sub        esp, 8
0042e4a6  fstp       dword ptr [esp + 4]
0042e4aa  lea        ecx, [esp + 0x20]
0042e4ae  fld        dword ptr [0x4a4060]
0042e4b4  fstp       dword ptr [esp]
0042e4b7  push       ecx
0042e4b8  push       9
0042e4ba  call       0x4273f0

// block 0042e4bf
0042e4bf  jmp        0x42e4c7

// block 0042e4c1
0042e4c1  fstp       st(2)

// block 0042e4c3
0042e4c3  fstp       st(1)
0042e4c5  fstp       st(0)

// block 0042e4c7
0042e4c7  test       dword ptr [0x4cee78], 0x8000
0042e4d1  movsx      edi, word ptr [esi + 0x46e]
0042e4d8  mov        edx, dword ptr [0x4b44f4]
0042e4de  mov        esi, dword ptr [edx + 0x488]
0042e4e4  lea        edi, [edi + edi + 4]
0042e4e8  je         0x42e4fb

// block 0042e4ea
0042e4ea  push       0x4cf1d0
0042e4ef  call       dword ptr [0x498088]

// block 0042e4f5
0042e4f5  inc        byte ptr [0x4cf221]

// block 0042e4fb
0042e4fb  inc        dword ptr [esi + 0x130]
0042e501  call       0x4621c0

// block 0042e506
0042e506  fld        dword ptr [esp + 0x18]
0042e50a  fadd       qword ptr [0x4a3d70]
0042e510  mov        ebx, eax
0042e512  or         dword ptr [ebx + 0x480], 1
0042e519  mov        dword ptr [ebx + 0x20], 0x17
0042e520  fadd       qword ptr [0x4a3d28]
0042e526  push       edi
0042e527  push       ebx
0042e528  mov        ecx, esi
0042e52a  fstp       dword ptr [ebx + 0x430]
0042e530  fld        dword ptr [esp + 0x24]
0042e534  fadd       qword ptr [0x4a3d68]
0042e53a  fstp       dword ptr [ebx + 0x434]
0042e540  fld        dword ptr [esp + 0x28]
0042e544  fstp       dword ptr [ebx + 0x438]
0042e54a  call       0x454d10

// block 0042e54f
0042e54f  lea        eax, [esp + 0x14]
0042e553  call       0x461250

// block 0042e558
0042e558  test       dword ptr [0x4cee78], 0x8000
0042e562  je         0x42e575

// block 0042e564
0042e564  push       0x4cf1d0
0042e569  call       dword ptr [0x49808c]

// block 0042e56f
0042e56f  dec        byte ptr [0x4cf221]

// block 0042e575
0042e575  mov        esi, dword ptr [esp + 0x10]
0042e579  fld        qword ptr [0x4a3d70]
0042e57f  mov        ecx, dword ptr [esp + 0x2c]

// block 0042e583
0042e583  inc        ecx
0042e584  add        ebp, 0x14
0042e587  cmp        ecx, dword ptr [esi + 0x470]
0042e58d  mov        dword ptr [esp + 0x2c], ecx
0042e591  jl         0x42e41f

// block 0042e597
0042e597  pop        edi
0042e598  fstp       st(0)
0042e59a  pop        ebx

// block 0042e59b
0042e59b  mov        dword ptr [esi + 0xc], 1
0042e5a2  pop        ebp

// block 0042e5a3
0042e5a3  xor        eax, eax
0042e5a5  pop        esi
0042e5a6  add        esp, 0x14
0042e5a9  ret        8
