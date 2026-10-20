
// block 0042b1d0
0042b1d0  push       ebp
0042b1d1  mov        ebp, esp
0042b1d3  and        esp, 0xfffffff8
0042b1d6  sub        esp, 0x34c
0042b1dc  mov        eax, dword ptr [0x4ad138]
0042b1e1  xor        eax, esp
0042b1e3  mov        dword ptr [esp + 0x348], eax
0042b1ea  mov        eax, dword ptr [ebp + 0xc]
0042b1ed  push       ebx
0042b1ee  push       esi
0042b1ef  mov        esi, dword ptr [ebp + 8]
0042b1f2  xor        ebx, ebx
0042b1f4  push       edi
0042b1f5  mov        edi, ecx
0042b1f7  mov        dword ptr [esp + 0x10], eax
0042b1fb  cmp        dword ptr [ebp + 0x14], ebx
0042b1fe  je         0x42b221

// block 0042b200
0042b200  cmp        dword ptr [edi + 0x44c], ebx
0042b206  je         0x42b221

// block 0042b208
0042b208  xor        eax, eax
0042b20a  pop        edi
0042b20b  pop        esi
0042b20c  pop        ebx
0042b20d  mov        ecx, dword ptr [esp + 0x348]
0042b214  xor        ecx, esp
0042b216  call       0x46c932

// block 0042b21b
0042b21b  mov        esp, ebp
0042b21d  pop        ebp
0042b21e  ret        0x10

// block 0042b221
0042b221  mov        ecx, dword ptr [edi + 0x50]
0042b224  fld        dword ptr [0x4a3e54]
0042b22a  mov        edx, dword ptr [edi + 0x54]
0042b22d  fstp       dword ptr [esp + 0xc]
0042b231  mov        eax, dword ptr [edi + 0x58]
0042b234  push       0x100
0042b239  mov        dword ptr [esp + 0x44], ecx
0042b23d  lea        ecx, [esp + 0x254]
0042b244  push       ebx
0042b245  push       ecx
0042b246  mov        dword ptr [esp + 0x38], ebx
0042b24a  mov        dword ptr [esp + 0x3c], ebx
0042b24e  mov        dword ptr [esp + 0x50], edx
0042b252  mov        dword ptr [esp + 0x54], eax
0042b256  call       0x477420

// block 0042b25b
0042b25b  mov        eax, dword ptr [esp + 0x1c]
0042b25f  fld        dword ptr [eax]
0042b261  add        esp, 4
0042b264  fld        qword ptr [0x4a3cf8]
0042b26a  lea        ecx, [esp + 0x3c]
0042b26e  fmul       st(1), st(0)
0042b270  fxch       st(1)
0042b272  fstp       dword ptr [esp + 0x28]
0042b276  fmul       dword ptr [eax + 4]
0042b279  fstp       dword ptr [esp + 0x2c]
0042b27d  fld        dword ptr [esi]
0042b27f  fld        dword ptr [esp + 0x28]
0042b283  fld        st(0)
0042b285  fsubp      st(2)
0042b287  fxch       st(1)
0042b289  fstp       dword ptr [esp + 0x54]
0042b28d  fld        dword ptr [esi + 4]
0042b290  fld        dword ptr [esp + 0x2c]
0042b294  fld        st(0)
0042b296  fsubp      st(2)
0042b298  fxch       st(1)
0042b29a  fstp       dword ptr [esp + 0x58]
0042b29e  fld        dword ptr [esi]
0042b2a0  faddp      st(2)
0042b2a2  fxch       st(1)
0042b2a4  fstp       dword ptr [esp + 0x60]
0042b2a8  fadd       dword ptr [esi + 4]
0042b2ab  fstp       dword ptr [esp + 0x64]
0042b2af  fldz       
0042b2b1  fstp       dword ptr [esp + 0x44]
0042b2b5  fld        dword ptr [0x4a3e54]
0042b2bb  fstp       dword ptr [esp + 4]
0042b2bf  fld        dword ptr [edi + 0x68]
0042b2c2  fstp       dword ptr [esp]
0042b2c5  call       0x42e880

// block 0042b2ca
0042b2ca  fld        dword ptr [esp + 0x34]
0042b2ce  fld        st(0)
0042b2d0  fadd       dword ptr [edi + 0x50]
0042b2d3  fstp       dword ptr [esp + 0x20]
0042b2d7  mov        edx, dword ptr [esp + 0x20]
0042b2db  fld        dword ptr [edi + 0x54]
0042b2de  mov        dword ptr [esp + 0x14], edx
0042b2e2  fld        dword ptr [esp + 0x38]
0042b2e6  fld        st(0)
0042b2e8  faddp      st(2)
0042b2ea  fxch       st(1)
0042b2ec  fstp       dword ptr [esp + 0x24]
0042b2f0  mov        eax, dword ptr [esp + 0x24]
0042b2f4  fld        dword ptr [edi + 0x58]
0042b2f7  mov        dword ptr [esp + 0x18], eax
0042b2fb  fld        dword ptr [esp + 0x3c]
0042b2ff  fld        st(0)
0042b301  faddp      st(2)
0042b303  fxch       st(1)
0042b305  fstp       dword ptr [esp + 0x28]
0042b309  mov        ecx, dword ptr [esp + 0x28]
0042b30d  fldz       
0042b30f  mov        dword ptr [esp + 0x1c], ecx
0042b313  fstp       dword ptr [esp + 0x1c]
0042b317  fld        st(2)
0042b319  faddp      st(3)
0042b31b  fxch       st(2)
0042b31d  fstp       dword ptr [esp + 0x34]
0042b321  fadd       st(0), st(0)
0042b323  fstp       dword ptr [esp + 0x38]
0042b327  fadd       st(0), st(0)
0042b329  fstp       dword ptr [esp + 0x3c]
0042b32d  fld        dword ptr [edi + 0x6c]
0042b330  fcomp      qword ptr [0x4a3d68]
0042b336  fnstsw     ax
0042b338  test       ah, 0x41
0042b33b  jne        0x42b55c

// block 0042b341
0042b341  jmp        0x42b345

// block 0042b343
0042b343  fstp       st(0)

// block 0042b345
0042b345  fld        dword ptr [0x4a3d78]
0042b34b  fld        dword ptr [esp + 0x14]
0042b34f  fld        dword ptr [esp + 0x4c]
0042b353  fcomp      st(1)
0042b355  fnstsw     ax
0042b357  fld        dword ptr [esp + 0x18]
0042b35b  test       ah, 0x41
0042b35e  je         0x42b4c3

// block 0042b364
0042b364  fld        dword ptr [esp + 0x58]
0042b368  fcomp      st(2)
0042b36a  fnstsw     ax
0042b36c  fstp       st(1)
0042b36e  test       ah, 5
0042b371  jnp        0x42b4c5

// block 0042b377
0042b377  fld        dword ptr [esp + 0x50]
0042b37b  fcomp      st(1)
0042b37d  fnstsw     ax
0042b37f  test       ah, 0x41
0042b382  je         0x42b4c5

// block 0042b388
0042b388  fld        dword ptr [esp + 0x5c]
0042b38c  fcompp     
0042b38e  fnstsw     ax
0042b390  test       ah, 5
0042b393  jnp        0x42b4c7

// block 0042b399
0042b399  inc        dword ptr [esp + 0x2c]
0042b39d  cmp        dword ptr [ebp + 0x10], 0
0042b3a1  mov        byte ptr [esp + ebx + 0x250], 1
0042b3a9  je         0x42b3e4

// block 0042b3ab
0042b3ab  sub        esp, 8
0042b3ae  fst        dword ptr [esp + 4]
0042b3b2  lea        ecx, [esp + 0x1c]
0042b3b6  fstp       dword ptr [esp]
0042b3b9  call       0x42e820

// block 0042b3be
0042b3be  test       eax, eax
0042b3c0  jne        0x42b3e6

// block 0042b3c2
0042b3c2  fld        dword ptr [0x4a41f4]
0042b3c8  sub        esp, 8
0042b3cb  fstp       dword ptr [esp + 4]
0042b3cf  mov        edx, ecx
0042b3d1  fld        dword ptr [0x4a4060]
0042b3d7  fstp       dword ptr [esp]
0042b3da  push       edx
0042b3db  push       9
0042b3dd  call       0x4273f0

// block 0042b3e2
0042b3e2  jmp        0x42b3e6

// block 0042b3e4
0042b3e4  fstp       st(0)

// block 0042b3e6
0042b3e6  fld        dword ptr [0x4a3d78]
0042b3ec  sub        esp, 8
0042b3ef  fst        dword ptr [esp + 4]
0042b3f3  lea        ecx, [esp + 0x1c]
0042b3f7  fstp       dword ptr [esp]
0042b3fa  call       0x42e820

// block 0042b3ff
0042b3ff  test       eax, eax
0042b401  jne        0x42b4c9

// block 0042b407
0042b407  test       dword ptr [0x4cee78], 0x8000
0042b411  movsx      eax, word ptr [edi + 0x4a6]
0042b418  mov        edx, dword ptr [0x4b44f4]
0042b41e  mov        esi, dword ptr [edx + 0x488]
0042b424  lea        ecx, [eax + eax + 4]
0042b428  mov        dword ptr [esp + 0x10], ecx
0042b42c  je         0x42b43f

// block 0042b42e
0042b42e  push       0x4cf1d0
0042b433  call       dword ptr [0x498088]

// block 0042b439
0042b439  inc        byte ptr [0x4cf221]

// block 0042b43f
0042b43f  inc        dword ptr [esi + 0x130]
0042b445  call       0x4621c0

// block 0042b44a
0042b44a  fld        dword ptr [esp + 0x14]
0042b44e  fadd       qword ptr [0x4a3d70]
0042b454  mov        ebx, eax
0042b456  or         dword ptr [ebx + 0x480], 1
0042b45d  mov        eax, dword ptr [esp + 0x10]
0042b461  fadd       qword ptr [0x4a3d28]
0042b467  mov        dword ptr [ebx + 0x20], 0x17
0042b46e  push       eax
0042b46f  push       ebx
0042b470  fstp       dword ptr [ebx + 0x430]
0042b476  mov        ecx, esi
0042b478  fld        dword ptr [esp + 0x20]
0042b47c  fadd       qword ptr [0x4a3d68]
0042b482  fstp       dword ptr [ebx + 0x434]
0042b488  fld        dword ptr [esp + 0x24]
0042b48c  fstp       dword ptr [ebx + 0x438]
0042b492  call       0x454d10

// block 0042b497
0042b497  lea        eax, [esp + 0x64]
0042b49b  call       0x461250

// block 0042b4a0
0042b4a0  test       dword ptr [0x4cee78], 0x8000
0042b4aa  je         0x42b4bd

// block 0042b4ac
0042b4ac  push       0x4cf1d0
0042b4b1  call       dword ptr [0x49808c]

// block 0042b4b7
0042b4b7  dec        byte ptr [0x4cf221]

// block 0042b4bd
0042b4bd  mov        ebx, dword ptr [esp + 0x30]
0042b4c1  jmp        0x42b4c9

// block 0042b4c3
0042b4c3  fstp       st(1)

// block 0042b4c5
0042b4c5  fstp       st(0)

// block 0042b4c7
0042b4c7  fstp       st(0)

// block 0042b4c9
0042b4c9  fld        dword ptr [esp + 0x34]
0042b4cd  inc        ebx
0042b4ce  fadd       dword ptr [esp + 0x14]
0042b4d2  mov        dword ptr [esp + 0x30], ebx
0042b4d6  fstp       dword ptr [esp + 0x14]
0042b4da  fld        dword ptr [esp + 0x38]
0042b4de  fadd       dword ptr [esp + 0x18]
0042b4e2  fstp       dword ptr [esp + 0x18]
0042b4e6  fld        dword ptr [esp + 0x1c]
0042b4ea  fadd       dword ptr [esp + 0x3c]
0042b4ee  fstp       dword ptr [esp + 0x1c]
0042b4f2  fld        dword ptr [esp + 0xc]
0042b4f6  fld        qword ptr [0x4a3d68]
0042b4fc  fadd       st(1), st(0)
0042b4fe  fxch       st(1)
0042b500  fstp       dword ptr [esp + 0xc]
0042b504  fld        dword ptr [esp + 0xc]
0042b508  fadd       qword ptr [0x4a4078]
0042b50e  fld        dword ptr [edi + 0x6c]
0042b511  fcompp     
0042b513  fnstsw     ax
0042b515  test       ah, 0x41
0042b518  je         0x42b343

// block 0042b51e
0042b51e  cmp        dword ptr [esp + 0x2c], 0
0042b523  je         0x42b55a

// block 0042b525
0042b525  xor        esi, esi
0042b527  test       ebx, ebx
0042b529  jle        0x42b544

// block 0042b52b
0042b52b  cmp        byte ptr [esp + esi + 0x250], 0
0042b533  je         0x42b53a

// block 0042b535
0042b535  inc        esi
0042b536  cmp        esi, ebx
0042b538  jl         0x42b52b

// block 0042b53a
0042b53a  test       esi, esi
0042b53c  je         0x42b544

// block 0042b53e
0042b53e  fstp       st(0)
0042b540  fldz       
0042b542  jmp        0x42b583

// block 0042b544
0042b544  xor        eax, eax
0042b546  test       ebx, ebx
0042b548  jle        0x42b55a

// block 0042b54a
0042b54a  cmp        byte ptr [esp + esi + 0x250], 0
0042b552  jne        0x42b577

// block 0042b554
0042b554  inc        esi
0042b555  inc        eax
0042b556  cmp        esi, ebx
0042b558  jl         0x42b54a

// block 0042b55a
0042b55a  fstp       st(0)

// block 0042b55c
0042b55c  mov        ecx, dword ptr [esp + 0x354]
0042b563  mov        eax, dword ptr [esp + 0x2c]
0042b567  pop        edi
0042b568  pop        esi
0042b569  pop        ebx
0042b56a  xor        ecx, esp
0042b56c  call       0x46c932

// block 0042b571
0042b571  mov        esp, ebp
0042b573  pop        ebp
0042b574  ret        0x10

// block 0042b577
0042b577  cmp        esi, ebx
0042b579  mov        dword ptr [esp + 0xc], eax
0042b57d  jge        0x42b55a

// block 0042b57f
0042b57f  fimul      dword ptr [esp + 0xc]

// block 0042b583
0042b583  cmp        esi, ebx
0042b585  fstp       dword ptr [edi + 0x6c]
0042b588  jge        0x42b55c

// block 0042b58a
0042b58a  lea        ebx, [ebx]

// block 0042b590
0042b590  cmp        byte ptr [esp + esi + 0x250], 0
0042b598  je         0x42b5a1

// block 0042b59a
0042b59a  inc        esi
0042b59b  cmp        esi, ebx
0042b59d  jl         0x42b590

// block 0042b59f
0042b59f  jmp        0x42b55c

// block 0042b5a1
0042b5a1  cmp        esi, ebx
0042b5a3  jge        0x42b55c

// block 0042b5a5
0042b5a5  xor        eax, eax
0042b5a7  mov        dword ptr [esp + 0x10], esi
0042b5ab  jmp        0x42b5b0

// block 0042b5b0
0042b5b0  cmp        byte ptr [esp + esi + 0x250], 0
0042b5b8  jne        0x42b5c0

// block 0042b5ba
0042b5ba  inc        esi
0042b5bb  inc        eax
0042b5bc  cmp        esi, ebx
0042b5be  jl         0x42b5b0

// block 0042b5c0
0042b5c0  push       0x1e8
0042b5c5  lea        ecx, [esp + 0x6c]
0042b5c9  push       0
0042b5cb  push       ecx
0042b5cc  mov        dword ptr [esp + 0x18], eax
0042b5d0  call       0x477420

// block 0042b5d5
0042b5d5  fild       dword ptr [esp + 0x18]
0042b5d9  fld        qword ptr [0x4a3d68]
0042b5df  add        esp, 0xc
0042b5e2  fmul       st(1), st(0)
0042b5e4  fxch       st(1)
0042b5e6  fstp       dword ptr [esp + 0x7c]
0042b5ea  fld        dword ptr [esp + 0x7c]
0042b5ee  fstp       dword ptr [esp + 0x78]
0042b5f2  fild       dword ptr [esp + 0x10]
0042b5f6  fstp       dword ptr [esp + 0x10]
0042b5fa  fld        dword ptr [esp + 0x10]
0042b5fe  fst        dword ptr [esp + 0x10]
0042b602  fld        dword ptr [esp + 0x34]
0042b606  fld        dword ptr [esp + 0x10]
0042b60a  fld        st(0)
0042b60c  fmulp      st(2)
0042b60e  fxch       st(1)
0042b610  fstp       dword ptr [esp + 0x20]
0042b614  fld        dword ptr [esp + 0x38]
0042b618  fmul       st(1)
0042b61a  fstp       dword ptr [esp + 0x24]
0042b61e  fmul       dword ptr [esp + 0x3c]
0042b622  fstp       dword ptr [esp + 0x28]
0042b626  fld        dword ptr [esp + 0x20]
0042b62a  fadd       dword ptr [esp + 0x40]
0042b62e  fstp       dword ptr [esp + 0x14]
0042b632  mov        edx, dword ptr [esp + 0x14]
0042b636  fld        dword ptr [esp + 0x44]
0042b63a  mov        dword ptr [esp + 0x68], edx
0042b63e  fadd       dword ptr [esp + 0x24]
0042b642  mov        dx, word ptr [edi + 0x4a4]
0042b649  mov        word ptr [esp + 0x8c], dx
0042b651  fstp       dword ptr [esp + 0x18]
0042b655  mov        eax, dword ptr [esp + 0x18]
0042b659  fld        dword ptr [esp + 0x48]
0042b65d  mov        dword ptr [esp + 0x6c], eax
0042b661  fadd       dword ptr [esp + 0x28]
0042b665  mov        ax, word ptr [edi + 0x4a6]
0042b66c  mov        word ptr [esp + 0x8e], ax
0042b674  mov        eax, dword ptr [0x4b44f4]
0042b679  fstp       dword ptr [esp + 0x1c]
0042b67d  mov        ecx, dword ptr [esp + 0x1c]
0042b681  fld        dword ptr [0x4a3e54]
0042b687  mov        dword ptr [esp + 0x70], ecx
0042b68b  fstp       dword ptr [esp + 0x88]
0042b692  lea        ecx, [eax + 0x468]
0042b698  fld        dword ptr [edi + 0x68]
0042b69b  mov        dword ptr [esp + 0x10], eax
0042b69f  fstp       dword ptr [esp + 0x74]
0042b6a3  mov        dword ptr [esp + 0xc], ecx
0042b6a7  fld        dword ptr [edi + 0x70]
0042b6aa  fstp       dword ptr [esp + 0x84]
0042b6b1  fmulp      st(1)
0042b6b3  fsubr      dword ptr [edi + 0x474]
0042b6b9  fstp       dword ptr [esp + 0x80]
0042b6c0  cmp        dword ptr [ecx], 0x100
0042b6c6  jge        0x42b736

// block 0042b6c8
0042b6c8  inc        dword ptr [eax + 0x46c]
0042b6ce  cmp        dword ptr [eax + 0x46c], 0x10000
0042b6d8  lea        ebx, [eax + 0x46c]
0042b6de  jge        0x42b6e6

// block 0042b6e0
0042b6e0  mov        dword ptr [ebx], 0x10000

// block 0042b6e6
0042b6e6  push       0xfa4
0042b6eb  call       0x46c9ea

// block 0042b6f0
0042b6f0  add        esp, 4
0042b6f3  test       eax, eax
0042b6f5  je         0x42b6fe

// block 0042b6f7
0042b6f7  call       0x428520

// block 0042b6fc
0042b6fc  jmp        0x42b700

// block 0042b6fe
0042b6fe  xor        eax, eax

// block 0042b700
0042b700  mov        ecx, dword ptr [ebx]
0042b702  mov        edx, dword ptr [esp + 0x10]
0042b706  mov        dword ptr [eax + 0x80], ecx
0042b70c  mov        ecx, dword ptr [edx + 0x464]
0042b712  mov        dword ptr [eax + 4], ecx
0042b715  mov        dword ptr [ecx + 8], eax
0042b718  mov        ecx, dword ptr [esp + 0xc]
0042b71c  inc        dword ptr [ecx]
0042b71e  mov        dword ptr [edx + 0x464], eax
0042b724  mov        edx, dword ptr [eax]
0042b726  mov        edx, dword ptr [edx + 4]
0042b729  lea        ecx, [esp + 0x68]
0042b72d  push       ecx
0042b72e  mov        ecx, eax
0042b730  call       edx

// block 0042b732
0042b732  mov        ebx, dword ptr [esp + 0x30]

// block 0042b736
0042b736  cmp        esi, ebx
0042b738  jl         0x42b590

// block 0042b73e
0042b73e  jmp        0x42b55c
