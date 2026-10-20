
// block 0042bd90
0042bd90  push       ebp
0042bd91  mov        ebp, esp
0042bd93  and        esp, 0xfffffff8
0042bd96  sub        esp, 0x44
0042bd99  cmp        dword ptr [ebp + 0xc], 0
0042bd9d  push       ebx
0042bd9e  push       esi
0042bd9f  mov        edx, ecx
0042bda1  push       edi
0042bda2  mov        dword ptr [esp + 0x18], edx
0042bda6  je         0x42bdbc

// block 0042bda8
0042bda8  cmp        dword ptr [edx + 0x44c], 0
0042bdaf  je         0x42bdbc

// block 0042bdb1
0042bdb1  xor        eax, eax
0042bdb3  pop        edi
0042bdb4  pop        esi
0042bdb5  pop        ebx
0042bdb6  mov        esp, ebp
0042bdb8  pop        ebp
0042bdb9  ret        8

// block 0042bdbc
0042bdbc  fld        dword ptr [0x4a3e54]
0042bdc2  sub        esp, 8
0042bdc5  fst        dword ptr [esp + 0x18]
0042bdc9  lea        ecx, [esp + 0x4c]
0042bdcd  fstp       dword ptr [esp + 4]
0042bdd1  mov        dword ptr [esp + 0x1c], 0
0042bdd9  fld        dword ptr [edx + 0x68]
0042bddc  fstp       dword ptr [esp]
0042bddf  call       0x42e880

// block 0042bde4
0042bde4  fld        dword ptr [edx + 0x50]
0042bde7  fld        dword ptr [esp + 0x44]
0042bdeb  fld        st(0)
0042bded  faddp      st(2)
0042bdef  fxch       st(1)
0042bdf1  fstp       dword ptr [esp + 0x28]
0042bdf5  mov        eax, dword ptr [esp + 0x28]
0042bdf9  fld        dword ptr [edx + 0x54]
0042bdfc  mov        dword ptr [esp + 0x38], eax
0042be00  fld        dword ptr [esp + 0x48]
0042be04  fld        st(0)
0042be06  faddp      st(2)
0042be08  fxch       st(1)
0042be0a  fstp       dword ptr [esp + 0x2c]
0042be0e  mov        ecx, dword ptr [esp + 0x2c]
0042be12  fld        dword ptr [edx + 0x58]
0042be15  mov        dword ptr [esp + 0x3c], ecx
0042be19  fldz       
0042be1b  fadd       st(1), st(0)
0042be1d  fxch       st(1)
0042be1f  fstp       dword ptr [esp + 0x30]
0042be23  mov        eax, dword ptr [esp + 0x30]
0042be27  fld        st(2)
0042be29  mov        dword ptr [esp + 0x40], eax
0042be2d  faddp      st(3)
0042be2f  fxch       st(2)
0042be31  fstp       dword ptr [esp + 0x44]
0042be35  fadd       st(0), st(0)
0042be37  fstp       dword ptr [esp + 0x48]
0042be3b  fld        st(0)
0042be3d  fadd       st(1)
0042be3f  fstp       dword ptr [esp + 0x4c]
0042be43  fld        dword ptr [edx + 0x6c]
0042be46  fld        qword ptr [0x4a3d68]
0042be4c  fcom       st(1)
0042be4e  fnstsw     ax
0042be50  fstp       st(1)
0042be52  test       ah, 5
0042be55  jnp        0x42be73

// block 0042be57
0042be57  fstp       st(0)

// block 0042be59
0042be59  mov        eax, dword ptr [esp + 0x14]
0042be5d  fstp       st(0)
0042be5f  pop        edi
0042be60  pop        esi
0042be61  mov        dword ptr [edx + 0xc], 1
0042be68  pop        ebx
0042be69  mov        esp, ebp
0042be6b  pop        ebp
0042be6c  ret        8

// block 0042be6f
0042be6f  fldz       
0042be71  fxch       st(1)

// block 0042be73
0042be73  fld        dword ptr [esp + 0x38]
0042be77  mov        esi, 1
0042be7c  add        dword ptr [esp + 0x14], esi
0042be80  fld        st(0)
0042be82  fadd       st(2)
0042be84  fcomp      qword ptr [0x4a3d60]
0042be8a  fnstsw     ax
0042be8c  fld        dword ptr [esp + 0x3c]
0042be90  test       ah, 0x41
0042be93  jnp        0x42c022

// block 0042be99
0042be99  fxch       st(1)
0042be9b  fsub       st(2)
0042be9d  fcomp      qword ptr [0x4a3d28]
0042bea3  fnstsw     ax
0042bea5  test       ah, 1
0042bea8  je         0x42c024

// block 0042beae
0042beae  fld        st(0)
0042beb0  fadd       st(2)
0042beb2  fst        qword ptr [esp + 0x20]
0042beb6  fcomp      st(3)
0042beb8  fnstsw     ax
0042beba  fstp       st(2)
0042bebc  test       ah, 0x41
0042bebf  jnp        0x42c026

// block 0042bec5
0042bec5  fsub       st(1), st(0)
0042bec7  fxch       st(1)
0042bec9  fcomp      qword ptr [0x4a3d50]
0042becf  fnstsw     ax
0042bed1  test       ah, 1
0042bed4  je         0x42c028

// block 0042beda
0042beda  test       dword ptr [0x4cee78], 0x8000
0042bee4  fstp       st(0)
0042bee6  movsx      edi, word ptr [edx + 0x4a6]
0042beed  mov        ecx, dword ptr [0x4b44f4]
0042bef3  mov        ebx, dword ptr [ecx + 0x488]
0042bef9  lea        edi, [edi + edi + 4]
0042befd  je         0x42bf10

// block 0042beff
0042beff  push       0x4cf1d0
0042bf04  call       dword ptr [0x498088]

// block 0042bf0a
0042bf0a  inc        byte ptr [0x4cf221]

// block 0042bf10
0042bf10  add        dword ptr [ebx + 0x130], esi
0042bf16  call       0x4621c0

// block 0042bf1b
0042bf1b  fld        dword ptr [esp + 0x38]
0042bf1f  fadd       qword ptr [0x4a3d70]
0042bf25  mov        esi, eax
0042bf27  or         dword ptr [esi + 0x480], 1
0042bf2e  mov        dword ptr [esi + 0x20], 0x17
0042bf35  fst        qword ptr [esp + 0x28]
0042bf39  push       edi
0042bf3a  fadd       qword ptr [0x4a3d28]
0042bf40  push       esi
0042bf41  mov        ecx, ebx
0042bf43  fstp       dword ptr [esi + 0x430]
0042bf49  fld        qword ptr [esp + 0x28]
0042bf4d  fstp       dword ptr [esi + 0x434]
0042bf53  fld        dword ptr [esp + 0x48]
0042bf57  fstp       dword ptr [esi + 0x438]
0042bf5d  call       0x454d10

// block 0042bf62
0042bf62  mov        ebx, esi
0042bf64  lea        eax, [esp + 0x1c]
0042bf68  call       0x461250

// block 0042bf6d
0042bf6d  test       dword ptr [0x4cee78], 0x8000
0042bf77  je         0x42bf8a

// block 0042bf79
0042bf79  push       0x4cf1d0
0042bf7e  call       dword ptr [0x49808c]

// block 0042bf84
0042bf84  dec        byte ptr [0x4cf221]

// block 0042bf8a
0042bf8a  cmp        dword ptr [ebp + 8], 0
0042bf8e  je         0x42c016

// block 0042bf94
0042bf94  fld        qword ptr [0x4a3d60]
0042bf9a  fcomp      qword ptr [esp + 0x28]
0042bf9e  fnstsw     ax
0042bfa0  test       ah, 1
0042bfa3  je         0x42c016

// block 0042bfa5
0042bfa5  fld        dword ptr [esp + 0x38]
0042bfa9  fld        qword ptr [0x4a3d70]
0042bfaf  fsub       st(1), st(0)
0042bfb1  fxch       st(1)
0042bfb3  fcomp      qword ptr [0x4a3d28]
0042bfb9  fnstsw     ax
0042bfbb  test       ah, 1
0042bfbe  je         0x42c014

// block 0042bfc0
0042bfc0  fld        dword ptr [esp + 0x3c]
0042bfc4  fld        st(0)
0042bfc6  fadd       st(2)
0042bfc8  fcomp      qword ptr [0x4a3d58]
0042bfce  fnstsw     ax
0042bfd0  test       ah, 0x41
0042bfd3  jnp        0x42c012

// block 0042bfd5
0042bfd5  fsubrp     st(1)
0042bfd7  fcomp      qword ptr [0x4a3d50]
0042bfdd  fnstsw     ax
0042bfdf  test       ah, 1
0042bfe2  je         0x42c016

// block 0042bfe4
0042bfe4  fld        dword ptr [0x4a41f4]
0042bfea  sub        esp, 8
0042bfed  fstp       dword ptr [esp + 4]
0042bff1  lea        edx, [esp + 0x40]
0042bff5  fld        dword ptr [0x4a4060]
0042bffb  fstp       dword ptr [esp]
0042bffe  push       edx
0042bfff  push       9
0042c001  call       0x4273f0

// block 0042c006
0042c006  fld        qword ptr [0x4a3d68]
0042c00c  mov        edx, dword ptr [esp + 0x18]
0042c010  jmp        0x42c028

// block 0042c012
0042c012  fstp       st(1)

// block 0042c014
0042c014  fstp       st(0)

// block 0042c016
0042c016  fld        qword ptr [0x4a3d68]
0042c01c  mov        edx, dword ptr [esp + 0x18]
0042c020  jmp        0x42c028

// block 0042c022
0042c022  fstp       st(1)

// block 0042c024
0042c024  fstp       st(2)

// block 0042c026
0042c026  fstp       st(1)

// block 0042c028
0042c028  fld        dword ptr [esp + 0x38]
0042c02c  fadd       dword ptr [esp + 0x44]
0042c030  fstp       dword ptr [esp + 0x38]
0042c034  fld        dword ptr [esp + 0x3c]
0042c038  fadd       dword ptr [esp + 0x48]
0042c03c  fstp       dword ptr [esp + 0x3c]
0042c040  fld        dword ptr [esp + 0x40]
0042c044  fadd       dword ptr [esp + 0x4c]
0042c048  fstp       dword ptr [esp + 0x40]
0042c04c  fld        dword ptr [esp + 0x10]
0042c050  fadd       st(1)
0042c052  fstp       dword ptr [esp + 0x10]
0042c056  fld        dword ptr [esp + 0x10]
0042c05a  fadd       qword ptr [0x4a4078]
0042c060  fld        dword ptr [edx + 0x6c]
0042c063  fcompp     
0042c065  fnstsw     ax
0042c067  test       ah, 0x41
0042c06a  je         0x42be6f

// block 0042c070
0042c070  jmp        0x42be59
