
// block 0042d9c0
0042d9c0  sub        esp, 0x144
0042d9c6  mov        eax, dword ptr [0x4ad138]
0042d9cb  xor        eax, esp
0042d9cd  mov        dword ptr [esp + 0x140], eax
0042d9d4  push       ebx
0042d9d5  mov        ebx, dword ptr [esp + 0x150]
0042d9dc  push       ebp
0042d9dd  push       esi
0042d9de  mov        esi, dword ptr [esp + 0x154]
0042d9e5  xor        ebp, ebp
0042d9e7  push       edi
0042d9e8  mov        edi, ecx
0042d9ea  cmp        dword ptr [esp + 0x164], ebp
0042d9f1  je         0x42da02

// block 0042d9f3
0042d9f3  cmp        dword ptr [edi + 0x44c], ebp
0042d9f9  je         0x42da02

// block 0042d9fb
0042d9fb  xor        eax, eax
0042d9fd  jmp        0x42dedd

// block 0042da02
0042da02  push       0x100
0042da07  lea        eax, [esp + 0x54]
0042da0b  push       ebp
0042da0c  push       eax
0042da0d  mov        dword ptr [esp + 0x30], ebp
0042da11  call       0x477420

// block 0042da16
0042da16  fld        dword ptr [ebx]
0042da18  fld        qword ptr [0x4a3cf8]
0042da1e  mov        ecx, dword ptr [edi + 0xf9c]
0042da24  fmul       st(1), st(0)
0042da26  add        esp, 0xc
0042da29  cmp        dword ptr [edi + 0x470], ebp
0042da2f  fxch       st(1)
0042da31  mov        dword ptr [esp + 0x14], ecx
0042da35  fstp       dword ptr [esp + 0x18]
0042da39  fmul       dword ptr [ebx + 4]
0042da3c  fstp       dword ptr [esp + 0x1c]
0042da40  fld        dword ptr [esi]
0042da42  fld        dword ptr [esp + 0x18]
0042da46  fld        st(0)
0042da48  fsubp      st(2)
0042da4a  fxch       st(1)
0042da4c  fstp       dword ptr [esp + 0x3c]
0042da50  fld        dword ptr [esi + 4]
0042da53  fld        dword ptr [esp + 0x1c]
0042da57  fld        st(0)
0042da59  fsubp      st(2)
0042da5b  fxch       st(1)
0042da5d  fstp       dword ptr [esp + 0x40]
0042da61  fld        dword ptr [esi]
0042da63  faddp      st(2)
0042da65  fxch       st(1)
0042da67  fstp       dword ptr [esp + 0x30]
0042da6b  fadd       dword ptr [esi + 4]
0042da6e  fstp       dword ptr [esp + 0x34]
0042da72  jle        0x42ded9

// block 0042da78
0042da78  jmp        0x42da80

// block 0042da80
0042da80  mov        edx, dword ptr [ecx]
0042da82  fld        dword ptr [0x4a3d78]
0042da88  mov        eax, dword ptr [ecx + 4]
0042da8b  mov        dword ptr [esp + 0x18], edx
0042da8f  fld        dword ptr [esp + 0x18]
0042da93  fld        dword ptr [esp + 0x3c]
0042da97  mov        edx, dword ptr [ecx + 8]
0042da9a  fcomp      st(1)
0042da9c  mov        dword ptr [esp + 0x1c], eax
0042daa0  mov        dword ptr [esp + 0x20], edx
0042daa4  fnstsw     ax
0042daa6  test       ah, 0x41
0042daa9  je         0x42dc0e

// block 0042daaf
0042daaf  fld        dword ptr [esp + 0x30]
0042dab3  fcompp     
0042dab5  fnstsw     ax
0042dab7  test       ah, 5
0042daba  jnp        0x42dc10

// block 0042dac0
0042dac0  fld        dword ptr [esp + 0x1c]
0042dac4  fld        dword ptr [esp + 0x40]
0042dac8  fcomp      st(1)
0042daca  fnstsw     ax
0042dacc  test       ah, 0x41
0042dacf  je         0x42dc0e

// block 0042dad5
0042dad5  fld        dword ptr [esp + 0x34]
0042dad9  fcompp     
0042dadb  fnstsw     ax
0042dadd  test       ah, 5
0042dae0  jnp        0x42dc10

// block 0042dae6
0042dae6  mov        eax, 0x55555556
0042daeb  imul       ebp
0042daed  mov        eax, edx
0042daef  shr        eax, 0x1f
0042daf2  add        eax, edx
0042daf4  mov        ebx, 1
0042daf9  add        dword ptr [esp + 0x24], ebx
0042dafd  lea        eax, [eax + eax*2]
0042db00  mov        edx, ebp
0042db02  sub        edx, eax
0042db04  mov        byte ptr [esp + ebp + 0x50], 1
0042db09  jne        0x42dc10

// block 0042db0f
0042db0f  cmp        dword ptr [esp + 0x160], edx
0042db16  je         0x42db51

// block 0042db18
0042db18  sub        esp, 8
0042db1b  fst        dword ptr [esp + 4]
0042db1f  lea        ecx, [esp + 0x20]
0042db23  fstp       dword ptr [esp]
0042db26  call       0x42e820

// block 0042db2b
0042db2b  test       eax, eax
0042db2d  jne        0x42db53

// block 0042db2f
0042db2f  fld        dword ptr [0x4a41f4]
0042db35  sub        esp, 8
0042db38  fstp       dword ptr [esp + 4]
0042db3c  mov        eax, ecx
0042db3e  fld        dword ptr [0x4a4060]
0042db44  fstp       dword ptr [esp]
0042db47  push       eax
0042db48  push       9
0042db4a  call       0x4273f0

// block 0042db4f
0042db4f  jmp        0x42db53

// block 0042db51
0042db51  fstp       st(0)

// block 0042db53
0042db53  test       dword ptr [0x4cee78], 0x8000
0042db5d  movsx      ecx, word ptr [edi + 0x46e]
0042db64  mov        eax, dword ptr [0x4b44f4]
0042db69  mov        esi, dword ptr [eax + 0x488]
0042db6f  lea        edx, [ecx + ecx + 4]
0042db73  mov        dword ptr [esp + 0x10], edx
0042db77  je         0x42db8a

// block 0042db79
0042db79  push       0x4cf1d0
0042db7e  call       dword ptr [0x498088]

// block 0042db84
0042db84  inc        byte ptr [0x4cf221]

// block 0042db8a
0042db8a  add        dword ptr [esi + 0x130], ebx
0042db90  call       0x4621c0

// block 0042db95
0042db95  fld        dword ptr [esp + 0x18]
0042db99  fadd       qword ptr [0x4a3d70]
0042db9f  mov        ecx, dword ptr [esp + 0x10]
0042dba3  mov        ebx, eax
0042dba5  or         dword ptr [ebx + 0x480], 1
0042dbac  fadd       qword ptr [0x4a3d28]
0042dbb2  mov        dword ptr [ebx + 0x20], 0x17
0042dbb9  push       ecx
0042dbba  push       ebx
0042dbbb  fstp       dword ptr [ebx + 0x430]
0042dbc1  mov        ecx, esi
0042dbc3  fld        dword ptr [esp + 0x24]
0042dbc7  fadd       qword ptr [0x4a3d68]
0042dbcd  fstp       dword ptr [ebx + 0x434]
0042dbd3  fld        dword ptr [esp + 0x28]
0042dbd7  fstp       dword ptr [ebx + 0x438]
0042dbdd  call       0x454d10

// block 0042dbe2
0042dbe2  lea        eax, [esp + 0x48]
0042dbe6  call       0x461250

// block 0042dbeb
0042dbeb  test       dword ptr [0x4cee78], 0x8000
0042dbf5  je         0x42dc08

// block 0042dbf7
0042dbf7  push       0x4cf1d0
0042dbfc  call       dword ptr [0x49808c]

// block 0042dc02
0042dc02  dec        byte ptr [0x4cf221]

// block 0042dc08
0042dc08  mov        ecx, dword ptr [esp + 0x14]
0042dc0c  jmp        0x42dc12

// block 0042dc0e
0042dc0e  fstp       st(0)

// block 0042dc10
0042dc10  fstp       st(0)

// block 0042dc12
0042dc12  add        ecx, 0x14
0042dc15  inc        ebp
0042dc16  cmp        ebp, dword ptr [edi + 0x470]
0042dc1c  mov        dword ptr [esp + 0x14], ecx
0042dc20  jl         0x42da80

// block 0042dc26
0042dc26  mov        eax, dword ptr [esp + 0x24]
0042dc2a  xor        ebx, ebx
0042dc2c  cmp        eax, ebx
0042dc2e  je         0x42ded9

// block 0042dc34
0042dc34  cmp        eax, ebp
0042dc36  jl         0x42dc41

// block 0042dc38
0042dc38  mov        byte ptr [edi + 0x7c], 1
0042dc3c  jmp        0x42dedd

// block 0042dc41
0042dc41  xor        esi, esi
0042dc43  cmp        ebp, ebx
0042dc45  jle        0x42dcba

// block 0042dc47
0042dc47  cmp        byte ptr [esp + esi + 0x50], 0
0042dc4c  je         0x42dc53

// block 0042dc4e
0042dc4e  inc        esi
0042dc4f  cmp        esi, ebp
0042dc51  jl         0x42dc47

// block 0042dc53
0042dc53  cmp        esi, ebx
0042dc55  je         0x42dcba

// block 0042dc57
0042dc57  mov        edx, dword ptr [edi + 0x470]
0042dc5d  sub        edx, esi
0042dc5f  mov        dword ptr [esp + 0x10], ebx
0042dc63  test       edx, edx
0042dc65  jle        0x42ded3

// block 0042dc6b
0042dc6b  lea        edx, [esi + esi*4]
0042dc6e  add        edx, edx
0042dc70  add        edx, edx

// block 0042dc72
0042dc72  mov        ecx, dword ptr [edi + 0xf9c]
0042dc78  mov        ebp, dword ptr [ecx + edx]
0042dc7b  lea        eax, [ecx + edx]
0042dc7e  add        ecx, ebx
0042dc80  mov        dword ptr [ecx], ebp
0042dc82  mov        ebp, dword ptr [eax + 4]
0042dc85  mov        dword ptr [ecx + 4], ebp
0042dc88  mov        ebp, dword ptr [eax + 8]
0042dc8b  mov        dword ptr [ecx + 8], ebp
0042dc8e  mov        ebp, dword ptr [eax + 0xc]
0042dc91  mov        dword ptr [ecx + 0xc], ebp
0042dc94  mov        eax, dword ptr [eax + 0x10]
0042dc97  mov        dword ptr [ecx + 0x10], eax
0042dc9a  mov        eax, dword ptr [esp + 0x10]
0042dc9e  mov        ecx, dword ptr [edi + 0x470]
0042dca4  inc        eax
0042dca5  sub        ecx, esi
0042dca7  add        ebx, 0x14
0042dcaa  add        edx, 0x14
0042dcad  cmp        eax, ecx
0042dcaf  mov        dword ptr [esp + 0x10], eax
0042dcb3  jl         0x42dc72

// block 0042dcb5
0042dcb5  jmp        0x42ded3

// block 0042dcba
0042dcba  test       ebp, ebp
0042dcbc  jle        0x42ded9

// block 0042dcc2
0042dcc2  cmp        byte ptr [esp + esi + 0x50], 0
0042dcc7  jne        0x42dcd4

// block 0042dcc9
0042dcc9  inc        esi
0042dcca  inc        ebx
0042dccb  cmp        esi, ebp
0042dccd  jl         0x42dcc2

// block 0042dccf
0042dccf  jmp        0x42ded9

// block 0042dcd4
0042dcd4  cmp        esi, ebp
0042dcd6  mov        dword ptr [esp + 0x14], ebx
0042dcda  jge        0x42ded9

// block 0042dce0
0042dce0  test       ebx, ebx
0042dce2  je         0x42ded9

// block 0042dce8
0042dce8  cmp        byte ptr [esp + esi + 0x50], 0
0042dced  je         0x42dcff

// block 0042dcef
0042dcef  inc        esi
0042dcf0  cmp        esi, ebp
0042dcf2  jl         0x42dce8

// block 0042dcf4
0042dcf4  mov        dword ptr [edi + 0x470], ebx
0042dcfa  jmp        0x42ded9

// block 0042dcff
0042dcff  cmp        esi, ebp
0042dd01  jl         0x42dd0e

// block 0042dd03
0042dd03  mov        dword ptr [edi + 0x470], ebx
0042dd09  jmp        0x42ded9

// block 0042dd0e
0042dd0e  xor        ecx, ecx
0042dd10  mov        dword ptr [esp + 0x28], ecx
0042dd14  test       ebx, ebx
0042dd16  jle        0x42de6d

// block 0042dd1c
0042dd1c  mov        dword ptr [esp + 0x10], ecx

// block 0042dd20
0042dd20  mov        eax, dword ptr [edi + 0xf9c]
0042dd26  add        eax, dword ptr [esp + 0x10]
0042dd2a  mov        edx, dword ptr [eax]
0042dd2c  mov        dword ptr [esp + 0x18], edx
0042dd30  mov        edx, dword ptr [eax + 4]
0042dd33  mov        eax, dword ptr [eax + 8]
0042dd36  mov        dword ptr [esp + 0x20], eax
0042dd3a  mov        dword ptr [esp + 0x1c], edx
0042dd3e  mov        eax, 0x55555556
0042dd43  imul       ecx
0042dd45  mov        eax, edx
0042dd47  shr        eax, 0x1f
0042dd4a  add        eax, edx
0042dd4c  lea        edx, [eax + eax*2]
0042dd4f  mov        eax, ecx
0042dd51  sub        eax, edx
0042dd53  jne        0x42de5b

// block 0042dd59
0042dd59  cmp        dword ptr [esp + 0x160], eax
0042dd60  je         0x42dd9d

// block 0042dd62
0042dd62  fld        dword ptr [0x4a3d78]
0042dd68  sub        esp, 8
0042dd6b  fst        dword ptr [esp + 4]
0042dd6f  lea        ecx, [esp + 0x20]
0042dd73  fstp       dword ptr [esp]
0042dd76  call       0x42e820

// block 0042dd7b
0042dd7b  test       eax, eax
0042dd7d  jne        0x42dd9d

// block 0042dd7f
0042dd7f  fld        dword ptr [0x4a41f4]
0042dd85  sub        esp, 8
0042dd88  fstp       dword ptr [esp + 4]
0042dd8c  fld        dword ptr [0x4a4060]
0042dd92  fstp       dword ptr [esp]
0042dd95  push       ecx
0042dd96  push       9
0042dd98  call       0x4273f0

// block 0042dd9d
0042dd9d  test       dword ptr [0x4cee78], 0x8000
0042dda7  movsx      edx, word ptr [edi + 0x46e]
0042ddae  mov        ecx, dword ptr [0x4b44f4]
0042ddb4  mov        ebp, dword ptr [ecx + 0x488]
0042ddba  lea        eax, [edx + edx + 4]
0042ddbe  mov        dword ptr [esp + 0x2c], eax
0042ddc2  je         0x42ddd5

// block 0042ddc4
0042ddc4  push       0x4cf1d0
0042ddc9  call       dword ptr [0x498088]

// block 0042ddcf
0042ddcf  inc        byte ptr [0x4cf221]

// block 0042ddd5
0042ddd5  inc        dword ptr [ebp + 0x130]
0042dddb  call       0x4621c0

// block 0042dde0
0042dde0  fld        dword ptr [esp + 0x18]
0042dde4  fadd       qword ptr [0x4a3d70]
0042ddea  mov        edx, dword ptr [esp + 0x2c]
0042ddee  mov        ebx, eax
0042ddf0  or         dword ptr [ebx + 0x480], 1
0042ddf7  fadd       qword ptr [0x4a3d28]
0042ddfd  mov        dword ptr [ebx + 0x20], 0x17
0042de04  push       edx
0042de05  push       ebx
0042de06  fstp       dword ptr [ebx + 0x430]
0042de0c  mov        ecx, ebp
0042de0e  fld        dword ptr [esp + 0x24]
0042de12  fadd       qword ptr [0x4a3d68]
0042de18  fstp       dword ptr [ebx + 0x434]
0042de1e  fld        dword ptr [esp + 0x28]
0042de22  fstp       dword ptr [ebx + 0x438]
0042de28  call       0x454d10

// block 0042de2d
0042de2d  lea        eax, [esp + 0x4c]
0042de31  call       0x461250

// block 0042de36
0042de36  test       dword ptr [0x4cee78], 0x8000
0042de40  je         0x42de53

// block 0042de42
0042de42  push       0x4cf1d0
0042de47  call       dword ptr [0x49808c]

// block 0042de4d
0042de4d  dec        byte ptr [0x4cf221]

// block 0042de53
0042de53  mov        ecx, dword ptr [esp + 0x28]
0042de57  mov        ebx, dword ptr [esp + 0x14]

// block 0042de5b
0042de5b  add        dword ptr [esp + 0x10], 0x14
0042de60  inc        ecx
0042de61  cmp        ecx, ebx
0042de63  mov        dword ptr [esp + 0x28], ecx
0042de67  jl         0x42dd20

// block 0042de6d
0042de6d  mov        eax, dword ptr [edi + 0x470]
0042de73  xor        ebx, ebx
0042de75  sub        eax, esi
0042de77  mov        dword ptr [esp + 0x10], ebx
0042de7b  test       eax, eax
0042de7d  jle        0x42ded3

// block 0042de7f
0042de7f  lea        edx, [esi + esi*4]
0042de82  add        edx, edx
0042de84  add        edx, edx
0042de86  jmp        0x42de90

// block 0042de90
0042de90  mov        ecx, dword ptr [edi + 0xf9c]
0042de96  mov        ebp, dword ptr [ecx + edx]
0042de99  lea        eax, [ecx + edx]
0042de9c  add        ecx, ebx
0042de9e  mov        dword ptr [ecx], ebp
0042dea0  mov        ebp, dword ptr [eax + 4]
0042dea3  mov        dword ptr [ecx + 4], ebp
0042dea6  mov        ebp, dword ptr [eax + 8]
0042dea9  mov        dword ptr [ecx + 8], ebp
0042deac  mov        ebp, dword ptr [eax + 0xc]
0042deaf  mov        dword ptr [ecx + 0xc], ebp
0042deb2  mov        eax, dword ptr [eax + 0x10]
0042deb5  mov        dword ptr [ecx + 0x10], eax
0042deb8  mov        eax, dword ptr [esp + 0x10]
0042debc  mov        ecx, dword ptr [edi + 0x470]
0042dec2  inc        eax
0042dec3  sub        ecx, esi
0042dec5  add        ebx, 0x14
0042dec8  add        edx, 0x14
0042decb  cmp        eax, ecx
0042decd  mov        dword ptr [esp + 0x10], eax
0042ded1  jl         0x42de90

// block 0042ded3
0042ded3  sub        dword ptr [edi + 0x470], esi

// block 0042ded9
0042ded9  mov        eax, dword ptr [esp + 0x24]

// block 0042dedd
0042dedd  mov        ecx, dword ptr [esp + 0x150]
0042dee4  pop        edi
0042dee5  pop        esi
0042dee6  pop        ebp
0042dee7  pop        ebx
0042dee8  xor        ecx, esp
0042deea  call       0x46c932

// block 0042deef
0042deef  add        esp, 0x144
0042def5  ret        0x10
