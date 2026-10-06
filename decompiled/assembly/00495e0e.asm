
// block 00495e0e
00495e0e  pextrw     eax, xmm0, 3
00495e13  and        ax, 0x7fff
00495e17  sub        ax, 0x3030
00495e1b  cmp        ax, 0x10c5
00495e1f  ja         0x495f67

// block 00495e25
00495e25  movlpd     xmm1, qword ptr [0x4a7e50]
00495e2d  mulsd      xmm1, xmm0
00495e31  movlpd     xmm2, qword ptr [0x4a7e58]
00495e39  cvtsd2si   edx, xmm1
00495e3d  addsd      xmm1, xmm2
00495e41  movlpd     xmm3, qword ptr [0x4a7e70]
00495e49  subsd      xmm1, xmm2
00495e4d  movapd     xmm2, xmmword ptr [0x4a7e60]
00495e55  mulsd      xmm3, xmm1
00495e59  unpcklpd   xmm1, xmm1
00495e5d  add        edx, 0x1c7600
00495e63  movsd      xmm4, xmm0
00495e67  and        edx, 0x3f
00495e6a  movapd     xmm5, xmmword ptr [0x4a7e40]
00495e72  lea        eax, [0x4a7610]
00495e78  shl        edx, 5
00495e7b  add        eax, edx
00495e7d  mulpd      xmm2, xmm1
00495e81  subsd      xmm0, xmm3
00495e85  mulsd      xmm1, qword ptr [0x4a7e78]
00495e8d  subsd      xmm4, xmm3
00495e91  movlpd     xmm7, qword ptr [eax + 8]
00495e96  unpcklpd   xmm0, xmm0
00495e9a  movsd      xmm3, xmm4
00495e9e  subsd      xmm4, xmm2
00495ea2  mulpd      xmm5, xmm0
00495ea6  subpd      xmm0, xmm2
00495eaa  movapd     xmm6, xmmword ptr [0x4a7e20]
00495eb2  mulsd      xmm7, xmm4
00495eb6  subsd      xmm3, xmm4
00495eba  mulpd      xmm5, xmm0
00495ebe  mulpd      xmm0, xmm0
00495ec2  subsd      xmm3, xmm2
00495ec6  movapd     xmm2, xmmword ptr [eax]
00495eca  subsd      xmm1, xmm3
00495ece  movlpd     xmm3, qword ptr [eax + 0x18]
00495ed3  addsd      xmm2, xmm3
00495ed7  subsd      xmm7, xmm2
00495edb  mulsd      xmm2, xmm4
00495edf  mulpd      xmm6, xmm0
00495ee3  mulsd      xmm3, xmm4
00495ee7  mulpd      xmm2, xmm0
00495eeb  mulpd      xmm0, xmm0
00495eef  addpd      xmm5, xmmword ptr [0x4a7e30]
00495ef7  mulsd      xmm4, qword ptr [eax]
00495efb  addpd      xmm6, xmmword ptr [0x4a7e10]
00495f03  mulpd      xmm5, xmm0
00495f07  movsd      xmm0, xmm3
00495f0b  addsd      xmm3, qword ptr [eax + 8]
00495f10  mulsd      xmm1, xmm7
00495f14  movsd      xmm7, xmm4
00495f18  addsd      xmm4, xmm3
00495f1c  addpd      xmm6, xmm5
00495f20  movlpd     xmm5, qword ptr [eax + 8]
00495f25  subsd      xmm5, xmm3
00495f29  subsd      xmm3, xmm4
00495f2d  addsd      xmm1, qword ptr [eax + 0x10]
00495f32  mulpd      xmm6, xmm2
00495f36  addsd      xmm5, xmm0
00495f3a  addsd      xmm3, xmm7
00495f3e  addsd      xmm1, xmm5
00495f42  addsd      xmm1, xmm3
00495f46  addsd      xmm1, xmm6
00495f4a  unpckhpd   xmm6, xmm6
00495f4e  addsd      xmm1, xmm6
00495f52  sub        esp, 0x10
00495f55  addsd      xmm4, xmm1
00495f59  movlpd     qword ptr [esp + 4], xmm4
00495f5f  fld        qword ptr [esp + 4]
00495f63  add        esp, 0x10
00495f66  ret        

// block 00495f67
00495f67  jg         0x495fb2

// block 00495f69
00495f69  sub        esp, 0x10
00495f6c  shr        ax, 4
00495f70  cmp        ax, 0xcfd
00495f74  jne        0x495f8c

// block 00495f76
00495f76  mulsd      xmm0, qword ptr [0x4a7e90]
00495f7e  movlpd     qword ptr [esp + 4], xmm0
00495f84  fld        qword ptr [esp + 4]
00495f88  add        esp, 0x10
00495f8b  ret        

// block 00495f8c
00495f8c  movlpd     xmm3, qword ptr [0x4a7e80]
00495f94  mulsd      xmm3, xmm0
00495f98  subsd      xmm3, xmm0
00495f9c  mulsd      xmm3, qword ptr [0x4a7e88]
00495fa4  movlpd     qword ptr [esp + 4], xmm0
00495faa  fld        qword ptr [esp + 4]
00495fae  add        esp, 0x10
00495fb1  ret        

// block 00495fb2
00495fb2  jmp        0x49390f
