
// block 00495afe
00495afe  pextrw     eax, xmm0, 3
00495b03  and        ax, 0x7fff
00495b07  sub        ax, 0x3820
00495b0b  cmp        ax, 0x8a8
00495b0f  ja         0x495cec

// block 00495b15
00495b15  unpcklpd   xmm0, xmm0
00495b19  movapd     xmm1, xmmword ptr [0x4a7570]
00495b21  mulpd      xmm1, xmm0
00495b25  cvtsd2si   edx, xmm1
00495b29  movapd     xmm2, xmmword ptr [0x4a7580]
00495b31  addpd      xmm1, xmm2
00495b35  movapd     xmm3, xmmword ptr [0x4a7590]
00495b3d  subpd      xmm1, xmm2
00495b41  movlpd     xmm5, qword ptr [0x4a75c0]
00495b49  add        edx, 0x72900
00495b4f  movapd     xmm4, xmmword ptr [0x4a75a0]
00495b57  mulpd      xmm3, xmm1
00495b5b  and        edx, 0x1f
00495b5e  mulsd      xmm5, xmm1
00495b62  mov        ecx, edx
00495b64  mulpd      xmm4, xmm1
00495b68  shl        ecx, 1
00495b6a  subpd      xmm0, xmm3
00495b6e  mulpd      xmm1, xmmword ptr [0x4a75b0]
00495b76  add        edx, ecx
00495b78  shl        ecx, 2
00495b7b  add        edx, ecx
00495b7d  addsd      xmm5, xmm0
00495b81  movapd     xmm2, xmm0
00495b85  subpd      xmm0, xmm4
00495b89  movlpd     xmm6, qword ptr [0x4a75c8]
00495b91  shl        edx, 4
00495b94  lea        eax, [0x4a5f70]
00495b9a  andpd      xmm5, xmmword ptr [0x4a75d0]
00495ba2  movapd     xmm3, xmm0
00495ba6  add        eax, edx
00495ba8  subpd      xmm2, xmm0
00495bac  unpckhpd   xmm0, xmm0
00495bb0  divsd      xmm6, xmm5
00495bb4  subpd      xmm2, xmm4
00495bb8  movapd     xmm7, xmmword ptr [eax + 0x10]
00495bbd  subsd      xmm3, xmm5
00495bc1  mulpd      xmm7, xmm0
00495bc5  subpd      xmm2, xmm1
00495bc9  movapd     xmm1, xmmword ptr [eax + 0x30]
00495bce  mulpd      xmm1, xmm0
00495bd2  movapd     xmm4, xmmword ptr [eax + 0x60]
00495bd7  mulpd      xmm4, xmm0
00495bdb  addsd      xmm2, xmm3
00495bdf  movapd     xmm3, xmm0
00495be3  mulpd      xmm0, xmm0
00495be7  addpd      xmm7, xmmword ptr [eax]
00495beb  addpd      xmm1, xmmword ptr [eax + 0x20]
00495bf0  mulpd      xmm1, xmm0
00495bf4  addpd      xmm4, xmmword ptr [eax + 0x50]
00495bf9  addpd      xmm7, xmm1
00495bfd  movapd     xmm1, xmmword ptr [eax + 0x70]
00495c02  mulpd      xmm1, xmm0
00495c06  mulpd      xmm0, xmm0
00495c0a  addpd      xmm4, xmm1
00495c0e  movapd     xmm1, xmmword ptr [eax + 0x40]
00495c13  mulpd      xmm1, xmm0
00495c17  addpd      xmm7, xmm1
00495c1b  movapd     xmm1, xmm3
00495c1f  mulpd      xmm3, xmm0
00495c23  mulsd      xmm0, xmm0
00495c27  mulpd      xmm1, xmmword ptr [eax + 0x90]
00495c2f  mulpd      xmm4, xmm3
00495c33  movsd      xmm3, xmm1
00495c37  addpd      xmm7, xmm4
00495c3b  movsd      xmm4, xmm1
00495c3f  mulsd      xmm0, xmm7
00495c43  unpckhpd   xmm7, xmm7
00495c47  addsd      xmm0, xmm7
00495c4b  unpckhpd   xmm1, xmm1
00495c4f  addsd      xmm3, xmm1
00495c53  subsd      xmm4, xmm3
00495c57  addsd      xmm1, xmm4
00495c5b  movsd      xmm4, xmm2
00495c5f  movlpd     xmm7, qword ptr [eax + 0x90]
00495c67  unpckhpd   xmm2, xmm2
00495c6b  addsd      xmm7, qword ptr [eax + 0x98]
00495c73  mulsd      xmm7, xmm2
00495c77  addsd      xmm7, qword ptr [eax + 0x88]
00495c7f  addsd      xmm7, xmm1
00495c83  addsd      xmm0, xmm7
00495c87  movlpd     xmm7, qword ptr [0x4a75c8]
00495c8f  mulsd      xmm4, xmm6
00495c93  movlpd     xmm2, qword ptr [eax + 0xa8]
00495c9b  andpd      xmm2, xmm6
00495c9f  mulsd      xmm5, xmm2

// block 00495ca3
00495ca3  mulsd      xmm6, qword ptr [eax + 0xa0]
00495cab  subsd      xmm7, xmm5
00495caf  subsd      xmm2, qword ptr [eax + 0x80]
00495cb7  subsd      xmm7, xmm4
00495cbb  mulsd      xmm7, xmm6
00495cbf  movsd      xmm4, xmm3
00495cc3  subsd      xmm3, xmm2
00495cc7  addsd      xmm2, xmm3
00495ccb  subsd      xmm4, xmm2
00495ccf  addsd      xmm0, xmm4
00495cd3  subsd      xmm0, xmm7
00495cd7  sub        esp, 0x10
00495cda  addsd      xmm0, xmm3
00495cde  movlpd     qword ptr [esp + 4], xmm0
00495ce4  fld        qword ptr [esp + 4]
00495ce8  add        esp, 0x10
00495ceb  ret        

// block 00495cec
00495cec  jg         0x495d2d

// block 00495cee
00495cee  sub        esp, 0x10
00495cf1  shr        ax, 4
00495cf5  cmp        ax, 0xc7e
00495cf9  jne        0x495d07

// block 00495cfb
00495cfb  movsd      xmm3, xmm0
00495cff  mulsd      xmm3, qword ptr [0x4a75f0]

// block 00495d07
00495d07  movlpd     xmm3, qword ptr [0x4a75e8]
00495d0f  mulsd      xmm3, xmm0
00495d13  addsd      xmm3, xmm0
00495d17  mulsd      xmm3, qword ptr [0x4a75f0]
00495d1f  movlpd     qword ptr [esp + 4], xmm3
00495d25  fld        qword ptr [esp + 4]
00495d29  add        esp, 0x10
00495d2c  ret        

// block 00495d2d
00495d2d  jmp        0x4936ff
