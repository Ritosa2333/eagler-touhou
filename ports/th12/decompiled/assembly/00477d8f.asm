
// block 00477d8f
00477d8f  mov        edi, edi
00477d91  push       ebp
00477d92  mov        ebp, esp
00477d94  sub        esp, 0x1fc
00477d9a  mov        eax, dword ptr [0x4ad138]
00477d9f  xor        eax, ebp
00477da1  mov        dword ptr [ebp - 4], eax
00477da4  mov        ecx, dword ptr [ebp + 0x14]
00477da7  mov        eax, dword ptr [ebp + 8]
00477daa  push       esi
00477dab  xor        esi, esi
00477dad  push       edi
00477dae  mov        edi, dword ptr [ebp + 0xc]
00477db1  mov        dword ptr [ebp - 0x1e4], ecx
00477db7  lea        ecx, [ebp - 0x184]
00477dbd  mov        dword ptr [ebp - 0x19c], eax
00477dc3  mov        dword ptr [ebp - 0x1ac], ecx
00477dc9  mov        dword ptr [ebp - 0x1dc], 0x15e
00477dd3  mov        dword ptr [ebp - 0x1d0], esi
00477dd9  mov        dword ptr [ebp - 0x1e8], esi
00477ddf  mov        dword ptr [ebp - 0x188], esi
00477de5  cmp        edi, esi
00477de7  jne        0x477e09

// block 00477de9
00477de9  call       0x46e96f

// block 00477dee
00477dee  push       esi
00477def  push       esi
00477df0  push       esi
00477df1  push       esi
00477df2  push       esi
00477df3  mov        dword ptr [eax], 0x16
00477df9  call       0x470f2d

// block 00477dfe
00477dfe  add        esp, 0x14
00477e01  or         eax, 0xffffffff
00477e04  jmp        0x478d3e

// block 00477e09
00477e09  cmp        eax, esi
00477e0b  je         0x477de9

// block 00477e0d
00477e0d  test       byte ptr [eax + 0xc], 0x40
00477e11  push       ebx
00477e12  jne        0x477e8e

// block 00477e14
00477e14  push       eax
00477e15  call       0x47c1da

// block 00477e1a
00477e1a  pop        ecx
00477e1b  mov        edx, 0x4adbb0
00477e20  cmp        eax, -1
00477e23  je         0x477e40

// block 00477e25
00477e25  cmp        eax, -2
00477e28  je         0x477e40

// block 00477e2a
00477e2a  mov        ecx, eax
00477e2c  and        ecx, 0x1f
00477e2f  mov        ebx, eax
00477e31  sar        ebx, 5
00477e34  shl        ecx, 6
00477e37  add        ecx, dword ptr [ebx*4 + 0x4d6320]
00477e3e  jmp        0x477e42

// block 00477e40
00477e40  mov        ecx, edx

// block 00477e42
00477e42  test       byte ptr [ecx + 0x24], 0x7f
00477e46  jne        0x477e6e

// block 00477e48
00477e48  cmp        eax, -1
00477e4b  je         0x477e66

// block 00477e4d
00477e4d  cmp        eax, -2
00477e50  je         0x477e66

// block 00477e52
00477e52  mov        ecx, eax
00477e54  and        eax, 0x1f
00477e57  sar        ecx, 5
00477e5a  shl        eax, 6
00477e5d  add        eax, dword ptr [ecx*4 + 0x4d6320]
00477e64  jmp        0x477e68

// block 00477e66
00477e66  mov        eax, edx

// block 00477e68
00477e68  test       byte ptr [eax + 0x24], 0x80
00477e6c  je         0x477e8e

// block 00477e6e
00477e6e  call       0x46e96f

// block 00477e73
00477e73  push       esi
00477e74  push       esi
00477e75  push       esi
00477e76  push       esi
00477e77  push       esi
00477e78  mov        dword ptr [eax], 0x16
00477e7e  call       0x470f2d

// block 00477e83
00477e83  add        esp, 0x14
00477e86  or         eax, 0xffffffff
00477e89  jmp        0x478d3d

// block 00477e8e
00477e8e  push       dword ptr [ebp + 0x10]
00477e91  lea        ecx, [ebp - 0x1f8]
00477e97  call       0x46d3b4

// block 00477e9c
00477e9c  mov        al, byte ptr [edi]
00477e9e  mov        byte ptr [ebp - 0x19d], 0
00477ea5  mov        dword ptr [ebp - 0x18c], esi
00477eab  mov        dword ptr [ebp - 0x1c4], esi
00477eb1  test       al, al
00477eb3  je         0x478d24

// block 00477eb9
00477eb9  mov        esi, dword ptr [ebp - 0x19c]
00477ebf  movzx      eax, al
00477ec2  push       eax
00477ec3  call       0x48bb76

// block 00477ec8
00477ec8  pop        ecx
00477ec9  test       eax, eax
00477ecb  je         0x477efd

// block 00477ecd
00477ecd  dec        dword ptr [ebp - 0x18c]
00477ed3  push       esi
00477ed4  push       esi
00477ed5  lea        esi, [ebp - 0x18c]
00477edb  call       0x477d65

// block 00477ee0
00477ee0  pop        ecx
00477ee1  push       eax
00477ee2  call       0x477d52

// block 00477ee7
00477ee7  pop        ecx
00477ee8  pop        ecx

// block 00477ee9
00477ee9  inc        edi
00477eea  movzx      eax, byte ptr [edi]
00477eed  push       eax
00477eee  call       0x48bb76

// block 00477ef3
00477ef3  pop        ecx
00477ef4  test       eax, eax
00477ef6  jne        0x477ee9

// block 00477ef8
00477ef8  jmp        0x478ca6

// block 00477efd
00477efd  mov        al, byte ptr [edi]
00477eff  cmp        al, 0x25
00477f01  jne        0x478c38

// block 00477f07
00477f07  cmp        byte ptr [edi + 1], al
00477f0a  je         0x478c2a

// block 00477f10
00477f10  xor        esi, esi
00477f12  mov        dword ptr [ebp - 0x1c0], esi
00477f18  mov        byte ptr [ebp - 0x1d1], 0
00477f1f  mov        dword ptr [ebp - 0x1a8], esi
00477f25  mov        dword ptr [ebp - 0x1b4], esi
00477f2b  mov        dword ptr [ebp - 0x194], esi
00477f31  mov        byte ptr [ebp - 0x19f], 0
00477f38  mov        byte ptr [ebp - 0x1a0], 0
00477f3f  mov        byte ptr [ebp - 0x196], 0
00477f46  mov        byte ptr [ebp - 0x1ad], 0
00477f4d  mov        byte ptr [ebp - 0x19e], 0
00477f54  mov        byte ptr [ebp - 0x18d], 0
00477f5b  mov        byte ptr [ebp - 0x195], 1
00477f62  mov        dword ptr [ebp - 0x1d8], esi

// block 00477f68
00477f68  inc        edi
00477f69  movzx      ebx, byte ptr [edi]
00477f6c  movzx      eax, bl
00477f6f  push       eax
00477f70  call       0x48ba71

// block 00477f75
00477f75  pop        ecx
00477f76  test       eax, eax
00477f78  je         0x477f98

// block 00477f7a
00477f7a  mov        eax, dword ptr [ebp - 0x194]
00477f80  inc        dword ptr [ebp - 0x1b4]
00477f86  imul       eax, eax, 0xa
00477f89  lea        eax, [eax + ebx - 0x30]
00477f8d  mov        dword ptr [ebp - 0x194], eax
00477f93  jmp        0x47805d

// block 00477f98
00477f98  cmp        ebx, 0x4e
00477f9b  jg         0x478024

// block 00477fa1
00477fa1  je         0x47805d

// block 00477fa7
00477fa7  cmp        ebx, 0x2a
00477faa  je         0x47801c

// block 00477fac
00477fac  cmp        ebx, 0x46
00477faf  je         0x47805d

// block 00477fb5
00477fb5  cmp        ebx, 0x49
00477fb8  je         0x477fca

// block 00477fba
00477fba  cmp        ebx, 0x4c
00477fbd  jne        0x478033

// block 00477fbf
00477fbf  inc        byte ptr [ebp - 0x195]
00477fc5  jmp        0x47805d

// block 00477fca
00477fca  mov        cl, byte ptr [edi + 1]
00477fcd  cmp        cl, 0x36
00477fd0  jne        0x477ff0

// block 00477fd2
00477fd2  lea        eax, [edi + 2]
00477fd5  cmp        byte ptr [eax], 0x34
00477fd8  jne        0x477ff0

// block 00477fda
00477fda  inc        dword ptr [ebp - 0x1d8]
00477fe0  mov        edi, eax
00477fe2  mov        dword ptr [ebp - 0x1cc], esi
00477fe8  mov        dword ptr [ebp - 0x1c8], esi
00477fee  jmp        0x47805d

// block 00477ff0
00477ff0  cmp        cl, 0x33
00477ff3  jne        0x478001

// block 00477ff5
00477ff5  lea        eax, [edi + 2]
00477ff8  cmp        byte ptr [eax], 0x32
00477ffb  jne        0x478001

// block 00477ffd
00477ffd  mov        edi, eax
00477fff  jmp        0x47805d

// block 00478001
00478001  cmp        cl, 0x64
00478004  je         0x47805d

// block 00478006
00478006  cmp        cl, 0x69
00478009  je         0x47805d

// block 0047800b
0047800b  cmp        cl, 0x6f
0047800e  je         0x47805d

// block 00478010
00478010  cmp        cl, 0x78
00478013  je         0x47805d

// block 00478015
00478015  cmp        cl, 0x58
00478018  jne        0x478033

// block 0047801a
0047801a  jmp        0x47805d

// block 0047801c
0047801c  inc        byte ptr [ebp - 0x196]
00478022  jmp        0x47805d

// block 00478024
00478024  cmp        ebx, 0x68
00478027  je         0x478051

// block 00478029
00478029  cmp        ebx, 0x6c
0047802c  je         0x47803b

// block 0047802e
0047802e  cmp        ebx, 0x77
00478031  je         0x478049

// block 00478033
00478033  inc        byte ptr [ebp - 0x1ad]
00478039  jmp        0x47805d

// block 0047803b
0047803b  lea        eax, [edi + 1]
0047803e  cmp        byte ptr [eax], 0x6c
00478041  je         0x477fda

// block 00478043
00478043  inc        byte ptr [ebp - 0x195]

// block 00478049
00478049  inc        byte ptr [ebp - 0x18d]
0047804f  jmp        0x47805d

// block 00478051
00478051  dec        byte ptr [ebp - 0x195]
00478057  dec        byte ptr [ebp - 0x18d]

// block 0047805d
0047805d  cmp        byte ptr [ebp - 0x1ad], 0
00478064  je         0x477f68

// block 0047806a
0047806a  cmp        byte ptr [ebp - 0x196], 0
00478071  mov        dword ptr [ebp - 0x1b8], edi
00478077  jne        0x478092

// block 00478079
00478079  mov        eax, dword ptr [ebp - 0x1e4]
0047807f  mov        esi, dword ptr [eax]
00478081  mov        dword ptr [ebp - 0x1fc], eax
00478087  add        eax, 4
0047808a  mov        dword ptr [ebp - 0x1e4], eax
00478090  jmp        0x478094

// block 00478092
00478092  xor        esi, esi

// block 00478094
00478094  xor        bl, bl
00478096  mov        dword ptr [ebp - 0x1bc], esi
0047809c  cmp        byte ptr [ebp - 0x18d], bl
004780a2  jne        0x4780bc

// block 004780a4
004780a4  mov        al, byte ptr [edi]
004780a6  cmp        al, 0x53
004780a8  je         0x4780b5

// block 004780aa
004780aa  mov        byte ptr [ebp - 0x18d], 0xff
004780b1  cmp        al, 0x43
004780b3  jne        0x4780bc

// block 004780b5
004780b5  mov        byte ptr [ebp - 0x18d], 1

// block 004780bc
004780bc  movzx      eax, byte ptr [edi]
004780bf  or         eax, 0x20
004780c2  mov        dword ptr [ebp - 0x1a4], eax
004780c8  cmp        eax, 0x6e
004780cb  je         0x478117

// block 004780cd
004780cd  cmp        eax, 0x63
004780d0  je         0x4780eb

// block 004780d2
004780d2  cmp        eax, 0x7b
004780d5  je         0x4780eb

// block 004780d7
004780d7  push       dword ptr [ebp - 0x19c]
004780dd  lea        esi, [ebp - 0x18c]
004780e3  call       0x477d65

// block 004780e8
004780e8  pop        ecx
004780e9  jmp        0x4780fc

// block 004780eb
004780eb  mov        edx, dword ptr [ebp - 0x19c]
004780f1  inc        dword ptr [ebp - 0x18c]
004780f7  call       0x477d3c

// block 004780fc
004780fc  mov        dword ptr [ebp - 0x188], eax
00478102  cmp        eax, -1
00478105  je         0x478cdc

// block 0047810b
0047810b  mov        esi, dword ptr [ebp - 0x1bc]
00478111  mov        edi, dword ptr [ebp - 0x1b8]

// block 00478117
00478117  mov        ecx, dword ptr [ebp - 0x1b4]
0047811d  test       ecx, ecx
0047811f  je         0x47812e

// block 00478121
00478121  cmp        dword ptr [ebp - 0x194], 0
00478128  je         0x478cb2

// block 0047812e
0047812e  mov        eax, dword ptr [ebp - 0x1a4]
00478134  cmp        eax, 0x6f
00478137  jg         0x4786af

// block 0047813d
0047813d  je         0x478944

// block 00478143
00478143  cmp        eax, 0x63
00478146  je         0x478556

// block 0047814c
0047814c  push       0x64
0047814e  pop        edx
0047814f  cmp        eax, edx
00478151  je         0x478944

// block 00478157
00478157  jle        0x4786d7

// block 0047815d
0047815d  cmp        eax, 0x67
00478160  jle        0x4781a7

// block 00478162
00478162  cmp        eax, 0x69
00478165  je         0x478188

// block 00478167
00478167  cmp        eax, 0x6e
0047816a  jne        0x4786d7

// block 00478170
00478170  cmp        byte ptr [ebp - 0x196], 0
00478177  mov        edi, dword ptr [ebp - 0x18c]
0047817d  je         0x478be9

// block 00478183
00478183  jmp        0x478c15

// block 00478188
00478188  mov        dword ptr [ebp - 0x1a4], edx

// block 0047818e
0047818e  cmp        dword ptr [ebp - 0x188], 0x2d
00478195  jne        0x4787cf

// block 0047819b
0047819b  mov        byte ptr [ebp - 0x1a0], 1
004781a2  jmp        0x4787d8

// block 004781a7
004781a7  xor        ebx, ebx
004781a9  cmp        dword ptr [ebp - 0x188], 0x2d
004781b0  jne        0x4781be

// block 004781b2
004781b2  mov        eax, dword ptr [ebp - 0x1ac]
004781b8  mov        byte ptr [eax], 0x2d
004781bb  inc        ebx
004781bc  jmp        0x4781c7

// block 004781be
004781be  cmp        dword ptr [ebp - 0x188], 0x2b
004781c5  jne        0x4781e4

// block 004781c7
004781c7  dec        dword ptr [ebp - 0x194]
004781cd  mov        edx, dword ptr [ebp - 0x19c]
004781d3  inc        dword ptr [ebp - 0x18c]
004781d9  call       0x477d3c

// block 004781de
004781de  mov        dword ptr [ebp - 0x188], eax

// block 004781e4
004781e4  cmp        dword ptr [ebp - 0x1b4], 0
004781eb  jne        0x4781f4

// block 004781ed
004781ed  or         dword ptr [ebp - 0x194], 0xffffffff

// block 004781f4
004781f4  movzx      eax, byte ptr [ebp - 0x188]
004781fb  jmp        0x478268

// block 004781fd
004781fd  mov        eax, dword ptr [ebp - 0x194]
00478203  dec        dword ptr [ebp - 0x194]
00478209  test       eax, eax
0047820b  je         0x478273

// block 0047820d
0047820d  mov        al, byte ptr [ebp - 0x188]
00478213  mov        ecx, dword ptr [ebp - 0x1ac]
00478219  inc        dword ptr [ebp - 0x1a8]
0047821f  mov        byte ptr [ebx + ecx], al
00478222  lea        eax, [ebp - 0x1d0]
00478228  push       eax
00478229  lea        eax, [ebp - 0x184]
0047822f  push       eax
00478230  inc        ebx
00478231  push       ebx
00478232  lea        edi, [ebp - 0x1ac]
00478238  lea        esi, [ebp - 0x1dc]
0047823e  call       0x477cb3

// block 00478243
00478243  add        esp, 0xc
00478246  test       eax, eax
00478248  je         0x478cdc

// block 0047824e
0047824e  mov        edx, dword ptr [ebp - 0x19c]
00478254  inc        dword ptr [ebp - 0x18c]
0047825a  call       0x477d3c

// block 0047825f
0047825f  mov        dword ptr [ebp - 0x188], eax
00478265  movzx      eax, al

// block 00478268
00478268  push       eax
00478269  call       0x48ba71

// block 0047826e
0047826e  pop        ecx
0047826f  test       eax, eax
00478271  jne        0x4781fd

// block 00478273
00478273  mov        eax, dword ptr [ebp - 0x1f8]
00478279  mov        eax, dword ptr [eax + 0xbc]
0047827f  mov        eax, dword ptr [eax]
00478281  mov        al, byte ptr [eax]
00478283  mov        byte ptr [ebp - 0x19f], al
00478289  cmp        al, byte ptr [ebp - 0x188]
0047828f  jne        0x47837a

// block 00478295
00478295  mov        eax, dword ptr [ebp - 0x194]
0047829b  dec        dword ptr [ebp - 0x194]
004782a1  test       eax, eax
004782a3  je         0x47837a

// block 004782a9
004782a9  mov        edx, dword ptr [ebp - 0x19c]
004782af  inc        dword ptr [ebp - 0x18c]
004782b5  call       0x477d3c

// block 004782ba
004782ba  mov        ecx, dword ptr [ebp - 0x1ac]
004782c0  mov        dword ptr [ebp - 0x188], eax
004782c6  mov        al, byte ptr [ebp - 0x19f]
004782cc  mov        byte ptr [ebx + ecx], al
004782cf  lea        eax, [ebp - 0x1d0]
004782d5  push       eax
004782d6  lea        eax, [ebp - 0x184]
004782dc  push       eax
004782dd  inc        ebx
004782de  push       ebx
004782df  lea        edi, [ebp - 0x1ac]
004782e5  lea        esi, [ebp - 0x1dc]
004782eb  call       0x477cb3

// block 004782f0
004782f0  add        esp, 0xc
004782f3  test       eax, eax
004782f5  je         0x478cdc

// block 004782fb
004782fb  movzx      eax, byte ptr [ebp - 0x188]
00478302  jmp        0x47836f

// block 00478304
00478304  mov        eax, dword ptr [ebp - 0x194]
0047830a  dec        dword ptr [ebp - 0x194]
00478310  test       eax, eax
00478312  je         0x47837a

// block 00478314
00478314  mov        eax, dword ptr [ebp - 0x1ac]
0047831a  mov        cl, byte ptr [ebp - 0x188]
00478320  inc        dword ptr [ebp - 0x1a8]
00478326  mov        byte ptr [ebx + eax], cl
00478329  lea        eax, [ebp - 0x1d0]
0047832f  push       eax
00478330  lea        eax, [ebp - 0x184]
00478336  push       eax
00478337  inc        ebx
00478338  push       ebx
00478339  lea        edi, [ebp - 0x1ac]
0047833f  lea        esi, [ebp - 0x1dc]
00478345  call       0x477cb3

// block 0047834a
0047834a  add        esp, 0xc
0047834d  test       eax, eax
0047834f  je         0x478cdc

// block 00478355
00478355  mov        edx, dword ptr [ebp - 0x19c]
0047835b  inc        dword ptr [ebp - 0x18c]
00478361  call       0x477d3c

// block 00478366
00478366  mov        dword ptr [ebp - 0x188], eax
0047836c  movzx      eax, al

// block 0047836f
0047836f  push       eax
00478370  call       0x48ba71

// block 00478375
00478375  pop        ecx
00478376  test       eax, eax
00478378  jne        0x478304

// block 0047837a
0047837a  cmp        dword ptr [ebp - 0x1a8], 0
00478381  je         0x4784e6

// block 00478387
00478387  cmp        dword ptr [ebp - 0x188], 0x65
0047838e  je         0x47839d

// block 00478390
00478390  cmp        dword ptr [ebp - 0x188], 0x45
00478397  jne        0x4784e6

// block 0047839d
0047839d  mov        eax, dword ptr [ebp - 0x194]
004783a3  dec        dword ptr [ebp - 0x194]
004783a9  test       eax, eax
004783ab  je         0x4784e6

// block 004783b1
004783b1  mov        eax, dword ptr [ebp - 0x1ac]
004783b7  mov        byte ptr [ebx + eax], 0x65
004783bb  lea        eax, [ebp - 0x1d0]
004783c1  push       eax
004783c2  lea        eax, [ebp - 0x184]
004783c8  push       eax
004783c9  inc        ebx
004783ca  push       ebx
004783cb  lea        edi, [ebp - 0x1ac]
004783d1  lea        esi, [ebp - 0x1dc]
004783d7  call       0x477cb3

// block 004783dc
004783dc  add        esp, 0xc
004783df  test       eax, eax
004783e1  je         0x478cdc

// block 004783e7
004783e7  mov        edx, dword ptr [ebp - 0x19c]
004783ed  inc        dword ptr [ebp - 0x18c]
004783f3  call       0x477d3c

// block 004783f8
004783f8  mov        dword ptr [ebp - 0x188], eax
004783fe  cmp        eax, 0x2d
00478401  jne        0x47842f

// block 00478403
00478403  mov        eax, dword ptr [ebp - 0x1ac]
00478409  mov        byte ptr [ebx + eax], 0x2d
0047840d  lea        eax, [ebp - 0x1d0]
00478413  push       eax
00478414  lea        eax, [ebp - 0x184]
0047841a  push       eax
0047841b  inc        ebx
0047841c  push       ebx
0047841d  call       0x477cb3

// block 00478422
00478422  add        esp, 0xc
00478425  test       eax, eax
00478427  je         0x478cdc

// block 0047842d
0047842d  jmp        0x478438

// block 0047842f
0047842f  cmp        dword ptr [ebp - 0x188], 0x2b
00478436  jne        0x478467

// block 00478438
00478438  mov        eax, dword ptr [ebp - 0x194]
0047843e  dec        dword ptr [ebp - 0x194]
00478444  test       eax, eax
00478446  jne        0x478450

// block 00478448
00478448  and        dword ptr [ebp - 0x194], eax
0047844e  jmp        0x478467

// block 00478450
00478450  mov        edx, dword ptr [ebp - 0x19c]
00478456  inc        dword ptr [ebp - 0x18c]
0047845c  call       0x477d3c

// block 00478461
00478461  mov        dword ptr [ebp - 0x188], eax

// block 00478467
00478467  movzx      eax, byte ptr [ebp - 0x188]
0047846e  jmp        0x4784db

// block 00478470
00478470  mov        eax, dword ptr [ebp - 0x194]
00478476  dec        dword ptr [ebp - 0x194]
0047847c  test       eax, eax
0047847e  je         0x4784e6

// block 00478480
00478480  mov        eax, dword ptr [ebp - 0x1ac]
00478486  mov        cl, byte ptr [ebp - 0x188]
0047848c  inc        dword ptr [ebp - 0x1a8]
00478492  mov        byte ptr [ebx + eax], cl
00478495  lea        eax, [ebp - 0x1d0]
0047849b  push       eax
0047849c  lea        eax, [ebp - 0x184]
004784a2  push       eax
004784a3  inc        ebx
004784a4  push       ebx
004784a5  lea        edi, [ebp - 0x1ac]
004784ab  lea        esi, [ebp - 0x1dc]
004784b1  call       0x477cb3

// block 004784b6
004784b6  add        esp, 0xc
004784b9  test       eax, eax
004784bb  je         0x478cdc

// block 004784c1
004784c1  mov        edx, dword ptr [ebp - 0x19c]
004784c7  inc        dword ptr [ebp - 0x18c]
004784cd  call       0x477d3c

// block 004784d2
004784d2  mov        dword ptr [ebp - 0x188], eax
004784d8  movzx      eax, al

// block 004784db
004784db  push       eax
004784dc  call       0x48ba71

// block 004784e1
004784e1  pop        ecx
004784e2  test       eax, eax
004784e4  jne        0x478470

// block 004784e6
004784e6  push       dword ptr [ebp - 0x19c]
004784ec  dec        dword ptr [ebp - 0x18c]
004784f2  push       dword ptr [ebp - 0x188]
004784f8  call       0x477d52

// block 004784fd
004784fd  cmp        dword ptr [ebp - 0x1a8], 0
00478504  pop        ecx
00478505  pop        ecx
00478506  je         0x478cdc

// block 0047850c
0047850c  cmp        byte ptr [ebp - 0x196], 0
00478513  jne        0x478c15

// block 00478519
00478519  mov        eax, dword ptr [ebp - 0x1ac]
0047851f  inc        dword ptr [ebp - 0x1c4]
00478525  lea        ecx, [ebp - 0x1f8]
0047852b  push       ecx
0047852c  push       eax
0047852d  push       dword ptr [ebp - 0x1bc]
00478533  mov        byte ptr [ebx + eax], 0
00478537  movsx      eax, byte ptr [ebp - 0x195]
0047853e  dec        eax
0047853f  push       eax
00478540  push       dword ptr [0x4ade8c]
00478546  call       0x4751de

// block 0047854b
0047854b  pop        ecx
0047854c  call       eax

// block 0047854e
0047854e  add        esp, 0x10
00478551  jmp        0x478c15

// block 00478556
00478556  test       ecx, ecx
00478558  jne        0x47856a

// block 0047855a
0047855a  inc        dword ptr [ebp - 0x194]
00478560  mov        dword ptr [ebp - 0x1b4], 1

// block 0047856a
0047856a  cmp        byte ptr [ebp - 0x18d], 0
00478571  jle        0x47857a

// block 00478573
00478573  mov        byte ptr [ebp - 0x19e], 1

// block 0047857a
0047857a  mov        edi, dword ptr [ebp - 0x19c]
00478580  dec        dword ptr [ebp - 0x18c]
00478586  push       edi
00478587  push       dword ptr [ebp - 0x188]
0047858d  mov        dword ptr [ebp - 0x1c0], esi
00478593  call       0x477d52

// block 00478598
00478598  pop        ecx
00478599  pop        ecx

// block 0047859a
0047859a  cmp        dword ptr [ebp - 0x1b4], 0
004785a1  je         0x4785b7

// block 004785a3
004785a3  mov        eax, dword ptr [ebp - 0x194]
004785a9  dec        dword ptr [ebp - 0x194]
004785af  test       eax, eax
004785b1  je         0x4788ea

// block 004785b7
004785b7  inc        dword ptr [ebp - 0x18c]
004785bd  mov        edx, edi
004785bf  call       0x477d3c

// block 004785c4
004785c4  mov        dword ptr [ebp - 0x188], eax
004785ca  cmp        eax, -1
004785cd  je         0x4788db

// block 004785d3
004785d3  cmp        dword ptr [ebp - 0x1a4], 0x63
004785da  je         0x47862a

// block 004785dc
004785dc  cmp        dword ptr [ebp - 0x1a4], 0x73
004785e3  jne        0x4785f8

// block 004785e5
004785e5  cmp        eax, 9
004785e8  jl         0x4785f3

// block 004785ea
004785ea  cmp        eax, 0xd
004785ed  jle        0x4788db

// block 004785f3
004785f3  cmp        eax, 0x20
004785f6  jne        0x47862a

// block 004785f8
004785f8  cmp        dword ptr [ebp - 0x1a4], 0x7b
004785ff  jne        0x4788db

// block 00478605
00478605  movsx      ebx, byte ptr [ebp - 0x19f]
0047860c  xor        edx, edx
0047860e  mov        ecx, eax
00478610  and        ecx, 7
00478613  inc        edx
00478614  shl        edx, cl
00478616  mov        ecx, eax
00478618  sar        ecx, 3
0047861b  movsx      ecx, byte ptr [ebp + ecx - 0x24]
00478620  xor        ecx, ebx
00478622  test       ecx, edx
00478624  je         0x4788db

// block 0047862a
0047862a  cmp        byte ptr [ebp - 0x196], 0
00478631  jne        0x4788d0

// block 00478637
00478637  cmp        byte ptr [ebp - 0x19e], 0
0047863e  je         0x4788c2

// block 00478644
00478644  mov        byte ptr [ebp - 0x1e0], al
0047864a  movzx      eax, al
0047864d  push       eax
0047864e  call       0x47c5db

// block 00478653
00478653  pop        ecx
00478654  test       eax, eax
00478656  je         0x47866b

// block 00478658
00478658  inc        dword ptr [ebp - 0x18c]
0047865e  mov        edx, edi
00478660  call       0x477d3c

// block 00478665
00478665  mov        byte ptr [ebp - 0x1df], al

// block 0047866b
0047866b  lea        eax, [ebp - 0x1f8]
00478671  push       eax
00478672  mov        eax, dword ptr [ebp - 0x1f8]
00478678  mov        dword ptr [ebp - 0x1e8], 0x3f
00478682  push       dword ptr [eax + 0xac]
00478688  lea        eax, [ebp - 0x1e0]
0047868e  push       eax
0047868f  lea        eax, [ebp - 0x1e8]
00478695  push       eax
00478696  call       0x48c167

// block 0047869b
0047869b  mov        ax, word ptr [ebp - 0x1e8]
004786a2  add        esp, 0x10
004786a5  mov        word ptr [esi], ax
004786a8  inc        esi
004786a9  inc        esi
004786aa  jmp        0x4788c5

// block 004786af
004786af  sub        eax, 0x70
004786b2  je         0x47893d

// block 004786b8
004786b8  sub        eax, 3
004786bb  je         0x47856a

// block 004786c1
004786c1  dec        eax
004786c2  dec        eax
004786c3  je         0x478944

// block 004786c9
004786c9  sub        eax, 3
004786cc  je         0x47818e

// block 004786d2
004786d2  sub        eax, 3
004786d5  je         0x47870a

// block 004786d7
004786d7  movzx      eax, byte ptr [edi]
004786da  cmp        eax, dword ptr [ebp - 0x188]
004786e0  jne        0x478cb2

// block 004786e6
004786e6  dec        byte ptr [ebp - 0x19d]
004786ec  cmp        byte ptr [ebp - 0x196], 0
004786f3  jne        0x478c15

// block 004786f9
004786f9  mov        eax, dword ptr [ebp - 0x1fc]
004786ff  mov        dword ptr [ebp - 0x1e4], eax
00478705  jmp        0x478c15

// block 0047870a
0047870a  cmp        byte ptr [ebp - 0x18d], 0
00478711  jle        0x47871a

// block 00478713
00478713  mov        byte ptr [ebp - 0x19e], 1

// block 0047871a
0047871a  inc        edi
0047871b  cmp        byte ptr [edi], 0x5e
0047871e  mov        esi, edi
00478720  jne        0x47872c

// block 00478722
00478722  lea        esi, [edi + 1]
00478725  mov        byte ptr [ebp - 0x19f], 0xff

// block 0047872c
0047872c  push       0x20
0047872e  lea        eax, [ebp - 0x24]
00478731  push       0
00478733  push       eax
00478734  call       0x477420

// block 00478739
00478739  add        esp, 0xc
0047873c  cmp        byte ptr [esi], 0x5d
0047873f  jne        0x47874a

// block 00478741
00478741  mov        dl, 0x5d
00478743  inc        esi
00478744  mov        byte ptr [ebp - 0x19], 0x20
00478748  jmp        0x4787b0

// block 0047874a
0047874a  mov        dl, byte ptr [ebp - 0x1d1]
00478750  jmp        0x4787b0

// block 00478752
00478752  inc        esi
00478753  cmp        al, 0x2d
00478755  jne        0x478799

// block 00478757
00478757  test       dl, dl
00478759  je         0x478799

// block 0047875b
0047875b  mov        cl, byte ptr [esi]
0047875d  cmp        cl, 0x5d
00478760  je         0x478799

// block 00478762
00478762  inc        esi
00478763  cmp        dl, cl
00478765  jae        0x47876b

// block 00478767
00478767  mov        al, cl
00478769  jmp        0x47876f

// block 0047876b
0047876b  mov        al, dl
0047876d  mov        dl, cl

// block 0047876f
0047876f  cmp        dl, al
00478771  ja         0x478795

// block 00478773
00478773  sub        al, dl
00478775  inc        al
00478777  movzx      edi, dl
0047877a  movzx      edx, al

// block 0047877d
0047877d  mov        ecx, edi
0047877f  and        ecx, 7
00478782  mov        eax, edi
00478784  mov        bl, 1
00478786  shl        bl, cl
00478788  shr        eax, 3
0047878b  lea        eax, [ebp + eax - 0x24]
0047878f  or         byte ptr [eax], bl
00478791  inc        edi
00478792  dec        edx
00478793  jne        0x47877d

// block 00478795
00478795  xor        dl, dl
00478797  jmp        0x4787b0

// block 00478799
00478799  movzx      ecx, al
0047879c  mov        dl, al
0047879e  mov        eax, ecx
004787a0  and        ecx, 7
004787a3  mov        bl, 1
004787a5  shr        eax, 3
004787a8  shl        bl, cl
004787aa  lea        eax, [ebp + eax - 0x24]
004787ae  or         byte ptr [eax], bl

// block 004787b0
004787b0  mov        al, byte ptr [esi]
004787b2  cmp        al, 0x5d
004787b4  jne        0x478752

// block 004787b6
004787b6  test       al, al
004787b8  je         0x478cdc

// block 004787be
004787be  mov        dword ptr [ebp - 0x1b8], esi
004787c4  mov        esi, dword ptr [ebp - 0x1bc]
004787ca  jmp        0x47857a

// block 004787cf
004787cf  cmp        dword ptr [ebp - 0x188], 0x2b
004787d6  jne        0x4787ff

// block 004787d8
004787d8  dec        dword ptr [ebp - 0x194]
004787de  jne        0x4787e8

// block 004787e0
004787e0  test       ecx, ecx
004787e2  je         0x4787e8

// block 004787e4
004787e4  mov        bl, 1
004787e6  jmp        0x4787ff

// block 004787e8
004787e8  mov        edx, dword ptr [ebp - 0x19c]
004787ee  inc        dword ptr [ebp - 0x18c]
004787f4  call       0x477d3c

// block 004787f9
004787f9  mov        dword ptr [ebp - 0x188], eax

// block 004787ff
004787ff  push       0x30
00478801  pop        esi
00478802  cmp        dword ptr [ebp - 0x188], esi
00478808  jne        0x478986

// block 0047880e
0047880e  mov        edx, dword ptr [ebp - 0x19c]
00478814  inc        dword ptr [ebp - 0x18c]
0047881a  call       0x477d3c

// block 0047881f
0047881f  mov        dword ptr [ebp - 0x188], eax
00478825  cmp        al, 0x78
00478827  je         0x478881

// block 00478829
00478829  cmp        al, 0x58
0047882b  je         0x478881

// block 0047882d
0047882d  cmp        dword ptr [ebp - 0x1a4], 0x78
00478834  mov        dword ptr [ebp - 0x1a8], 1
0047883e  je         0x478862

// block 00478840
00478840  cmp        dword ptr [ebp - 0x1b4], 0
00478847  je         0x478853

// block 00478849
00478849  dec        dword ptr [ebp - 0x194]
0047884f  jne        0x478853

// block 00478851
00478851  inc        bl

// block 00478853
00478853  mov        dword ptr [ebp - 0x1a4], 0x6f
0047885d  jmp        0x478986

// block 00478862
00478862  push       dword ptr [ebp - 0x19c]
00478868  dec        dword ptr [ebp - 0x18c]
0047886e  push       eax
0047886f  call       0x477d52

// block 00478874
00478874  pop        ecx
00478875  pop        ecx
00478876  mov        dword ptr [ebp - 0x188], esi
0047887c  jmp        0x478986

// block 00478881
00478881  mov        edx, dword ptr [ebp - 0x19c]
00478887  inc        dword ptr [ebp - 0x18c]
0047888d  call       0x477d3c

// block 00478892
00478892  cmp        dword ptr [ebp - 0x1b4], 0
00478899  mov        dword ptr [ebp - 0x188], eax
0047889f  je         0x4788b3

// block 004788a1
004788a1  sub        dword ptr [ebp - 0x194], 2
004788a8  cmp        dword ptr [ebp - 0x194], 1
004788af  jge        0x4788b3

// block 004788b1
004788b1  inc        bl

// block 004788b3
004788b3  mov        dword ptr [ebp - 0x1a4], 0x78
004788bd  jmp        0x478986

// block 004788c2
004788c2  mov        byte ptr [esi], al
004788c4  inc        esi

// block 004788c5
004788c5  mov        dword ptr [ebp - 0x1bc], esi
004788cb  jmp        0x47859a

// block 004788d0
004788d0  inc        dword ptr [ebp - 0x1c0]
004788d6  jmp        0x47859a

// block 004788db
004788db  dec        dword ptr [ebp - 0x18c]
004788e1  push       edi
004788e2  push       eax
004788e3  call       0x477d52

// block 004788e8
004788e8  pop        ecx
004788e9  pop        ecx

// block 004788ea
004788ea  cmp        dword ptr [ebp - 0x1c0], esi
004788f0  je         0x478cdc

// block 004788f6
004788f6  cmp        byte ptr [ebp - 0x196], 0
004788fd  jne        0x478c15

// block 00478903
00478903  inc        dword ptr [ebp - 0x1c4]
00478909  cmp        dword ptr [ebp - 0x1a4], 0x63
00478910  je         0x478c15

// block 00478916
00478916  cmp        byte ptr [ebp - 0x19e], 0
0047891d  je         0x47892f

// block 0047891f
0047891f  mov        ecx, dword ptr [ebp - 0x1bc]
00478925  xor        eax, eax
00478927  mov        word ptr [ecx], ax
0047892a  jmp        0x478c15

// block 0047892f
0047892f  mov        eax, dword ptr [ebp - 0x1bc]
00478935  mov        byte ptr [eax], 0
00478938  jmp        0x478c15

// block 0047893d
0047893d  mov        byte ptr [ebp - 0x195], 1

// block 00478944
00478944  cmp        dword ptr [ebp - 0x188], 0x2d
0047894b  jne        0x478956

// block 0047894d
0047894d  mov        byte ptr [ebp - 0x1a0], 1
00478954  jmp        0x47895f

// block 00478956
00478956  cmp        dword ptr [ebp - 0x188], 0x2b
0047895d  jne        0x478986

// block 0047895f
0047895f  dec        dword ptr [ebp - 0x194]
00478965  jne        0x47896f

// block 00478967
00478967  test       ecx, ecx
00478969  je         0x47896f

// block 0047896b
0047896b  mov        bl, 1
0047896d  jmp        0x478986

// block 0047896f
0047896f  mov        edx, dword ptr [ebp - 0x19c]
00478975  inc        dword ptr [ebp - 0x18c]
0047897b  call       0x477d3c

// block 00478980
00478980  mov        dword ptr [ebp - 0x188], eax

// block 00478986
00478986  cmp        dword ptr [ebp - 0x1d8], 0
0047898d  je         0x478add

// block 00478993
00478993  test       bl, bl
00478995  jne        0x478aa6

// block 0047899b
0047899b  cmp        dword ptr [ebp - 0x1a4], 0x78
004789a2  je         0x478a09

// block 004789a4
004789a4  cmp        dword ptr [ebp - 0x1a4], 0x70
004789ab  je         0x478a09

// block 004789ad
004789ad  movzx      eax, byte ptr [ebp - 0x188]
004789b4  push       eax
004789b5  call       0x48ba71

// block 004789ba
004789ba  pop        ecx
004789bb  test       eax, eax
004789bd  je         0x478a8d

// block 004789c3
004789c3  cmp        dword ptr [ebp - 0x1a4], 0x6f
004789ca  jne        0x4789ee

// block 004789cc
004789cc  cmp        dword ptr [ebp - 0x188], 0x38
004789d3  jge        0x478a8d

// block 004789d9
004789d9  mov        esi, dword ptr [ebp - 0x1cc]
004789df  mov        edi, dword ptr [ebp - 0x1c8]
004789e5  shld       edi, esi, 3
004789e9  shl        esi, 3
004789ec  jmp        0x478a40

// block 004789ee
004789ee  push       0
004789f0  push       0xa
004789f2  push       dword ptr [ebp - 0x1c8]
004789f8  push       dword ptr [ebp - 0x1cc]
004789fe  call       0x483220

// block 00478a03
00478a03  mov        esi, eax
00478a05  mov        edi, edx
00478a07  jmp        0x478a40

// block 00478a09
00478a09  movzx      eax, byte ptr [ebp - 0x188]
00478a10  push       eax
00478a11  call       0x48baf5

// block 00478a16
00478a16  pop        ecx
00478a17  test       eax, eax
00478a19  je         0x478a8d

// block 00478a1b
00478a1b  mov        esi, dword ptr [ebp - 0x1cc]
00478a21  mov        edi, dword ptr [ebp - 0x1c8]
00478a27  push       dword ptr [ebp - 0x188]
00478a2d  shld       edi, esi, 4
00478a31  shl        esi, 4
00478a34  call       0x477d1c

// block 00478a39
00478a39  pop        ecx
00478a3a  mov        dword ptr [ebp - 0x188], eax

// block 00478a40
00478a40  mov        eax, dword ptr [ebp - 0x188]
00478a46  inc        dword ptr [ebp - 0x1a8]
00478a4c  add        eax, -0x30
00478a4f  cdq        
00478a50  add        esi, eax
00478a52  adc        edi, edx
00478a54  cmp        dword ptr [ebp - 0x1b4], 0
00478a5b  mov        dword ptr [ebp - 0x1cc], esi
00478a61  mov        dword ptr [ebp - 0x1c8], edi
00478a67  je         0x478a71

// block 00478a69
00478a69  dec        dword ptr [ebp - 0x194]
00478a6f  je         0x478aa6

// block 00478a71
00478a71  mov        edx, dword ptr [ebp - 0x19c]
00478a77  inc        dword ptr [ebp - 0x18c]
00478a7d  call       0x477d3c

// block 00478a82
00478a82  mov        dword ptr [ebp - 0x188], eax
00478a88  jmp        0x47899b

// block 00478a8d
00478a8d  push       dword ptr [ebp - 0x19c]
00478a93  dec        dword ptr [ebp - 0x18c]
00478a99  push       dword ptr [ebp - 0x188]
00478a9f  call       0x477d52

// block 00478aa4
00478aa4  pop        ecx
00478aa5  pop        ecx

// block 00478aa6
00478aa6  cmp        byte ptr [ebp - 0x1a0], 0
00478aad  mov        edi, dword ptr [ebp - 0x1c0]
00478ab3  je         0x478bb7

// block 00478ab9
00478ab9  mov        eax, dword ptr [ebp - 0x1cc]
00478abf  mov        ecx, dword ptr [ebp - 0x1c8]
00478ac5  neg        eax
00478ac7  adc        ecx, 0
00478aca  neg        ecx
00478acc  mov        dword ptr [ebp - 0x1cc], eax
00478ad2  mov        dword ptr [ebp - 0x1c8], ecx
00478ad8  jmp        0x478bb7

// block 00478add
00478add  mov        edi, dword ptr [ebp - 0x1c0]
00478ae3  test       bl, bl
00478ae5  jne        0x478bac

// block 00478aeb
00478aeb  cmp        dword ptr [ebp - 0x1a4], 0x78
00478af2  je         0x478b2f

// block 00478af4
00478af4  cmp        dword ptr [ebp - 0x1a4], 0x70
00478afb  je         0x478b2f

// block 00478afd
00478afd  movzx      eax, byte ptr [ebp - 0x188]
00478b04  push       eax
00478b05  call       0x48ba71

// block 00478b0a
00478b0a  pop        ecx
00478b0b  test       eax, eax
00478b0d  je         0x478b93

// block 00478b13
00478b13  cmp        dword ptr [ebp - 0x1a4], 0x6f
00478b1a  jne        0x478b2a

// block 00478b1c
00478b1c  cmp        dword ptr [ebp - 0x188], 0x38
00478b23  jge        0x478b93

// block 00478b25
00478b25  shl        edi, 3
00478b28  jmp        0x478b56

// block 00478b2a
00478b2a  imul       edi, edi, 0xa
00478b2d  jmp        0x478b56

// block 00478b2f
00478b2f  movzx      eax, byte ptr [ebp - 0x188]
00478b36  push       eax
00478b37  call       0x48baf5

// block 00478b3c
00478b3c  pop        ecx
00478b3d  test       eax, eax
00478b3f  je         0x478b93

// block 00478b41
00478b41  push       dword ptr [ebp - 0x188]
00478b47  shl        edi, 4
00478b4a  call       0x477d1c

// block 00478b4f
00478b4f  pop        ecx
00478b50  mov        dword ptr [ebp - 0x188], eax

// block 00478b56
00478b56  inc        dword ptr [ebp - 0x1a8]
00478b5c  cmp        dword ptr [ebp - 0x1b4], 0
00478b63  mov        eax, dword ptr [ebp - 0x188]
00478b69  lea        edi, [edi + eax - 0x30]
00478b6d  je         0x478b77

// block 00478b6f
00478b6f  dec        dword ptr [ebp - 0x194]
00478b75  je         0x478bac

// block 00478b77
00478b77  mov        edx, dword ptr [ebp - 0x19c]
00478b7d  inc        dword ptr [ebp - 0x18c]
00478b83  call       0x477d3c

// block 00478b88
00478b88  mov        dword ptr [ebp - 0x188], eax
00478b8e  jmp        0x478aeb

// block 00478b93
00478b93  push       dword ptr [ebp - 0x19c]
00478b99  dec        dword ptr [ebp - 0x18c]
00478b9f  push       dword ptr [ebp - 0x188]
00478ba5  call       0x477d52

// block 00478baa
00478baa  pop        ecx
00478bab  pop        ecx

// block 00478bac
00478bac  cmp        byte ptr [ebp - 0x1a0], 0
00478bb3  je         0x478bb7

// block 00478bb5
00478bb5  neg        edi

// block 00478bb7
00478bb7  cmp        dword ptr [ebp - 0x1a4], 0x46
00478bbe  jne        0x478bc7

// block 00478bc0
00478bc0  and        dword ptr [ebp - 0x1a8], 0

// block 00478bc7
00478bc7  cmp        dword ptr [ebp - 0x1a8], 0
00478bce  je         0x478cdc

// block 00478bd4
00478bd4  cmp        byte ptr [ebp - 0x196], 0
00478bdb  jne        0x478c15

// block 00478bdd
00478bdd  inc        dword ptr [ebp - 0x1c4]
00478be3  mov        esi, dword ptr [ebp - 0x1bc]

// block 00478be9
00478be9  cmp        dword ptr [ebp - 0x1d8], 0
00478bf0  je         0x478c05

// block 00478bf2
00478bf2  mov        eax, dword ptr [ebp - 0x1cc]
00478bf8  mov        dword ptr [esi], eax
00478bfa  mov        eax, dword ptr [ebp - 0x1c8]
00478c00  mov        dword ptr [esi + 4], eax
00478c03  jmp        0x478c15

// block 00478c05
00478c05  cmp        byte ptr [ebp - 0x195], 0
00478c0c  je         0x478c12

// block 00478c0e
00478c0e  mov        dword ptr [esi], edi
00478c10  jmp        0x478c15

// block 00478c12
00478c12  mov        word ptr [esi], di

// block 00478c15
00478c15  mov        edi, dword ptr [ebp - 0x1b8]
00478c1b  inc        byte ptr [ebp - 0x19d]
00478c21  inc        edi
00478c22  mov        dword ptr [ebp - 0x1b8], edi
00478c28  jmp        0x478c8a

// block 00478c2a
00478c2a  cmp        al, 0x25
00478c2c  jne        0x478c38

// block 00478c2e
00478c2e  lea        eax, [edi + 1]
00478c31  cmp        byte ptr [eax], 0x25
00478c34  jne        0x478c38

// block 00478c36
00478c36  mov        edi, eax

// block 00478c38
00478c38  inc        dword ptr [ebp - 0x18c]
00478c3e  mov        edx, esi
00478c40  call       0x477d3c

// block 00478c45
00478c45  mov        ebx, eax
00478c47  movzx      eax, byte ptr [edi]
00478c4a  inc        edi
00478c4b  mov        dword ptr [ebp - 0x188], ebx
00478c51  mov        dword ptr [ebp - 0x1b8], edi
00478c57  cmp        eax, ebx
00478c59  jne        0x478cc7

// block 00478c5b
00478c5b  movzx      eax, bl
00478c5e  push       eax
00478c5f  call       0x47c5db

// block 00478c64
00478c64  pop        ecx
00478c65  test       eax, eax
00478c67  je         0x478c8a

// block 00478c69
00478c69  inc        dword ptr [ebp - 0x18c]
00478c6f  mov        edx, esi
00478c71  call       0x477d3c

// block 00478c76
00478c76  movzx      ecx, byte ptr [edi]
00478c79  inc        edi
00478c7a  mov        dword ptr [ebp - 0x1b8], edi
00478c80  cmp        ecx, eax
00478c82  jne        0x478ccb

// block 00478c84
00478c84  dec        dword ptr [ebp - 0x18c]

// block 00478c8a
00478c8a  cmp        dword ptr [ebp - 0x188], -1
00478c91  jne        0x478ca6

// block 00478c93
00478c93  cmp        byte ptr [edi], 0x25
00478c96  jne        0x478cdc

// block 00478c98
00478c98  mov        eax, dword ptr [ebp - 0x1b8]
00478c9e  cmp        byte ptr [eax + 1], 0x6e
00478ca2  jne        0x478cdc

// block 00478ca4
00478ca4  mov        edi, eax

// block 00478ca6
00478ca6  mov        al, byte ptr [edi]
00478ca8  test       al, al
00478caa  jne        0x477eb9

// block 00478cb0
00478cb0  jmp        0x478cdc

// block 00478cb2
00478cb2  push       dword ptr [ebp - 0x19c]
00478cb8  push       dword ptr [ebp - 0x188]

// block 00478cbe
00478cbe  call       0x477d52

// block 00478cc3
00478cc3  pop        ecx
00478cc4  pop        ecx
00478cc5  jmp        0x478cdc

// block 00478cc7
00478cc7  push       esi
00478cc8  push       ebx
00478cc9  jmp        0x478cbe

// block 00478ccb
00478ccb  push       esi
00478ccc  push       eax
00478ccd  call       0x477d52

// block 00478cd2
00478cd2  push       esi
00478cd3  push       ebx
00478cd4  call       0x477d52

// block 00478cd9
00478cd9  add        esp, 0x10

// block 00478cdc
00478cdc  cmp        dword ptr [ebp - 0x1d0], 1
00478ce3  jne        0x478cf1

// block 00478ce5
00478ce5  push       dword ptr [ebp - 0x1ac]
00478ceb  call       0x46c941

// block 00478cf0
00478cf0  pop        ecx

// block 00478cf1
00478cf1  cmp        dword ptr [ebp - 0x188], -1
00478cf8  jne        0x478d24

// block 00478cfa
00478cfa  mov        eax, dword ptr [ebp - 0x1c4]
00478d00  test       eax, eax
00478d02  jne        0x478d0f

// block 00478d04
00478d04  cmp        byte ptr [ebp - 0x19d], al
00478d0a  jne        0x478d0f

// block 00478d0c
00478d0c  or         eax, 0xffffffff

// block 00478d0f
00478d0f  cmp        byte ptr [ebp - 0x1ec], 0
00478d16  je         0x478d3d

// block 00478d18
00478d18  mov        ecx, dword ptr [ebp - 0x1f0]
00478d1e  and        dword ptr [ecx + 0x70], 0xfffffffd
00478d22  jmp        0x478d3d

// block 00478d24
00478d24  cmp        byte ptr [ebp - 0x1ec], 0
00478d2b  je         0x478d37

// block 00478d2d
00478d2d  mov        eax, dword ptr [ebp - 0x1f0]
00478d33  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00478d37
00478d37  mov        eax, dword ptr [ebp - 0x1c4]

// block 00478d3d
00478d3d  pop        ebx

// block 00478d3e
00478d3e  mov        ecx, dword ptr [ebp - 4]
00478d41  pop        edi
00478d42  xor        ecx, ebp
00478d44  pop        esi
00478d45  call       0x46c932

// block 00478d4a
00478d4a  leave      
00478d4b  ret        
