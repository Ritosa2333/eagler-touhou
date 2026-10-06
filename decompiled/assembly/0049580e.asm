
// block 0049580e
0049580e  unpcklpd   xmm7, xmm7
00495812  movapd     xmm2, xmm7
00495816  andpd      xmm2, xmmword ptr [0x4a5e30]
0049581e  comisd     xmm2, xmmword ptr [0x4a5f58]
00495826  jp         0x495ab2

// block 0049582c
0049582c  jae        0x495a87

// block 00495832
00495832  comisd     xmm2, xmmword ptr [0x4a5f48]
0049583a  jae        0x4958aa

// block 0049583c
0049583c  comisd     xmm2, xmmword ptr [0x4a5f50]
00495844  jb         0x495a40

// block 0049584a
0049584a  movapd     xmm1, xmm2
0049584e  mulpd      xmm1, xmm2
00495852  movapd     xmm3, xmm1
00495856  mulpd      xmm3, xmm1
0049585a  movapd     xmm5, xmmword ptr [0x4a5f00]
00495862  mulpd      xmm5, xmm3
00495866  addpd      xmm5, xmmword ptr [0x4a5ef0]
0049586e  mulpd      xmm5, xmm3
00495872  addpd      xmm5, xmmword ptr [0x4a5ee0]
0049587a  mulpd      xmm5, xmm3
0049587e  addpd      xmm5, xmmword ptr [0x4a5ed0]
00495886  mulsd      xmm5, xmm1
0049588a  movapd     xmm3, xmm5
0049588e  shufpd     xmm3, xmm3, 1
00495893  addsd      xmm5, xmm3
00495897  mulsd      xmm5, xmm7
0049589b  subsd      xmm7, xmm5
0049589f  movq       qword ptr [esp + 4], xmm7
004958a5  fld        qword ptr [esp + 4]
004958a9  ret        

// block 004958aa
004958aa  comisd     xmm2, xmmword ptr [0x4a5f40]
004958b2  jae        0x495948

// block 004958b8
004958b8  movapd     xmm1, xmm2
004958bc  mulpd      xmm1, xmm2
004958c0  movapd     xmm3, xmm1
004958c4  mulpd      xmm3, xmm1
004958c8  movapd     xmm5, xmmword ptr [0x4a5ec0]
004958d0  mulpd      xmm5, xmm3
004958d4  addpd      xmm5, xmmword ptr [0x4a5eb0]
004958dc  mulpd      xmm5, xmm3
004958e0  addpd      xmm5, xmmword ptr [0x4a5ea0]
004958e8  mulpd      xmm5, xmm3
004958ec  addpd      xmm5, xmmword ptr [0x4a5e90]
004958f4  mulpd      xmm5, xmm3
004958f8  addpd      xmm5, xmmword ptr [0x4a5e80]
00495900  mulpd      xmm5, xmm3
00495904  addpd      xmm5, xmmword ptr [0x4a5e70]
0049590c  mulpd      xmm5, xmm3
00495910  addpd      xmm5, xmmword ptr [0x4a5e60]
00495918  mulpd      xmm5, xmm3
0049591c  addpd      xmm5, xmmword ptr [0x4a5e50]
00495924  mulsd      xmm5, xmm1
00495928  movapd     xmm3, xmm5
0049592c  shufpd     xmm3, xmm3, 1
00495931  addsd      xmm5, xmm3
00495935  mulsd      xmm5, xmm7
00495939  subsd      xmm7, xmm5
0049593d  movq       qword ptr [esp + 4], xmm7
00495943  fld        qword ptr [esp + 4]
00495947  ret        

// block 00495948
00495948  movq       xmm6, xmm7
0049594c  xorpd      xmm6, xmm2
00495950  comisd     xmm2, xmmword ptr [0x4a5f38]
00495958  jae        0x4959a9

// block 0049595a
0049595a  movq       xmm0, qword ptr [0x4a5f30]
00495962  movq       xmm5, qword ptr [0x4a5f10]
0049596a  movq       xmm3, xmm2
0049596e  addsd      xmm3, xmm0
00495972  psrlq      xmm3, 0x2c
00495977  psubd      xmm3, xmm5
0049597b  movd       eax, xmm3
0049597f  lea        eax, [eax + eax*2]
00495982  movq       xmm5, qword ptr [eax*8 + 0x4a87e8]
0049598b  movq       xmm3, xmm2
0049598f  subsd      xmm2, xmm5
00495993  mulsd      xmm3, xmm5
00495997  addsd      xmm3, qword ptr [0x4a5f28]
0049599f  divsd      xmm2, xmm3
004959a3  unpcklpd   xmm2, xmm2
004959a7  jmp        0x4959c2

// block 004959a9
004959a9  mov        eax, 0x300
004959ae  movq       xmm0, xmm2
004959b2  movq       xmm2, qword ptr [0x4a5f20]
004959ba  divsd      xmm2, xmm0
004959be  unpcklpd   xmm2, xmm2

// block 004959c2
004959c2  movq       xmm0, qword ptr [eax*8 + 0x4a87d8]
004959cb  movq       xmm4, qword ptr [eax*8 + 0x4a87e0]
004959d4  movapd     xmm1, xmm2
004959d8  mulpd      xmm1, xmm2
004959dc  movapd     xmm3, xmm1
004959e0  mulpd      xmm3, xmm1
004959e4  movapd     xmm5, xmmword ptr [0x4a5f00]
004959ec  mulpd      xmm5, xmm3
004959f0  addpd      xmm5, xmmword ptr [0x4a5ef0]
004959f8  mulpd      xmm5, xmm3
004959fc  addpd      xmm5, xmmword ptr [0x4a5ee0]
00495a04  mulpd      xmm5, xmm3
00495a08  addpd      xmm5, xmmword ptr [0x4a5ed0]
00495a10  mulsd      xmm5, xmm1
00495a14  movapd     xmm3, xmm5
00495a18  shufpd     xmm3, xmm3, 1
00495a1d  addsd      xmm5, xmm3
00495a21  mulsd      xmm5, xmm2
00495a25  subsd      xmm5, xmm4
00495a29  subsd      xmm5, xmm2
00495a2d  subsd      xmm0, xmm5
00495a31  orpd       xmm0, xmm6
00495a35  movq       qword ptr [esp + 4], xmm0
00495a3b  fld        qword ptr [esp + 4]
00495a3f  ret        

// block 00495a40
00495a40  comisd     xmm2, xmmword ptr [0x4a5f18]
00495a48  jne        0x495a4f

// block 00495a4a
00495a4a  fld        qword ptr [esp + 4]
00495a4e  ret        

// block 00495a4f
00495a4f  comisd     xmm2, xmmword ptr [0x4a5f60]
00495a57  jae        0x495a76

// block 00495a59
00495a59  fld        qword ptr [0x4a5f68]
00495a5f  fmul       qword ptr [0x4a5f68]
00495a65  sub        esp, 8
00495a68  fstp       qword ptr [esp]
00495a6b  fld        qword ptr [esp]
00495a6e  add        esp, 8
00495a71  fadd       qword ptr [esp + 4]
00495a75  ret        

// block 00495a76
00495a76  fld        qword ptr [0x4a5f68]
00495a7c  fmul       qword ptr [0x4a5f68]
00495a82  fadd       qword ptr [esp + 4]
00495a86  ret        

// block 00495a87
00495a87  movq       xmm0, xmm2
00495a8b  movq       xmm3, qword ptr [0x4a5e20]
00495a93  andpd      xmm0, xmm3
00495a97  ucomisd    xmm0, xmm3
00495a9b  jp         0x495ab2

// block 00495a9d
00495a9d  mov        eax, dword ptr [esp + 8]
00495aa1  shr        eax, 0x1f
00495aa4  fld        qword ptr [0x4a5f68]
00495aaa  fadd       qword ptr [eax*8 + 0x4a5e40]
00495ab1  ret        

// block 00495ab2
00495ab2  mov        edx, 0x3eb
00495ab7  sub        esp, 0x10
00495aba  mov        dword ptr [esp + 0xc], edx
00495abe  mov        edx, esp
00495ac0  add        edx, 0x14
00495ac3  mov        dword ptr [esp + 8], edx
00495ac7  mov        dword ptr [esp + 4], edx
00495acb  mov        dword ptr [esp], edx
00495ace  call       0x493b07

// block 00495ad3
00495ad3  add        esp, 0x10
00495ad6  fld        qword ptr [esp + 4]
00495ada  ret        
