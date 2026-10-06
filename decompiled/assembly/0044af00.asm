
// block 0044af00
0044af00  push       ebp
0044af01  mov        ebp, esp
0044af03  and        esp, 0xfffffff8
0044af06  sub        esp, 0x70
0044af09  push       ebx
0044af0a  push       ebp
0044af0b  push       esi
0044af0c  mov        esi, dword ptr [0x4b4534]
0044af12  push       edi
0044af13  push       0x50
0044af15  xor        edi, edi
0044af17  lea        eax, [esp + 0x34]
0044af1b  push       edi
0044af1c  push       eax
0044af1d  call       0x477420

// block 0044af22
0044af22  mov        eax, dword ptr [esi + 0x34]
0044af25  fld        dword ptr [eax + 0x1074]
0044af2b  add        esp, 0xc
0044af2e  lea        ecx, [esp + 0x30]
0044af32  fstp       dword ptr [esp + 0x30]
0044af36  fld        dword ptr [eax + 0x1078]
0044af3c  push       ecx
0044af3d  push       0x4a2168
0044af42  fstp       dword ptr [esp + 0x3c]
0044af46  mov        dword ptr [esp + 0x4c], 0xbb8
0044af4e  mov        dword ptr [esp + 0x48], edi
0044af52  mov        dword ptr [esp + 0x44], edi
0044af56  call       0x412990

// block 0044af5b
0044af5b  mov        edx, dword ptr [esi + 0x34]
0044af5e  fld        dword ptr [edx + 0x1074]
0044af64  push       ecx
0044af65  lea        eax, [edi + 6]
0044af68  fstp       dword ptr [esp]
0044af6b  call       0x453e20

// block 0044af70
0044af70  mov        edi, dword ptr [esi + 0x34]
0044af73  mov        eax, dword ptr [0x4b43c8]
0044af78  mov        ebp, dword ptr [eax + 0x4debdc]
0044af7e  add        edi, 0x1074
0044af84  test       dword ptr [0x4cee78], 0x8000
0044af8e  je         0x44afa1

// block 0044af90
0044af90  push       0x4cf1d0
0044af95  call       dword ptr [0x498088]

// block 0044af9b
0044af9b  inc        byte ptr [0x4cf221]

// block 0044afa1
0044afa1  inc        dword ptr [ebp + 0x130]
0044afa7  call       0x4621c0

// block 0044afac
0044afac  fldz       
0044afae  mov        ebx, eax
0044afb0  or         dword ptr [ebx + 0x480], 1
0044afb7  mov        dword ptr [ebx + 0x20], 0x17
0044afbe  test       edi, edi
0044afc0  jne        0x44afee

// block 0044afc2
0044afc2  fst        dword ptr [esp + 0x18]
0044afc6  mov        ecx, dword ptr [esp + 0x18]
0044afca  fst        dword ptr [esp + 0x1c]
0044afce  mov        edx, dword ptr [esp + 0x1c]
0044afd2  fstp       dword ptr [esp + 0x20]
0044afd6  mov        eax, dword ptr [esp + 0x20]
0044afda  mov        dword ptr [ebx + 0x430], ecx
0044afe0  mov        dword ptr [ebx + 0x434], edx
0044afe6  mov        dword ptr [ebx + 0x438], eax
0044afec  jmp        0x44b01c

// block 0044afee
0044afee  fstp       st(0)
0044aff0  fld        dword ptr [edi]
0044aff2  fadd       qword ptr [0x4a3d70]
0044aff8  fadd       qword ptr [0x4a3d28]
0044affe  fstp       dword ptr [ebx + 0x430]
0044b004  fld        dword ptr [edi + 4]
0044b007  fadd       qword ptr [0x4a3d68]
0044b00d  fstp       dword ptr [ebx + 0x434]
0044b013  fld        dword ptr [edi + 8]
0044b016  fstp       dword ptr [ebx + 0x438]

// block 0044b01c
0044b01c  push       0xd0
0044b021  push       ebx
0044b022  mov        ecx, ebp
0044b024  call       0x454d10

// block 0044b029
0044b029  lea        eax, [esp + 0x14]
0044b02d  call       0x461250

// block 0044b032
0044b032  test       dword ptr [0x4cee78], 0x8000
0044b03c  je         0x44b04f

// block 0044b03e
0044b03e  push       0x4cf1d0
0044b043  call       dword ptr [0x49808c]

// block 0044b049
0044b049  dec        byte ptr [0x4cf221]

// block 0044b04f
0044b04f  mov        eax, dword ptr [esi + 0x34]
0044b052  fld        dword ptr [eax + 0x1074]
0044b058  add        eax, 0x1074
0044b05d  fadd       qword ptr [0x4a3d70]
0044b063  mov        edi, 0xa
0044b068  fadd       qword ptr [0x4a3d28]
0044b06e  fstp       dword ptr [esp + 0x24]
0044b072  fld        dword ptr [eax + 4]
0044b075  fadd       qword ptr [0x4a3d68]
0044b07b  fstp       dword ptr [esp + 0x28]
0044b07f  fld        dword ptr [eax + 8]
0044b082  fstp       dword ptr [esp + 0x2c]

// block 0044b086
0044b086  lea        ecx, [esp + 0x24]
0044b08a  push       ecx
0044b08b  push       1
0044b08d  lea        edx, [esp + 0x18]
0044b091  push       edx
0044b092  call       0x40fbe0

// block 0044b097
0044b097  sub        edi, 1
0044b09a  jne        0x44b086

// block 0044b09c
0044b09c  mov        eax, dword ptr [esi + 0x4c]
0044b09f  fld        dword ptr [0x4a4060]
0044b0a5  test       eax, eax
0044b0a7  je         0x44b0c7

// block 0044b0a9
0044b0a9  cmp        dword ptr [esi + 0x48], edi
0044b0ac  je         0x44b0e6

// block 0044b0ae
0044b0ae  mov        eax, dword ptr [esi + 0x34]
0044b0b1  fldz       
0044b0b3  add        eax, 0x1074
0044b0b8  sub        esp, 8
0044b0bb  fstp       dword ptr [esp + 4]
0044b0bf  fstp       dword ptr [esp]
0044b0c2  push       eax
0044b0c3  push       0x12
0044b0c5  jmp        0x44b101

// block 0044b0c7
0044b0c7  cmp        dword ptr [esi + 0x48], 0
0044b0cb  je         0x44b0e6

// block 0044b0cd
0044b0cd  mov        eax, dword ptr [esi + 0x34]
0044b0d0  fldz       
0044b0d2  add        eax, 0x1074
0044b0d7  sub        esp, 8
0044b0da  fstp       dword ptr [esp + 4]
0044b0de  fstp       dword ptr [esp]
0044b0e1  push       eax
0044b0e2  push       0x10
0044b0e4  jmp        0x44b101

// block 0044b0e6
0044b0e6  test       eax, eax
0044b0e8  je         0x44b12d

// block 0044b0ea
0044b0ea  mov        eax, dword ptr [esi + 0x34]
0044b0ed  fldz       
0044b0ef  add        eax, 0x1074
0044b0f4  sub        esp, 8
0044b0f7  fstp       dword ptr [esp + 4]
0044b0fb  fstp       dword ptr [esp]
0044b0fe  push       eax
0044b0ff  push       0x11

// block 0044b101
0044b101  call       0x4273f0

// block 0044b106
0044b106  fld        dword ptr [0x4a4060]
0044b10c  mov        edi, eax
0044b10e  test       edi, edi
0044b110  je         0x44b12d

// block 0044b112
0044b112  mov        eax, dword ptr [esi + 0x48]
0044b115  mov        dword ptr [edi + 0x9cc], eax
0044b11b  mov        ecx, dword ptr [esi + 0x4c]
0044b11e  mov        dword ptr [edi + 0x9d0], ecx
0044b124  mov        edx, dword ptr [esi + 0x30]
0044b127  mov        dword ptr [edi + 0x9c8], edx

// block 0044b12d
0044b12d  mov        eax, dword ptr [esi + 0x30]
0044b130  inc        eax
0044b131  cmp        eax, 4
0044b134  ja         0x44b584

// block 0044b13a
0044b13a  jmp        dword ptr [eax*4 + 0x44b590]

// block 0044b141
0044b141  test       edi, edi
0044b143  je         0x44b1f6

// block 0044b149
0044b149  mov        eax, dword ptr [0x4b0c48]
0044b14e  cmp        eax, dword ptr [0x4b0cd0]
0044b154  jl         0x44b15e

// block 0044b156
0044b156  fld        dword ptr [0x4a4014]
0044b15c  jmp        0x44b160

// block 0044b15e
0044b15e  fld1       

// block 0044b160
0044b160  fstp       dword ptr [esp + 0x10]
0044b164  mov        ebx, 0xa
0044b169  fld        dword ptr [esp + 0x10]
0044b16d  fstp       dword ptr [edi + 0x9d4]
0044b173  mov        eax, dword ptr [esi + 0x34]
0044b176  fld        dword ptr [eax + 0x1074]
0044b17c  add        eax, 0x1074
0044b181  fadd       qword ptr [0x4a3d70]
0044b187  fadd       qword ptr [0x4a3d28]
0044b18d  fstp       dword ptr [esi + 0x54]
0044b190  fld        dword ptr [eax + 4]
0044b193  fadd       qword ptr [0x4a3d68]
0044b199  fstp       dword ptr [esi + 0x58]
0044b19c  fld        dword ptr [eax + 8]
0044b19f  mov        eax, 0x51eb851f
0044b1a4  fstp       dword ptr [esi + 0x5c]
0044b1a7  fld        dword ptr [edi + 0x9d4]
0044b1ad  mov        dword ptr [esi + 0x64], 0x78
0044b1b4  fstp       dword ptr [esi + 0x60]
0044b1b7  mov        ecx, dword ptr [0x4b0c78]
0044b1bd  imul       ecx
0044b1bf  sar        edx, 5
0044b1c2  mov        ecx, edx
0044b1c4  shr        ecx, 0x1f
0044b1c7  add        ecx, edx
0044b1c9  mov        eax, ecx
0044b1cb  cdq        
0044b1cc  idiv       ebx
0044b1ce  mov        ebx, ecx
0044b1d0  mov        ecx, dword ptr [esi + 0x48]
0044b1d3  sub        ebx, edx
0044b1d5  imul       ecx, ebx
0044b1d8  mov        dword ptr [esp + 0x10], ecx
0044b1dc  fild       dword ptr [esp + 0x10]
0044b1e0  fmul       dword ptr [edi + 0x9d4]
0044b1e6  call       0x4931e0

// block 0044b1eb
0044b1eb  mov        edx, dword ptr [esi + 0x4c]
0044b1ee  imul       edx, ebx
0044b1f1  add        eax, edx
0044b1f3  mov        dword ptr [esi + 0x68], eax

// block 0044b1f6
0044b1f6  fld        dword ptr [0x4a42c0]
0044b1fc  mov        eax, dword ptr [esi + 0x34]
0044b1ff  sub        esp, 8
0044b202  fstp       dword ptr [esp + 4]
0044b206  add        eax, 0x1074
0044b20b  fstp       dword ptr [esp]
0044b20e  push       eax
0044b20f  push       4
0044b211  call       0x4273f0

// block 0044b216
0044b216  fld        dword ptr [0x4a42c0]
0044b21c  mov        ecx, dword ptr [esi + 0x34]
0044b21f  sub        esp, 8
0044b222  fstp       dword ptr [esp + 4]
0044b226  add        ecx, 0x1074
0044b22c  fld        dword ptr [0x4a4060]
0044b232  fstp       dword ptr [esp]
0044b235  push       ecx
0044b236  push       0xd
0044b238  call       0x4273f0

// block 0044b23d
0044b23d  pop        edi
0044b23e  pop        esi
0044b23f  pop        ebp
0044b240  pop        ebx
0044b241  mov        esp, ebp
0044b243  pop        ebp
0044b244  ret        

// block 0044b245
0044b245  test       edi, edi
0044b247  je         0x44b30e

// block 0044b24d
0044b24d  fld1       
0044b24f  fcomp      dword ptr [esi + 0x50]
0044b252  fnstsw     ax
0044b254  test       ah, 0x41
0044b257  jp         0x44b261

// block 0044b259
0044b259  fld        qword ptr [0x4a4078]
0044b25f  jmp        0x44b28c

// block 0044b261
0044b261  fld        dword ptr [0x4a4088]
0044b267  fcomp      dword ptr [esi + 0x50]
0044b26a  fnstsw     ax
0044b26c  test       ah, 0x41
0044b26f  jp         0x44b279

// block 0044b271
0044b271  fld        qword ptr [0x4a43c0]
0044b277  jmp        0x44b28c

// block 0044b279
0044b279  fld        dword ptr [esi + 0x50]
0044b27c  fmul       qword ptr [0x4a4108]
0044b282  fld        qword ptr [0x4a3d00]
0044b288  fmul       st(1), st(0)
0044b28a  faddp      st(1)

// block 0044b28c
0044b28c  fstp       dword ptr [edi + 0x9d4]
0044b292  mov        eax, dword ptr [esi + 0x34]
0044b295  fld        dword ptr [eax + 0x1074]
0044b29b  add        eax, 0x1074
0044b2a0  fadd       qword ptr [0x4a3d70]
0044b2a6  mov        ebx, 0xa
0044b2ab  fadd       qword ptr [0x4a3d28]
0044b2b1  fstp       dword ptr [esi + 0x54]
0044b2b4  fld        dword ptr [eax + 4]
0044b2b7  fadd       qword ptr [0x4a3d68]
0044b2bd  fstp       dword ptr [esi + 0x58]
0044b2c0  fld        dword ptr [eax + 8]
0044b2c3  mov        eax, 0x51eb851f
0044b2c8  fstp       dword ptr [esi + 0x5c]
0044b2cb  fld        dword ptr [edi + 0x9d4]
0044b2d1  mov        dword ptr [esi + 0x64], 0x78
0044b2d8  fstp       dword ptr [esi + 0x60]
0044b2db  mov        ecx, dword ptr [0x4b0c78]
0044b2e1  imul       ecx
0044b2e3  sar        edx, 5
0044b2e6  mov        ecx, edx
0044b2e8  shr        ecx, 0x1f
0044b2eb  add        ecx, edx
0044b2ed  mov        eax, ecx
0044b2ef  cdq        
0044b2f0  idiv       ebx
0044b2f2  sub        ecx, edx
0044b2f4  imul       ecx, dword ptr [esi + 0x4c]
0044b2f8  mov        dword ptr [esp + 0x10], ecx
0044b2fc  fild       dword ptr [esp + 0x10]
0044b300  fmul       dword ptr [edi + 0x9d4]
0044b306  call       0x4931e0

// block 0044b30b
0044b30b  mov        dword ptr [esi + 0x68], eax

// block 0044b30e
0044b30e  fld        dword ptr [0x4a42c0]
0044b314  mov        edx, dword ptr [esi + 0x34]
0044b317  sub        esp, 8
0044b31a  fstp       dword ptr [esp + 4]
0044b31e  add        edx, 0x1074
0044b324  fstp       dword ptr [esp]
0044b327  push       edx
0044b328  push       0xe
0044b32a  call       0x4273f0

// block 0044b32f
0044b32f  pop        edi
0044b330  pop        esi
0044b331  pop        ebp
0044b332  pop        ebx
0044b333  mov        esp, ebp
0044b335  pop        ebp
0044b336  ret        

// block 0044b337
0044b337  test       edi, edi
0044b339  je         0x44b3c7

// block 0044b33f
0044b33f  fld        dword ptr [0x4a4014]
0044b345  mov        ebx, 0xa
0044b34a  fstp       dword ptr [edi + 0x9d4]
0044b350  mov        eax, dword ptr [esi + 0x34]
0044b353  fld        dword ptr [eax + 0x1074]
0044b359  add        eax, 0x1074
0044b35e  fadd       qword ptr [0x4a3d70]
0044b364  fadd       qword ptr [0x4a3d28]
0044b36a  fstp       dword ptr [esi + 0x54]
0044b36d  fld        dword ptr [eax + 4]
0044b370  fadd       qword ptr [0x4a3d68]
0044b376  fstp       dword ptr [esi + 0x58]
0044b379  fld        dword ptr [eax + 8]
0044b37c  mov        eax, 0x51eb851f
0044b381  fstp       dword ptr [esi + 0x5c]
0044b384  fld        dword ptr [edi + 0x9d4]
0044b38a  mov        dword ptr [esi + 0x64], 0x78
0044b391  fstp       dword ptr [esi + 0x60]
0044b394  mov        ecx, dword ptr [0x4b0c78]
0044b39a  imul       ecx
0044b39c  sar        edx, 5
0044b39f  mov        ecx, edx
0044b3a1  shr        ecx, 0x1f
0044b3a4  add        ecx, edx
0044b3a6  mov        eax, ecx
0044b3a8  cdq        
0044b3a9  idiv       ebx
0044b3ab  sub        ecx, edx
0044b3ad  imul       ecx, dword ptr [esi + 0x4c]
0044b3b1  mov        dword ptr [esp + 0x10], ecx
0044b3b5  fild       dword ptr [esp + 0x10]
0044b3b9  fmul       dword ptr [edi + 0x9d4]
0044b3bf  call       0x4931e0

// block 0044b3c4
0044b3c4  mov        dword ptr [esi + 0x68], eax

// block 0044b3c7
0044b3c7  fld        dword ptr [0x4a42c0]
0044b3cd  mov        eax, dword ptr [esi + 0x34]
0044b3d0  sub        esp, 8
0044b3d3  fstp       dword ptr [esp + 4]
0044b3d7  add        eax, 0x1074
0044b3dc  fstp       dword ptr [esp]
0044b3df  push       eax
0044b3e0  push       5
0044b3e2  call       0x4273f0

// block 0044b3e7
0044b3e7  fld        dword ptr [0x4a42c0]
0044b3ed  mov        ecx, dword ptr [esi + 0x34]
0044b3f0  sub        esp, 8
0044b3f3  fstp       dword ptr [esp + 4]
0044b3f7  add        ecx, 0x1074
0044b3fd  fld        dword ptr [0x4a4060]
0044b403  fstp       dword ptr [esp]
0044b406  push       ecx
0044b407  push       0xf
0044b409  call       0x4273f0

// block 0044b40e
0044b40e  pop        edi
0044b40f  pop        esi
0044b410  pop        ebp
0044b411  pop        ebx
0044b412  mov        esp, ebp
0044b414  pop        ebp
0044b415  ret        

// block 0044b416
0044b416  mov        eax, dword ptr [0x4b0c54]
0044b41b  sub        eax, 1
0044b41e  je         0x44b46f

// block 0044b420
0044b420  sub        eax, 1
0044b423  je         0x44b44d

// block 0044b425
0044b425  sub        eax, 1
0044b428  jne        0x44b492

// block 0044b42a
0044b42a  fld        dword ptr [0x4a42c0]
0044b430  mov        edx, dword ptr [esi + 0x34]
0044b433  sub        esp, 8
0044b436  fstp       dword ptr [esp + 4]
0044b43a  add        edx, 0x1074
0044b440  fstp       dword ptr [esp]
0044b443  push       edx
0044b444  push       0xf
0044b446  call       0x4273f0

// block 0044b44b
0044b44b  jmp        0x44b494

// block 0044b44d
0044b44d  fld        dword ptr [0x4a42c0]
0044b453  mov        eax, dword ptr [esi + 0x34]
0044b456  sub        esp, 8
0044b459  fstp       dword ptr [esp + 4]
0044b45d  add        eax, 0x1074
0044b462  fstp       dword ptr [esp]
0044b465  push       eax
0044b466  push       0xe
0044b468  call       0x4273f0

// block 0044b46d
0044b46d  jmp        0x44b494

// block 0044b46f
0044b46f  fld        dword ptr [0x4a42c0]
0044b475  mov        ecx, dword ptr [esi + 0x34]
0044b478  sub        esp, 8
0044b47b  fstp       dword ptr [esp + 4]
0044b47f  add        ecx, 0x1074
0044b485  fstp       dword ptr [esp]
0044b488  push       ecx
0044b489  push       0xd
0044b48b  call       0x4273f0

// block 0044b490
0044b490  jmp        0x44b494

// block 0044b492
0044b492  fstp       st(0)

// block 0044b494
0044b494  test       edi, edi
0044b496  je         0x44b586

// block 0044b49c
0044b49c  fld1       
0044b49e  fcomp      dword ptr [esi + 0x50]
0044b4a1  fnstsw     ax
0044b4a3  test       ah, 0x41
0044b4a6  jp         0x44b4b0

// block 0044b4a8
0044b4a8  fld        qword ptr [0x4a4108]
0044b4ae  jmp        0x44b4d5

// block 0044b4b0
0044b4b0  fld        dword ptr [0x4a4088]
0044b4b6  fcomp      dword ptr [esi + 0x50]
0044b4b9  fnstsw     ax
0044b4bb  test       ah, 0x41
0044b4be  jp         0x44b4c8

// block 0044b4c0
0044b4c0  fld        qword ptr [0x4a3fd8]
0044b4c6  jmp        0x44b4d5

// block 0044b4c8
0044b4c8  fld        dword ptr [esi + 0x50]
0044b4cb  fld        qword ptr [0x4a3d00]
0044b4d1  fmul       st(1), st(0)
0044b4d3  faddp      st(1)

// block 0044b4d5
0044b4d5  fstp       dword ptr [edi + 0x9d4]
0044b4db  mov        edx, dword ptr [esi + 0x48]
0044b4de  mov        dword ptr [edi + 0x9cc], edx
0044b4e4  mov        eax, dword ptr [esi + 0x4c]
0044b4e7  mov        dword ptr [edi + 0x9d0], eax
0044b4ed  mov        ecx, dword ptr [esi + 0x30]
0044b4f0  mov        dword ptr [edi + 0x9c8], ecx
0044b4f6  cmp        dword ptr [esi + 0x4c], 0
0044b4fa  je         0x44b586

// block 0044b500
0044b500  mov        eax, dword ptr [esi + 0x34]
0044b503  fld        dword ptr [eax + 0x1074]
0044b509  add        eax, 0x1074
0044b50e  fadd       qword ptr [0x4a3d70]
0044b514  mov        ebx, 0xa
0044b519  fadd       qword ptr [0x4a3d28]
0044b51f  fstp       dword ptr [esi + 0x54]
0044b522  fld        dword ptr [eax + 4]
0044b525  fadd       qword ptr [0x4a3d68]
0044b52b  fstp       dword ptr [esi + 0x58]
0044b52e  fld        dword ptr [eax + 8]
0044b531  mov        eax, 0x51eb851f
0044b536  fstp       dword ptr [esi + 0x5c]
0044b539  fld        dword ptr [edi + 0x9d4]
0044b53f  mov        dword ptr [esi + 0x64], 0x78
0044b546  fstp       dword ptr [esi + 0x60]
0044b549  mov        ecx, dword ptr [0x4b0c78]
0044b54f  imul       ecx
0044b551  sar        edx, 5
0044b554  mov        ecx, edx
0044b556  shr        ecx, 0x1f
0044b559  add        ecx, edx
0044b55b  mov        eax, ecx
0044b55d  cdq        
0044b55e  idiv       ebx
0044b560  sub        ecx, edx
0044b562  imul       ecx, dword ptr [esi + 0x4c]
0044b566  mov        dword ptr [esp + 0x10], ecx
0044b56a  fild       dword ptr [esp + 0x10]
0044b56e  fmul       dword ptr [edi + 0x9d4]
0044b574  call       0x4931e0

// block 0044b579
0044b579  mov        dword ptr [esi + 0x68], eax
0044b57c  pop        edi
0044b57d  pop        esi
0044b57e  pop        ebp
0044b57f  pop        ebx
0044b580  mov        esp, ebp
0044b582  pop        ebp
0044b583  ret        

// block 0044b584
0044b584  fstp       st(0)

// block 0044b586
0044b586  pop        edi
0044b587  pop        esi
0044b588  pop        ebp
0044b589  pop        ebx
0044b58a  mov        esp, ebp
0044b58c  pop        ebp
0044b58d  ret        
