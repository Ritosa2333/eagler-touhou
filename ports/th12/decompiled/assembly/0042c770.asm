
// block 0042c770
0042c770  sub        esp, 0x20
0042c773  push       ebx
0042c774  push       ebp
0042c775  push       esi
0042c776  push       edi
0042c777  mov        esi, ecx
0042c779  mov        ebx, 0x40000
0042c77e  mov        ebp, 0x800000

// block 0042c783
0042c783  mov        eax, dword ptr [esi]
0042c785  mov        edx, dword ptr [eax]
0042c787  mov        ecx, esi
0042c789  call       edx

// block 0042c78b
0042c78b  mov        eax, dword ptr [esi + 0x430]
0042c791  test       eax, eax
0042c793  je         0x42c925

// block 0042c799
0042c799  xor        edi, edi
0042c79b  test       al, 1
0042c79d  je         0x42c7aa

// block 0042c79f
0042c79f  mov        eax, dword ptr [esi]
0042c7a1  mov        edx, dword ptr [eax + 0x28]
0042c7a4  mov        ecx, esi
0042c7a6  call       edx

// block 0042c7a8
0042c7a8  mov        edi, eax

// block 0042c7aa
0042c7aa  test       byte ptr [esi + 0x430], 4
0042c7b1  je         0x42c7be

// block 0042c7b3
0042c7b3  mov        eax, dword ptr [esi]
0042c7b5  mov        edx, dword ptr [eax + 0x2c]
0042c7b8  mov        ecx, esi
0042c7ba  call       edx

// block 0042c7bc
0042c7bc  add        edi, eax

// block 0042c7be
0042c7be  test       byte ptr [esi + 0x430], 8
0042c7c5  je         0x42c7d2

// block 0042c7c7
0042c7c7  mov        eax, dword ptr [esi]
0042c7c9  mov        edx, dword ptr [eax + 0x30]
0042c7cc  mov        ecx, esi
0042c7ce  call       edx

// block 0042c7d0
0042c7d0  add        edi, eax

// block 0042c7d2
0042c7d2  test       byte ptr [esi + 0x430], 0x10
0042c7d9  je         0x42c7e6

// block 0042c7db
0042c7db  mov        eax, dword ptr [esi]
0042c7dd  mov        edx, dword ptr [eax + 0x34]
0042c7e0  mov        ecx, esi
0042c7e2  call       edx

// block 0042c7e4
0042c7e4  add        edi, eax

// block 0042c7e6
0042c7e6  test       byte ptr [esi + 0x430], 0x40
0042c7ed  je         0x42c7fa

// block 0042c7ef
0042c7ef  mov        eax, dword ptr [esi]
0042c7f1  mov        edx, dword ptr [eax + 0x38]
0042c7f4  mov        ecx, esi
0042c7f6  call       edx

// block 0042c7f8
0042c7f8  add        edi, eax

// block 0042c7fa
0042c7fa  test       byte ptr [esi + 0x430], 0x20
0042c801  je         0x42c80e

// block 0042c803
0042c803  mov        eax, dword ptr [esi]
0042c805  mov        edx, dword ptr [eax + 0x3c]
0042c808  mov        ecx, esi
0042c80a  call       edx

// block 0042c80c
0042c80c  add        edi, eax

// block 0042c80e
0042c80e  test       dword ptr [esi + 0x430], 0x100
0042c818  je         0x42c825

// block 0042c81a
0042c81a  mov        eax, dword ptr [esi]
0042c81c  mov        edx, dword ptr [eax + 0x40]
0042c81f  mov        ecx, esi
0042c821  call       edx

// block 0042c823
0042c823  add        edi, eax

// block 0042c825
0042c825  test       dword ptr [esi + 0x430], 0x20000
0042c82f  je         0x42c83c

// block 0042c831
0042c831  mov        eax, dword ptr [esi]
0042c833  mov        edx, dword ptr [eax + 0x44]
0042c836  mov        ecx, esi
0042c838  call       edx

// block 0042c83a
0042c83a  add        edi, eax

// block 0042c83c
0042c83c  test       dword ptr [esi + 0x430], ebx
0042c842  je         0x42c84f

// block 0042c844
0042c844  mov        eax, dword ptr [esi]
0042c846  mov        edx, dword ptr [eax + 0x48]
0042c849  mov        ecx, esi
0042c84b  call       edx

// block 0042c84d
0042c84d  add        edi, eax

// block 0042c84f
0042c84f  test       dword ptr [esi + 0x430], ebp
0042c855  je         0x42c862

// block 0042c857
0042c857  mov        eax, dword ptr [esi]
0042c859  mov        edx, dword ptr [eax + 0x4c]
0042c85c  mov        ecx, esi
0042c85e  call       edx

// block 0042c860
0042c860  add        edi, eax

// block 0042c862
0042c862  test       dword ptr [esi + 0x430], 0x400
0042c86c  je         0x42c879

// block 0042c86e
0042c86e  mov        eax, dword ptr [esi]
0042c870  mov        edx, dword ptr [eax + 0x50]
0042c873  mov        ecx, esi
0042c875  call       edx

// block 0042c877
0042c877  add        edi, eax

// block 0042c879
0042c879  mov        eax, dword ptr [esi + 0x430]
0042c87f  test       eax, 0x1000
0042c884  je         0x42c90c

// block 0042c88a
0042c88a  cmp        dword ptr [esi + 0x18c], 0
0042c891  jg         0x42c8a1

// block 0042c893
0042c893  xor        eax, 0x1000
0042c898  mov        dword ptr [esi + 0x430], eax
0042c89e  inc        edi
0042c89f  jmp        0x42c90c

// block 0042c8a1
0042c8a1  mov        eax, dword ptr [esi + 0x18c]
0042c8a7  mov        ecx, dword ptr [esi + 0x194]
0042c8ad  mov        dword ptr [esi + 0x188], eax
0042c8b3  fld        dword ptr [ecx]
0042c8b5  fcomp      qword ptr [0x4a3db0]
0042c8bb  fnstsw     ax
0042c8bd  test       ah, 0x41
0042c8c0  jne        0x42c8df

// block 0042c8c2
0042c8c2  fld        dword ptr [0x4a3dac]
0042c8c8  fcomp      dword ptr [ecx]
0042c8ca  fnstsw     ax
0042c8cc  test       ah, 0x41
0042c8cf  jne        0x42c8df

// block 0042c8d1
0042c8d1  fld        dword ptr [esi + 0x190]
0042c8d7  fsub       qword ptr [0x4a3ce0]
0042c8dd  jmp        0x42c8f5

// block 0042c8df
0042c8df  fld        dword ptr [esi + 0x190]
0042c8e5  fld        dword ptr [ecx]
0042c8e7  fmul       qword ptr [0x4a3ce0]
0042c8ed  fstp       dword ptr [esp + 0x10]
0042c8f1  fsub       dword ptr [esp + 0x10]

// block 0042c8f5
0042c8f5  fstp       dword ptr [esi + 0x190]
0042c8fb  fld        dword ptr [esi + 0x190]
0042c901  call       0x4931e0

// block 0042c906
0042c906  mov        dword ptr [esi + 0x18c], eax

// block 0042c90c
0042c90c  mov        eax, dword ptr [esi + 0x44c]
0042c912  test       eax, eax
0042c914  je         0x42c91d

// block 0042c916
0042c916  dec        eax
0042c917  mov        dword ptr [esi + 0x44c], eax

// block 0042c91d
0042c91d  test       edi, edi
0042c91f  jne        0x42c783

// block 0042c925
0042c925  mov        ecx, dword ptr [esi + 0x470]
0042c92b  mov        edx, dword ptr [esi + 0xf9c]
0042c931  dec        ecx
0042c932  test       ecx, ecx
0042c934  jle        0x42c965

// block 0042c936
0042c936  lea        eax, [ecx + ecx*4]
0042c939  lea        eax, [edx + eax*4]
0042c93c  lea        esp, [esp]

// block 0042c940
0042c940  mov        edi, dword ptr [eax - 0x14]
0042c943  mov        dword ptr [eax], edi
0042c945  mov        edi, dword ptr [eax - 0x10]
0042c948  mov        dword ptr [eax + 4], edi
0042c94b  mov        edi, dword ptr [eax - 0xc]
0042c94e  mov        dword ptr [eax + 8], edi
0042c951  mov        edi, dword ptr [eax - 8]
0042c954  mov        dword ptr [eax + 0xc], edi
0042c957  mov        edi, dword ptr [eax - 4]
0042c95a  mov        dword ptr [eax + 0x10], edi
0042c95d  dec        ecx
0042c95e  sub        eax, 0x14
0042c961  test       ecx, ecx
0042c963  jg         0x42c940

// block 0042c965
0042c965  fld        dword ptr [edx]
0042c967  fadd       dword ptr [esi + 0x5c]
0042c96a  fstp       dword ptr [edx]
0042c96c  fld        dword ptr [esi + 0x60]
0042c96f  fadd       dword ptr [edx + 4]
0042c972  fstp       dword ptr [edx + 4]
0042c975  fld        dword ptr [esi + 0x64]
0042c978  fadd       dword ptr [edx + 8]
0042c97b  fstp       dword ptr [edx + 8]
0042c97e  mov        ecx, dword ptr [edx]
0042c980  fld        dword ptr [esi + 0x68]
0042c983  mov        dword ptr [esi + 0x50], ecx
0042c986  mov        eax, dword ptr [edx + 4]
0042c989  mov        dword ptr [esi + 0x54], eax
0042c98c  mov        ecx, dword ptr [edx + 8]
0042c98f  mov        dword ptr [esi + 0x58], ecx
0042c992  fstp       dword ptr [edx + 0xc]
0042c995  fld        dword ptr [esi + 0x74]
0042c998  fstp       dword ptr [edx + 0x10]
0042c99b  cmp        dword ptr [esi + 0x43c], 0
0042c9a2  mov        edx, dword ptr [esi + 0xf9c]
0042c9a8  jg         0x42ca8f

// block 0042c9ae
0042c9ae  test       dword ptr [esi + 0x430], 0x400
0042c9b8  jne        0x42ca8f

// block 0042c9be
0042c9be  xor        edi, edi
0042c9c0  cmp        dword ptr [esi + 0x470], edi
0042c9c6  jle        0x42ca82

// block 0042c9cc
0042c9cc  lea        esp, [esp]

// block 0042c9d0
0042c9d0  fld        dword ptr [esi + 0x6c]
0042c9d3  sub        esp, 8
0042c9d6  fstp       dword ptr [esp + 4]
0042c9da  lea        ecx, [esp + 0x20]
0042c9de  fld        dword ptr [esi + 0x68]
0042c9e1  fstp       dword ptr [esp]
0042c9e4  call       0x42e880

// block 0042c9e9
0042c9e9  fld        dword ptr [esi + 0x50]
0042c9ec  fadd       dword ptr [esp + 0x18]
0042c9f0  fstp       dword ptr [esp + 0x18]
0042c9f4  fld        dword ptr [esi + 0x54]
0042c9f7  fadd       dword ptr [esp + 0x1c]
0042c9fb  fstp       dword ptr [esp + 0x1c]
0042c9ff  fld        dword ptr [esp + 0x20]
0042ca03  fadd       dword ptr [esi + 0x58]
0042ca06  fstp       dword ptr [esp + 0x20]
0042ca0a  fld        dword ptr [esi + 0x70]
0042ca0d  fstp       dword ptr [esp + 0x10]
0042ca11  fld        dword ptr [esi + 0x70]
0042ca14  fstp       dword ptr [esp + 0x14]
0042ca18  fld        dword ptr [edx]
0042ca1a  fld        dword ptr [esp + 0x10]
0042ca1e  fld        st(0)
0042ca20  faddp      st(2)
0042ca22  fxch       st(1)
0042ca24  fcomp      qword ptr [0x4a3d60]
0042ca2a  fnstsw     ax
0042ca2c  test       ah, 0x41
0042ca2f  jnp        0x42ca70

// block 0042ca31
0042ca31  fsubr      dword ptr [edx]
0042ca33  fcomp      qword ptr [0x4a3d28]
0042ca39  fnstsw     ax
0042ca3b  test       ah, 1
0042ca3e  je         0x42ca72

// block 0042ca40
0042ca40  fld        dword ptr [edx + 4]
0042ca43  fld        dword ptr [esp + 0x14]
0042ca47  fld        st(0)
0042ca49  faddp      st(2)
0042ca4b  fxch       st(1)
0042ca4d  fcomp      qword ptr [0x4a3d58]
0042ca53  fnstsw     ax
0042ca55  test       ah, 0x41
0042ca58  jnp        0x42ca70

// block 0042ca5a
0042ca5a  fsubr      dword ptr [edx + 4]
0042ca5d  fcomp      qword ptr [0x4a3d50]
0042ca63  fnstsw     ax
0042ca65  test       ah, 1
0042ca68  jne        0x42cafa

// block 0042ca6e
0042ca6e  jmp        0x42ca72

// block 0042ca70
0042ca70  fstp       st(0)

// block 0042ca72
0042ca72  inc        edi
0042ca73  add        edx, 0x14
0042ca76  cmp        edi, dword ptr [esi + 0x470]
0042ca7c  jl         0x42c9d0

// block 0042ca82
0042ca82  pop        edi
0042ca83  pop        esi
0042ca84  pop        ebp
0042ca85  mov        eax, 1
0042ca8a  pop        ebx
0042ca8b  add        esp, 0x20
0042ca8e  ret        

// block 0042ca8f
0042ca8f  mov        edx, dword ptr [esi + 0x43c]
0042ca95  mov        ecx, dword ptr [esi + 0x444]
0042ca9b  mov        dword ptr [esi + 0x438], edx
0042caa1  fld        dword ptr [ecx]
0042caa3  fcomp      qword ptr [0x4a3db0]
0042caa9  fnstsw     ax
0042caab  test       ah, 0x41
0042caae  jne        0x42cacd

// block 0042cab0
0042cab0  fld        dword ptr [0x4a3dac]
0042cab6  fcomp      dword ptr [ecx]
0042cab8  fnstsw     ax
0042caba  test       ah, 0x41
0042cabd  jne        0x42cacd

// block 0042cabf
0042cabf  fld        dword ptr [esi + 0x440]
0042cac5  fsub       qword ptr [0x4a3ce0]
0042cacb  jmp        0x42cae3

// block 0042cacd
0042cacd  fld        dword ptr [esi + 0x440]
0042cad3  fld        dword ptr [ecx]
0042cad5  fmul       qword ptr [0x4a3ce0]
0042cadb  fstp       dword ptr [esp + 0x14]
0042cadf  fsub       dword ptr [esp + 0x14]

// block 0042cae3
0042cae3  fstp       dword ptr [esi + 0x440]
0042cae9  fld        dword ptr [esi + 0x440]
0042caef  call       0x4931e0

// block 0042caf4
0042caf4  mov        dword ptr [esi + 0x43c], eax

// block 0042cafa
0042cafa  mov        eax, dword ptr [esi + 0x470]
0042cb00  fldz       
0042cb02  mov        edi, dword ptr [esi + 0xf9c]
0042cb08  fstp       dword ptr [esp + 0x10]
0042cb0c  dec        eax
0042cb0d  xor        ebp, ebp
0042cb0f  xor        ebx, ebx
0042cb11  test       eax, eax
0042cb13  jle        0x42cc30

// block 0042cb19
0042cb19  lea        esp, [esp]

// block 0042cb20
0042cb20  fld        dword ptr [edi + 0x10]
0042cb23  sub        esp, 8
0042cb26  fmul       qword ptr [0x4a3cf8]
0042cb2c  lea        ecx, [esp + 0x20]
0042cb30  fstp       dword ptr [esp + 0x1c]
0042cb34  fld        dword ptr [esp + 0x1c]
0042cb38  fstp       dword ptr [esp + 4]
0042cb3c  fld        dword ptr [edi + 0xc]
0042cb3f  fstp       dword ptr [esp]
0042cb42  call       0x42e880

// block 0042cb47
0042cb47  fld        dword ptr [edi]
0042cb49  fadd       dword ptr [esp + 0x18]
0042cb4d  fstp       dword ptr [esp + 0x18]
0042cb51  fld        dword ptr [edi + 4]
0042cb54  fadd       dword ptr [esp + 0x1c]
0042cb58  fstp       dword ptr [esp + 0x1c]
0042cb5c  fld        dword ptr [edi + 8]
0042cb5f  fadd       dword ptr [esp + 0x20]
0042cb63  fstp       dword ptr [esp + 0x20]
0042cb67  fld        dword ptr [edi + 0x10]
0042cb6a  fadd       dword ptr [esp + 0x10]
0042cb6e  fstp       dword ptr [esp + 0x10]
0042cb72  fld        dword ptr [0x4a3e68]
0042cb78  fcomp      dword ptr [esp + 0x10]
0042cb7c  fnstsw     ax
0042cb7e  test       ah, 0x41
0042cb81  jp         0x42cc1d

// block 0042cb87
0042cb87  fld        dword ptr [edi + 0x10]
0042cb8a  sub        esp, 0xc
0042cb8d  fstp       dword ptr [esp + 8]
0042cb91  lea        eax, [esp + 0x24]
0042cb95  fld        dword ptr [esi + 0x70]
0042cb98  fmul       qword ptr [0x4a3cf8]
0042cb9e  fstp       dword ptr [esp + 0x20]
0042cba2  fld        dword ptr [esp + 0x20]
0042cba6  fstp       dword ptr [esp + 4]
0042cbaa  fld        dword ptr [edi + 0xc]
0042cbad  fstp       dword ptr [esp]
0042cbb0  call       0x437a80

// block 0042cbb5
0042cbb5  cmp        eax, 1
0042cbb8  jne        0x42cbee

// block 0042cbba
0042cbba  fld        dword ptr [0x4a3d78]
0042cbc0  mov        ecx, dword ptr [0x4b4514]
0042cbc6  mov        edx, dword ptr [esi]
0042cbc8  fst        dword ptr [esp + 0x24]
0042cbcc  mov        edx, dword ptr [edx + 0x18]
0042cbcf  fstp       dword ptr [esp + 0x28]
0042cbd3  fldz       
0042cbd5  push       eax
0042cbd6  push       0
0042cbd8  fstp       dword ptr [esp + 0x34]
0042cbdc  lea        eax, [esp + 0x2c]
0042cbe0  add        ecx, 0x97c
0042cbe6  push       eax
0042cbe7  push       ecx
0042cbe8  mov        ecx, esi
0042cbea  call       edx

// block 0042cbec
0042cbec  jmp        0x42cc1d

// block 0042cbee
0042cbee  cmp        eax, 2
0042cbf1  jne        0x42cc1d

// block 0042cbf3
0042cbf3  test       ebp, ebp
0042cbf5  jne        0x42cc1d

// block 0042cbf7
0042cbf7  mov        eax, dword ptr [esi + 0x2c]
0042cbfa  cdq        
0042cbfb  mov        ecx, 3
0042cc00  idiv       ecx
0042cc02  test       edx, edx
0042cc04  jne        0x42cc1d

// block 0042cc06
0042cc06  mov        edx, dword ptr [0x4b4514]
0042cc0c  add        edx, 0x97c
0042cc12  push       edx
0042cc13  call       0x4391c0

// block 0042cc18
0042cc18  mov        ebp, 1

// block 0042cc1d
0042cc1d  mov        eax, dword ptr [esi + 0x470]
0042cc23  inc        ebx
0042cc24  dec        eax
0042cc25  add        edi, 0x14
0042cc28  cmp        ebx, eax
0042cc2a  jl         0x42cb20

// block 0042cc30
0042cc30  lea        ecx, [esi + 0xae8]
0042cc36  push       ecx
0042cc37  call       0x455630

// block 0042cc3c
0042cc3c  mov        edx, dword ptr [esi + 0x2c]
0042cc3f  mov        ecx, dword ptr [esi + 0x34]
0042cc42  mov        dword ptr [esi + 0x28], edx
0042cc45  fld        dword ptr [ecx]
0042cc47  fcomp      qword ptr [0x4a3db0]
0042cc4d  fnstsw     ax
0042cc4f  test       ah, 0x41
0042cc52  jne        0x42cc7d

// block 0042cc54
0042cc54  fld        dword ptr [0x4a3dac]
0042cc5a  fcomp      dword ptr [ecx]
0042cc5c  fnstsw     ax
0042cc5e  test       ah, 0x41
0042cc61  jne        0x42cc7d

// block 0042cc63
0042cc63  fld        dword ptr [esi + 0x30]
0042cc66  inc        edx
0042cc67  fadd       qword ptr [0x4a3ce0]
0042cc6d  pop        edi
0042cc6e  mov        dword ptr [esi + 0x2c], edx
0042cc71  xor        eax, eax
0042cc73  fstp       dword ptr [esi + 0x30]
0042cc76  pop        esi
0042cc77  pop        ebp
0042cc78  pop        ebx
0042cc79  add        esp, 0x20
0042cc7c  ret        

// block 0042cc7d
0042cc7d  fld        dword ptr [ecx]
0042cc7f  fadd       dword ptr [esi + 0x30]
0042cc82  fstp       dword ptr [esp + 0x14]
0042cc86  fld        dword ptr [esp + 0x14]
0042cc8a  fst        dword ptr [esi + 0x30]
0042cc8d  call       0x4931e0

// block 0042cc92
0042cc92  pop        edi
0042cc93  mov        dword ptr [esi + 0x2c], eax
0042cc96  pop        esi
0042cc97  pop        ebp
0042cc98  xor        eax, eax
0042cc9a  pop        ebx
0042cc9b  add        esp, 0x20
0042cc9e  ret        
