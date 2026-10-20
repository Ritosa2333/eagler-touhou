
// block 00450cc0
00450cc0  sub        esp, 0x7c
00450cc3  push       ebx
00450cc4  push       ebp
00450cc5  push       esi
00450cc6  push       edi
00450cc7  push       0x38
00450cc9  xor        ebx, ebx
00450ccb  lea        eax, [esp + 0x54]
00450ccf  push       ebx
00450cd0  push       eax
00450cd1  mov        byte ptr [esp + 0x23], 1
00450cd6  call       0x477420

// block 00450cdb
00450cdb  mov        eax, dword ptr [0x4ce8ec]
00450ce0  mov        ecx, dword ptr [eax]
00450ce2  add        esp, 0xc
00450ce5  lea        edx, [esp + 0x40]
00450ce9  push       edx
00450cea  push       ebx
00450ceb  push       eax
00450cec  mov        eax, dword ptr [ecx + 0x20]
00450cef  call       eax

// block 00450cf1
00450cf1  mov        ecx, dword ptr [esp + 0x40]
00450cf5  mov        edx, dword ptr [esp + 0x44]
00450cf9  mov        eax, dword ptr [esp + 0x48]
00450cfd  mov        dword ptr [0x4cea84], ecx
00450d03  mov        ecx, dword ptr [esp + 0x4c]
00450d07  mov        dword ptr [0x4cea88], edx
00450d0d  mov        dword ptr [0x4cea8c], eax
00450d12  mov        dword ptr [0x4cea90], ecx
00450d18  cmp        byte ptr [0x4ceacd], bl
00450d1e  je         0x450d4e

// block 00450d20
00450d20  cmp        eax, 0x3c
00450d23  je         0x450d42

// block 00450d25
00450d25  push       0x4a26d4
00450d2a  mov        ecx, 0x4b0ec8
00450d2f  call       0x464220

// block 00450d34
00450d34  mov        eax, dword ptr [esp + 0x4c]
00450d38  add        esp, 4
00450d3b  and        dword ptr [0x4cf428], 0xffffffef

// block 00450d42
00450d42  cmp        byte ptr [0x4ceacd], bl
00450d48  jne        0x450e4b

// block 00450d4e
00450d4e  test       byte ptr [0x4ceae8], 1
00450d55  je         0x450d68

// block 00450d57
00450d57  mov        dword ptr [esp + 0x58], 0x17
00450d5f  mov        byte ptr [0x4ceaca], 1
00450d66  jmp        0x450da1

// block 00450d68
00450d68  mov        al, byte ptr [0x4ceaca]
00450d6d  cmp        al, 0xff
00450d6f  jne        0x450d93

// block 00450d71
00450d71  push       0x4a2700
00450d76  mov        ecx, 0x4b0ec8
00450d7b  mov        dword ptr [esp + 0x5c], 0x16
00450d83  mov        byte ptr [0x4ceaca], bl
00450d89  call       0x464220

// block 00450d8e
00450d8e  add        esp, 4
00450d91  jmp        0x450da1

// block 00450d93
00450d93  xor        edx, edx
00450d95  cmp        al, bl
00450d97  setne      dl
00450d9a  add        edx, 0x16
00450d9d  mov        dword ptr [esp + 0x58], edx

// block 00450da1
00450da1  cmp        byte ptr [0x4cf418], bl
00450da7  je         0x450db6

// block 00450da9
00450da9  mov        ebp, 1
00450dae  mov        dword ptr [0x4cee64], ebp
00450db4  jmp        0x450e20

// block 00450db6
00450db6  cmp        dword ptr [0x4cee64], ebx
00450dbc  jne        0x450e1b

// block 00450dbe
00450dbe  test       byte ptr [0x4cf428], 0x10
00450dc5  mov        dword ptr [esp + 0x80], 0x3c
00450dd0  mov        edi, 0x80000000
00450dd5  je         0x450de0

// block 00450dd7
00450dd7  mov        dword ptr [esp + 0x84], edi
00450dde  jmp        0x450dfa

// block 00450de0
00450de0  xor        eax, eax
00450de2  cmp        byte ptr [0x4cead3], 3
00450de9  setne      al
00450dec  dec        eax
00450ded  and        eax, 0x7fffffff
00450df2  inc        eax
00450df3  mov        dword ptr [esp + 0x84], eax

// block 00450dfa
00450dfa  push       0x4a272c
00450dff  mov        ecx, 0x4b0ec8
00450e04  call       0x464220

// block 00450e09
00450e09  add        esp, 4
00450e0c  mov        dword ptr [esp + 0x68], 1
00450e14  mov        ebp, 1
00450e19  jmp        0x450e8c

// block 00450e1b
00450e1b  mov        ebp, 1

// block 00450e20
00450e20  mov        edi, 0x80000000
00450e25  push       0x4a2758
00450e2a  mov        ecx, 0x4b0ec8
00450e2f  mov        dword ptr [esp + 0x84], ebx
00450e36  mov        dword ptr [esp + 0x6c], ebp
00450e3a  mov        dword ptr [esp + 0x88], edi
00450e41  call       0x464220

// block 00450e46
00450e46  add        esp, 4
00450e49  jmp        0x450e8c

// block 00450e4b
00450e4b  test       byte ptr [0x4cf428], 0x10
00450e52  mov        ecx, dword ptr [esp + 0x4c]
00450e56  mov        esi, 1
00450e5b  mov        dword ptr [esp + 0x58], ecx
00450e5f  mov        dword ptr [esp + 0x68], esi
00450e63  mov        edi, 0x80000000
00450e68  jne        0x450e7f

// block 00450e6a
00450e6a  cmp        byte ptr [0x4cead3], 3
00450e71  je         0x450e7f

// block 00450e73
00450e73  mov        dword ptr [esp + 0x84], esi
00450e7a  cmp        eax, 0x3c
00450e7d  je         0x450e86

// block 00450e7f
00450e7f  mov        dword ptr [esp + 0x84], edi

// block 00450e86
00450e86  mov        dword ptr [esp + 0x70], esi
00450e8a  mov        ebp, esi

// block 00450e8c
00450e8c  or         dword ptr [0x4cee78], 2
00450e93  mov        dword ptr [esp + 0x50], 0x280
00450e9b  mov        dword ptr [esp + 0x54], 0x1e0
00450ea3  mov        dword ptr [esp + 0x74], ebp
00450ea7  mov        dword ptr [esp + 0x78], 0x50
00450eaf  mov        dword ptr [esp + 0x7c], ebp
00450eb3  mov        dword ptr [0x4cee68], ebp
00450eb9  xor        esi, esi
00450ebb  jmp        0x450ec0

// block 00450ec0
00450ec0  test       byte ptr [0x4ceae8], 2
00450ec7  jne        0x450f4d

// block 00450ecd
00450ecd  mov        eax, dword ptr [0x4ce8ec]
00450ed2  mov        edx, dword ptr [eax]
00450ed4  mov        edx, dword ptr [edx + 0x40]
00450ed7  push       0x4ce8f0
00450edc  lea        ecx, [esp + 0x54]
00450ee0  push       ecx
00450ee1  mov        ecx, dword ptr [0x4cf3f0]
00450ee7  push       0x40
00450ee9  push       ecx
00450eea  push       ebp
00450eeb  push       ebx
00450eec  push       eax
00450eed  call       edx

// block 00450eef
00450eef  test       eax, eax
00450ef1  jge        0x451024

// block 00450ef7
00450ef7  cmp        esi, ebx
00450ef9  je         0x450f0d

// block 00450efb
00450efb  push       0x4a277c
00450f00  mov        ecx, 0x4b0ec8
00450f05  call       0x464220

// block 00450f0a
00450f0a  add        esp, 4

// block 00450f0d
00450f0d  mov        eax, dword ptr [0x4ce8ec]
00450f12  mov        ecx, dword ptr [eax]
00450f14  push       0x4ce8f0
00450f19  lea        edx, [esp + 0x54]
00450f1d  push       edx
00450f1e  mov        edx, dword ptr [0x4cf3f0]
00450f24  push       0x20
00450f26  push       edx
00450f27  push       ebp
00450f28  push       ebx
00450f29  push       eax
00450f2a  mov        eax, dword ptr [ecx + 0x40]
00450f2d  call       eax

// block 00450f2f
00450f2f  test       eax, eax
00450f31  jge        0x451009

// block 00450f37
00450f37  cmp        esi, ebx
00450f39  je         0x450f4d

// block 00450f3b
00450f3b  push       0x4a27a0
00450f40  mov        ecx, 0x4b0ec8
00450f45  call       0x464220

// block 00450f4a
00450f4a  add        esp, 4

// block 00450f4d
00450f4d  mov        eax, dword ptr [0x4ce8ec]
00450f52  mov        ecx, dword ptr [eax]
00450f54  push       0x4ce8f0
00450f59  lea        edx, [esp + 0x54]
00450f5d  push       edx
00450f5e  mov        edx, dword ptr [0x4cf3f0]
00450f64  push       0x20
00450f66  push       edx
00450f67  push       2
00450f69  push       ebx
00450f6a  push       eax
00450f6b  mov        eax, dword ptr [ecx + 0x40]
00450f6e  call       eax

// block 00450f70
00450f70  test       eax, eax
00450f72  jge        0x450fea

// block 00450f74
00450f74  cmp        dword ptr [0x4cee64], ebx
00450f7a  jne        0x450fa2

// block 00450f7c
00450f7c  push       0x4a27c0
00450f81  mov        ecx, 0x4b0ec8
00450f86  call       0x464220

// block 00450f8b
00450f8b  add        esp, 4
00450f8e  mov        dword ptr [esp + 0x80], ebx
00450f95  mov        dword ptr [0x4cee68], ebx
00450f9b  mov        esi, ebp
00450f9d  jmp        0x450ec0

// block 00450fa2
00450fa2  cmp        dword ptr [esp + 0x84], ebp
00450fa9  jne        0x450fb7

// block 00450fab
00450fab  mov        dword ptr [esp + 0x84], edi
00450fb2  jmp        0x450ec0

// block 00450fb7
00450fb7  push       0x4a27e8
00450fbc  mov        edi, 0x4b0ec8
00450fc1  call       0x464300

// block 00450fc6
00450fc6  mov        eax, dword ptr [0x4ce8ec]
00450fcb  add        esp, 4
00450fce  cmp        eax, ebx
00450fd0  je         0x450fe0

// block 00450fd2
00450fd2  mov        ecx, dword ptr [eax]
00450fd4  mov        edx, dword ptr [ecx + 8]
00450fd7  push       eax
00450fd8  call       edx

// block 00450fda
00450fda  mov        dword ptr [0x4ce8ec], ebx

// block 00450fe0
00450fe0  mov        eax, ebp
00450fe2  pop        edi
00450fe3  pop        esi
00450fe4  pop        ebp
00450fe5  pop        ebx
00450fe6  add        esp, 0x7c
00450fe9  ret        

// block 00450fea
00450fea  push       0x4a2820
00450fef  mov        ecx, 0x4b0ec8
00450ff4  call       0x464220

// block 00450ff9
00450ff9  add        esp, 4
00450ffc  and        dword ptr [0x4cee78], 0xfffffffe
00451003  mov        byte ptr [esp + 0x17], bl
00451007  jmp        0x45103c

// block 00451009
00451009  push       0x4a285c
0045100e  mov        ecx, 0x4b0ec8
00451013  call       0x464220

// block 00451018
00451018  add        esp, 4
0045101b  and        dword ptr [0x4cee78], 0xfffffffe
00451022  jmp        0x45103c

// block 00451024
00451024  push       0x4a2870
00451029  mov        ecx, 0x4b0ec8
0045102e  call       0x464220

// block 00451033
00451033  add        esp, 4
00451036  or         dword ptr [0x4cee78], ebp

// block 0045103c
0045103c  fld        dword ptr [0x4a3ef0]
00451042  mov        ecx, 0xe
00451047  lea        esi, [esp + 0x50]
0045104b  fstp       dword ptr [esp + 0x34]
0045104f  fld        dword ptr [0x4a3eec]
00451055  mov        edi, 0x4ce9dc

// block 0045105a
0045105a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045105c
0045105c  fstp       dword ptr [esp + 0x38]
00451060  fld        dword ptr [0x4a3ee8]
00451066  push       ecx
00451067  fstp       dword ptr [esp]
0045106a  call       0x4317d0

// block 0045106f
0045106f  fdivr      qword ptr [0x4a3ee0]
00451075  lea        eax, [esp + 0x1c]
00451079  push       eax
0045107a  lea        ecx, [esp + 0x2c]
0045107e  push       ecx
0045107f  lea        edx, [esp + 0x3c]
00451083  push       edx
00451084  push       0x4ce944
00451089  fstp       dword ptr [esp + 0x28]
0045108d  fld        dword ptr [esp + 0x28]
00451091  fchs       
00451093  fstp       dword ptr [esp + 0x4c]
00451097  fld        dword ptr [0x4a3ef0]
0045109d  fstp       dword ptr [esp + 0x38]
004510a1  fld        dword ptr [0x4a3eec]
004510a7  fstp       dword ptr [esp + 0x3c]
004510ab  fldz       
004510ad  fst        dword ptr [esp + 0x40]
004510b1  fst        dword ptr [esp + 0x2c]
004510b5  fld1       
004510b7  fstp       dword ptr [esp + 0x30]
004510bb  fstp       dword ptr [esp + 0x34]
004510bf  call       0x46c6da

// block 004510c4
004510c4  fld        dword ptr [0x4a3e20]
004510ca  sub        esp, 0x10
004510cd  fstp       dword ptr [esp + 0xc]
004510d1  fld        dword ptr [0x4a3edc]
004510d7  fstp       dword ptr [esp + 8]
004510db  fld        dword ptr [0x4a3ed8]
004510e1  fstp       dword ptr [esp + 4]
004510e5  fld        dword ptr [0x4a3e0c]
004510eb  fstp       dword ptr [esp]
004510ee  push       0x4ce984
004510f3  call       0x46c6e0

// block 004510f8
004510f8  mov        eax, dword ptr [0x4ce8f0]
004510fd  mov        ecx, dword ptr [eax]
004510ff  mov        edx, dword ptr [ecx + 0xb0]
00451105  push       0x4ce944
0045110a  push       2
0045110c  push       eax
0045110d  call       edx

// block 0045110f
0045110f  mov        eax, dword ptr [0x4ce8f0]
00451114  mov        ecx, dword ptr [eax]
00451116  mov        edx, dword ptr [ecx + 0xb0]
0045111c  push       0x4ce984
00451121  push       3
00451123  push       eax
00451124  call       edx

// block 00451126
00451126  mov        eax, dword ptr [0x4ce8f0]
0045112b  mov        ecx, dword ptr [eax]
0045112d  mov        edx, dword ptr [ecx + 0xc0]
00451133  push       0x4ce9c4
00451138  push       eax
00451139  call       edx

// block 0045113b
0045113b  mov        eax, dword ptr [0x4ce8f0]
00451140  mov        ecx, dword ptr [eax]
00451142  mov        edx, dword ptr [ecx + 0x1c]
00451145  push       0x4cee84
0045114a  push       eax
0045114b  call       edx

// block 0045114d
0045114d  test       byte ptr [0x4cef14], 0x40
00451154  jne        0x451168

// block 00451156
00451156  push       0x4a2890
0045115b  mov        ecx, 0x4b0ec8
00451160  call       0x464220

// block 00451165
00451165  add        esp, 4

// block 00451168
00451168  cmp        dword ptr [0x4ceedc], 0x100
00451172  ja         0x451186

// block 00451174
00451174  push       0x4a28e0
00451179  mov        ecx, 0x4b0ec8
0045117e  call       0x464220

// block 00451183
00451183  add        esp, 4

// block 00451186
00451186  test       byte ptr [0x4ceae8], 1
0045118d  jne        0x4511da

// block 0045118f
0045118f  cmp        byte ptr [esp + 0x17], bl
00451193  je         0x4511da

// block 00451195
00451195  mov        edx, dword ptr [esp + 0x58]
00451199  mov        eax, dword ptr [0x4ce8ec]
0045119e  mov        ecx, dword ptr [eax]
004511a0  push       0x15
004511a2  push       3
004511a4  push       ebx
004511a5  push       edx
004511a6  push       ebp
004511a7  push       ebx
004511a8  push       eax
004511a9  mov        eax, dword ptr [ecx + 0x28]
004511ac  call       eax

// block 004511ae
004511ae  test       eax, eax
004511b0  jne        0x4511bb

// block 004511b2
004511b2  or         dword ptr [0x4cee78], 4
004511b9  jmp        0x4511da

// block 004511bb
004511bb  and        dword ptr [0x4cee78], 0xfffffffb
004511c2  or         dword ptr [0x4ceae8], ebp
004511c8  push       0x4a2930
004511cd  mov        ecx, 0x4b0ec8
004511d2  call       0x464220

// block 004511d7
004511d7  add        esp, 4

// block 004511da
004511da  call       0x451200

// block 004511df
004511df  call       0x451e60

// block 004511e4
004511e4  pop        edi
004511e5  pop        esi
004511e6  pop        ebp
004511e7  mov        dword ptr [0x4cf3f4], ebx
004511ed  mov        dword ptr [0x4cee6c], ebx
004511f3  xor        eax, eax
004511f5  pop        ebx
004511f6  add        esp, 0x7c
004511f9  ret        
