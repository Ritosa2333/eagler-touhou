
// block 0045bce0
0045bce0  sub        esp, 0xcc
0045bce6  push       ebx
0045bce7  push       ebp
0045bce8  mov        ebp, dword ptr [esp + 0xdc]
0045bcef  mov        eax, dword ptr [ebp + 0x47c]
0045bcf5  push       esi
0045bcf6  push       edi
0045bcf7  test       al, 1
0045bcf9  jne        0x45bd0b

// block 0045bcfb
0045bcfb  or         eax, 0xffffffff
0045bcfe  pop        edi
0045bcff  pop        esi
0045bd00  pop        ebp
0045bd01  pop        ebx
0045bd02  add        esp, 0xcc
0045bd08  ret        8

// block 0045bd0b
0045bd0b  test       al, 2
0045bd0d  je         0x45bcfb

// block 0045bd0f
0045bd0f  cmp        byte ptr [ebp + 0x3bf], 0
0045bd16  je         0x45bcfb

// block 0045bd18
0045bd18  mov        ebx, dword ptr [esp + 0xe0]
0045bd1f  cmp        dword ptr [ebx + 0x4b56a0], 0
0045bd26  je         0x45bd2f

// block 0045bd28
0045bd28  mov        esi, ebx
0045bd2a  call       0x45a3c0

// block 0045bd2f
0045bd2f  mov        eax, dword ptr [ebp + 0x47c]
0045bd35  test       eax, 0x8000
0045bd3a  jne        0x45be17

// block 0045bd40
0045bd40  test       al, 0xc
0045bd42  je         0x45be17

// block 0045bd48
0045bd48  fld        dword ptr [ebp + 0x40]
0045bd4b  lea        ebx, [ebp + 0x33c]
0045bd51  lea        esi, [ebp + 0x2fc]
0045bd57  mov        ecx, 0x10
0045bd5c  mov        edi, ebx

// block 0045bd5e
0045bd5e  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045bd60
0045bd60  fmul       dword ptr [ebx]
0045bd62  fstp       dword ptr [ebx]
0045bd64  fld        dword ptr [ebp + 0x44]
0045bd67  fmul       dword ptr [ebp + 0x350]
0045bd6d  fstp       dword ptr [ebp + 0x350]
0045bd73  fldz       
0045bd75  fcomp      dword ptr [ebp + 0x24]
0045bd78  and        eax, 0xfffffff7
0045bd7b  mov        dword ptr [ebp + 0x47c], eax
0045bd81  fnstsw     ax
0045bd83  test       ah, 0x44
0045bd86  jnp        0x45bdab

// block 0045bd88
0045bd88  fld        dword ptr [ebp + 0x24]
0045bd8b  push       ecx
0045bd8c  lea        eax, [esp + 0x9c]
0045bd93  fstp       dword ptr [esp]
0045bd96  push       eax
0045bd97  call       0x46c6f2

// block 0045bd9c
0045bd9c  lea        ecx, [esp + 0x98]
0045bda3  push       ecx
0045bda4  push       ebx
0045bda5  push       ebx
0045bda6  call       0x46c6f8

// block 0045bdab
0045bdab  fldz       
0045bdad  fcomp      dword ptr [ebp + 0x28]
0045bdb0  fnstsw     ax
0045bdb2  test       ah, 0x44
0045bdb5  jnp        0x45bdda

// block 0045bdb7
0045bdb7  fld        dword ptr [ebp + 0x28]
0045bdba  push       ecx
0045bdbb  lea        edx, [esp + 0x9c]
0045bdc2  fstp       dword ptr [esp]
0045bdc5  push       edx
0045bdc6  call       0x46c6fe

// block 0045bdcb
0045bdcb  lea        eax, [esp + 0x98]
0045bdd2  push       eax
0045bdd3  push       ebx
0045bdd4  push       ebx
0045bdd5  call       0x46c6f8

// block 0045bdda
0045bdda  fldz       
0045bddc  fcomp      dword ptr [ebp + 0x2c]
0045bddf  fnstsw     ax
0045bde1  test       ah, 0x44
0045bde4  jnp        0x45be09

// block 0045bde6
0045bde6  fld        dword ptr [ebp + 0x2c]
0045bde9  push       ecx
0045bdea  lea        ecx, [esp + 0x9c]
0045bdf1  fstp       dword ptr [esp]
0045bdf4  push       ecx
0045bdf5  call       0x46c704

// block 0045bdfa
0045bdfa  lea        edx, [esp + 0x98]
0045be01  push       edx
0045be02  push       ebx
0045be03  push       ebx
0045be04  call       0x46c6f8

// block 0045be09
0045be09  and        dword ptr [ebp + 0x47c], 0xfffffffb
0045be10  mov        ebx, dword ptr [esp + 0xe0]

// block 0045be17
0045be17  fld        qword ptr [0x4a3cf8]
0045be1d  lea        esi, [ebp + 0x33c]
0045be23  mov        ecx, 0x10
0045be28  lea        edi, [esp + 0x18]

// block 0045be2c
0045be2c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045be2e
0045be2e  mov        ecx, dword ptr [ebp + 0x47c]
0045be34  mov        eax, ecx
0045be36  shr        eax, 0x13
0045be39  and        eax, 3
0045be3c  sub        eax, 0
0045be3f  je         0x45bea9

// block 0045be41
0045be41  sub        eax, 1
0045be44  je         0x45be7b

// block 0045be46
0045be46  sub        eax, 1
0045be49  jne        0x45bebf

// block 0045be4b
0045be4b  fld        dword ptr [ebp + 0x58]
0045be4e  fmul       dword ptr [ebp + 0x40]
0045be51  fmul       st(1)
0045be53  fstp       dword ptr [esp + 0x14]
0045be57  fld        dword ptr [esp + 0x14]
0045be5b  fabs       
0045be5d  fstp       dword ptr [esp + 0x14]
0045be61  fld        dword ptr [esp + 0x14]
0045be65  fld        dword ptr [ebp + 0x430]
0045be6b  fadd       dword ptr [ebp + 0x424]
0045be71  fadd       dword ptr [ebp + 0x43c]
0045be77  faddp      st(1)
0045be79  jmp        0x45bebb

// block 0045be7b
0045be7b  fld        dword ptr [ebp + 0x430]
0045be81  fadd       dword ptr [ebp + 0x424]
0045be87  fadd       dword ptr [ebp + 0x43c]
0045be8d  fld        dword ptr [ebp + 0x58]
0045be90  fmul       dword ptr [ebp + 0x40]
0045be93  fmul       st(2)
0045be95  fstp       dword ptr [esp + 0x14]
0045be99  fld        dword ptr [esp + 0x14]
0045be9d  fabs       
0045be9f  fstp       dword ptr [esp + 0x14]
0045bea3  fsub       dword ptr [esp + 0x14]
0045bea7  jmp        0x45bebb

// block 0045bea9
0045bea9  fld        dword ptr [ebp + 0x430]
0045beaf  fadd       dword ptr [ebp + 0x424]
0045beb5  fadd       dword ptr [ebp + 0x43c]

// block 0045bebb
0045bebb  fstp       dword ptr [esp + 0x48]

// block 0045bebf
0045bebf  shr        ecx, 0x15
0045bec2  and        ecx, 3
0045bec5  sub        ecx, 0
0045bec8  je         0x45bf40

// block 0045beca
0045beca  sub        ecx, 1
0045becd  je         0x45bf0c

// block 0045becf
0045becf  sub        ecx, 1
0045bed2  jne        0x45bf5a

// block 0045bed8
0045bed8  fld        dword ptr [ebp + 0x5c]
0045bedb  fmul       dword ptr [ebp + 0x44]
0045bede  fmulp      st(1)
0045bee0  fstp       dword ptr [esp + 0x14]
0045bee4  fld        dword ptr [esp + 0x14]
0045bee8  fabs       
0045beea  fstp       dword ptr [esp + 0x14]
0045beee  fld        dword ptr [esp + 0x14]
0045bef2  fld        dword ptr [ebp + 0x434]
0045bef8  fadd       dword ptr [ebp + 0x428]
0045befe  fadd       dword ptr [ebp + 0x440]
0045bf04  faddp      st(1)
0045bf06  fstp       dword ptr [esp + 0x4c]
0045bf0a  jmp        0x45bf5c

// block 0045bf0c
0045bf0c  fld        dword ptr [ebp + 0x434]
0045bf12  fadd       dword ptr [ebp + 0x428]
0045bf18  fadd       dword ptr [ebp + 0x440]
0045bf1e  fld        dword ptr [ebp + 0x5c]
0045bf21  fmul       dword ptr [ebp + 0x44]
0045bf24  fmulp      st(2)
0045bf26  fxch       st(1)
0045bf28  fstp       dword ptr [esp + 0x14]
0045bf2c  fld        dword ptr [esp + 0x14]
0045bf30  fabs       
0045bf32  fstp       dword ptr [esp + 0x14]
0045bf36  fsub       dword ptr [esp + 0x14]
0045bf3a  fstp       dword ptr [esp + 0x4c]
0045bf3e  jmp        0x45bf5c

// block 0045bf40
0045bf40  fstp       st(0)
0045bf42  fld        dword ptr [ebp + 0x434]
0045bf48  fadd       dword ptr [ebp + 0x428]
0045bf4e  fadd       dword ptr [ebp + 0x440]
0045bf54  fstp       dword ptr [esp + 0x4c]
0045bf58  jmp        0x45bf5c

// block 0045bf5a
0045bf5a  fstp       st(0)

// block 0045bf5c
0045bf5c  fld        dword ptr [ebp + 0x438]
0045bf62  push       ebp
0045bf63  fadd       dword ptr [ebp + 0x42c]
0045bf69  mov        eax, ebx
0045bf6b  fstp       dword ptr [esp + 0x54]
0045bf6f  call       0x459a60

// block 0045bf74
0045bf74  fld        dword ptr [ebp + 0x438]
0045bf7a  mov        eax, dword ptr [0x4ce8f0]
0045bf7f  fadd       dword ptr [ebp + 0x42c]
0045bf85  lea        edx, [esp + 0x18]
0045bf89  push       edx
0045bf8a  push       0x100
0045bf8f  fadd       dword ptr [ebp + 0x444]
0045bf95  push       eax
0045bf96  fstp       dword ptr [esp + 0x5c]
0045bf9a  mov        ecx, dword ptr [eax]
0045bf9c  mov        eax, dword ptr [ecx + 0xb0]
0045bfa2  call       eax

// block 0045bfa4
0045bfa4  mov        ecx, dword ptr [ebp + 0x3f4]
0045bfaa  mov        eax, dword ptr [ecx + 8]
0045bfad  cmp        dword ptr [ebx + 0x4b563c], eax
0045bfb3  je         0x45bfd1

// block 0045bfb5
0045bfb5  mov        dword ptr [ebx + 0x4b563c], eax
0045bfbb  mov        eax, dword ptr [eax]
0045bfbd  mov        ecx, dword ptr [0x4ce8f0]
0045bfc3  mov        edx, dword ptr [ecx]
0045bfc5  push       eax
0045bfc6  push       0
0045bfc8  push       ecx
0045bfc9  mov        ecx, dword ptr [edx + 0x104]
0045bfcf  call       ecx

// block 0045bfd1
0045bfd1  mov        ecx, dword ptr [ebp + 0x3f4]
0045bfd7  cmp        dword ptr [ebx + 0x4b5648], ecx
0045bfdd  jne        0x45c00f

// block 0045bfdf
0045bfdf  fldz       
0045bfe1  fcom       dword ptr [ebp + 0x60]
0045bfe4  fnstsw     ax
0045bfe6  test       ah, 0x44
0045bfe9  jp         0x45c00d

// block 0045bfeb
0045bfeb  fcomp      dword ptr [ebp + 0x60]
0045bfee  fnstsw     ax
0045bff0  test       ah, 0x44
0045bff3  jp         0x45c00f

// block 0045bff5
0045bff5  fld1       
0045bff7  fcom       dword ptr [ebp + 0x50]
0045bffa  fnstsw     ax
0045bffc  test       ah, 0x44
0045bfff  jp         0x45c00d

// block 0045c001
0045c001  fcomp      dword ptr [ebp + 0x54]
0045c004  fnstsw     ax
0045c006  test       ah, 0x44
0045c009  jnp        0x45c06a

// block 0045c00b
0045c00b  jmp        0x45c00f

// block 0045c00d
0045c00d  fstp       st(0)

// block 0045c00f
0045c00f  mov        dword ptr [ebx + 0x4b5648], ecx
0045c015  fld        dword ptr [ebp + 0x7c]
0045c018  fadd       dword ptr [ebp + 0x60]
0045c01b  mov        eax, dword ptr [0x4ce8f0]
0045c020  lea        esi, [ebp + 0x37c]
0045c026  mov        ecx, 0x10
0045c02b  lea        edi, [esp + 0x58]

// block 0045c02f
0045c02f  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045c031
0045c031  fstp       dword ptr [esp + 0x78]
0045c035  fld        dword ptr [ebp + 0x80]
0045c03b  fadd       dword ptr [ebp + 0x64]
0045c03e  fstp       dword ptr [esp + 0x7c]
0045c042  fld        dword ptr [ebp + 0x50]
0045c045  fmul       dword ptr [esp + 0x58]
0045c049  fstp       dword ptr [esp + 0x58]
0045c04d  fld        dword ptr [ebp + 0x54]
0045c050  lea        ecx, [esp + 0x58]
0045c054  fmul       dword ptr [esp + 0x6c]
0045c058  push       ecx
0045c059  push       0x10
0045c05b  push       eax
0045c05c  fstp       dword ptr [esp + 0x78]
0045c060  mov        edx, dword ptr [eax]
0045c062  mov        edx, dword ptr [edx + 0xb0]
0045c068  call       edx

// block 0045c06a
0045c06a  cmp        byte ptr [ebx + 0x4b5642], 2
0045c071  je         0x45c0d8

// block 0045c073
0045c073  mov        edx, dword ptr [ebx + 0x4b564c]
0045c079  mov        eax, dword ptr [0x4ce8f0]
0045c07e  mov        ecx, dword ptr [eax]
0045c080  push       0x14
0045c082  push       0
0045c084  push       edx
0045c085  push       0
0045c087  push       eax
0045c088  mov        eax, dword ptr [ecx + 0x190]
0045c08e  call       eax

// block 0045c090
0045c090  mov        eax, dword ptr [0x4ce8f0]
0045c095  mov        ecx, dword ptr [eax]
0045c097  mov        edx, dword ptr [ecx + 0x164]
0045c09d  push       0x102
0045c0a2  push       eax
0045c0a3  call       edx

// block 0045c0a5
0045c0a5  mov        eax, dword ptr [0x4ce8f0]
0045c0aa  mov        ecx, dword ptr [eax]
0045c0ac  mov        edx, dword ptr [ecx + 0x10c]
0045c0b2  push       3
0045c0b4  push       6
0045c0b6  push       0
0045c0b8  push       eax
0045c0b9  call       edx

// block 0045c0bb
0045c0bb  mov        eax, dword ptr [0x4ce8f0]
0045c0c0  mov        ecx, dword ptr [eax]
0045c0c2  mov        edx, dword ptr [ecx + 0x10c]
0045c0c8  push       3
0045c0ca  push       3
0045c0cc  push       0
0045c0ce  push       eax
0045c0cf  call       edx

// block 0045c0d1
0045c0d1  mov        byte ptr [ebx + 0x4b5642], 2

// block 0045c0d8
0045c0d8  mov        eax, dword ptr [0x4ce8cc]
0045c0dd  cmp        byte ptr [eax + 0x4b5647], 1
0045c0e4  je         0x45c11e

// block 0045c0e6
0045c0e6  mov        eax, dword ptr [0x4ce8f0]
0045c0eb  mov        ecx, dword ptr [eax]
0045c0ed  mov        edx, dword ptr [ecx + 0x10c]
0045c0f3  push       4
0045c0f5  push       4
0045c0f7  push       0
0045c0f9  push       eax
0045c0fa  call       edx

// block 0045c0fc
0045c0fc  mov        eax, dword ptr [0x4ce8f0]
0045c101  mov        ecx, dword ptr [eax]
0045c103  mov        edx, dword ptr [ecx + 0x10c]
0045c109  push       4
0045c10b  push       1
0045c10d  push       0
0045c10f  push       eax
0045c110  call       edx

// block 0045c112
0045c112  mov        eax, dword ptr [0x4ce8cc]
0045c117  mov        byte ptr [eax + 0x4b5647], 1

// block 0045c11e
0045c11e  mov        eax, dword ptr [0x4ce8f0]
0045c123  mov        ecx, dword ptr [eax]
0045c125  mov        edx, dword ptr [ecx + 0x144]
0045c12b  push       2
0045c12d  push       0
0045c12f  push       5
0045c131  push       eax
0045c132  call       edx

// block 0045c134
0045c134  pop        edi
0045c135  pop        esi
0045c136  pop        ebp
0045c137  xor        eax, eax
0045c139  pop        ebx
0045c13a  add        esp, 0xcc
0045c140  ret        8
