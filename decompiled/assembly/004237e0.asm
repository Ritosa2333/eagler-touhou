
// block 004237e0
004237e0  sub        esp, 0x48
004237e3  push       ebx
004237e4  push       ebp
004237e5  push       esi
004237e6  push       edi
004237e7  test       eax, eax
004237e9  je         0x423ebc

// block 004237ef
004237ef  nop        

// block 004237f0
004237f0  mov        ecx, dword ptr [eax + 4]
004237f3  mov        ebp, dword ptr [eax]
004237f5  mov        eax, dword ptr [ebp + 0x68]
004237f8  mov        dword ptr [esp + 0x18], ecx
004237fc  test       eax, eax
004237fe  je         0x423808

// block 00423800
00423800  cmp        eax, dword ptr [0x4b0cb8]
00423806  jne        0x42380b

// block 00423808
00423808  dec        dword ptr [ebp + 0x60]

// block 0042380b
0042380b  cmp        dword ptr [ebp + 0x60], 0
0042380f  jg         0x423eb2

// block 00423815
00423815  mov        edi, dword ptr [esp + 0x5c]
00423819  mov        eax, dword ptr [edi + 0x1a4]
0042381f  mov        ecx, dword ptr [edi + eax*4 + 0xdc]
00423826  lea        esi, [edi + eax*4 + 0xdc]
0042382d  test       ecx, ecx
0042382f  je         0x42389f

// block 00423831
00423831  mov        edx, dword ptr [0x4ce8cc]
00423837  mov        eax, dword ptr [edx + 0x8856b8]
0042383d  test       eax, eax
0042383f  je         0x42384e

// block 00423841
00423841  mov        ebx, dword ptr [eax]
00423843  cmp        dword ptr [ebx], ecx
00423845  je         0x423867

// block 00423847
00423847  mov        eax, dword ptr [eax + 4]
0042384a  test       eax, eax
0042384c  jne        0x423841

// block 0042384e
0042384e  mov        eax, dword ptr [edx + 0x8856c0]
00423854  test       eax, eax
00423856  je         0x42389f

// block 00423858
00423858  mov        edx, dword ptr [eax]
0042385a  cmp        dword ptr [edx], ecx
0042385c  je         0x42386b

// block 0042385e
0042385e  mov        eax, dword ptr [eax + 4]
00423861  test       eax, eax
00423863  jne        0x423858

// block 00423865
00423865  jmp        0x42389f

// block 00423867
00423867  mov        eax, ebx
00423869  jmp        0x42386d

// block 0042386b
0042386b  mov        eax, edx

// block 0042386d
0042386d  test       eax, eax
0042386f  je         0x42389f

// block 00423871
00423871  mov        edx, 0x10000000
00423876  or         dword ptr [eax + 0x47c], edx
0042387c  cmp        dword ptr [eax + 0x18], 0
00423880  jne        0x42389f

// block 00423882
00423882  mov        eax, dword ptr [eax + 0x14]
00423885  test       eax, eax
00423887  je         0x42389f

// block 00423889
00423889  lea        esp, [esp]

// block 00423890
00423890  mov        ecx, dword ptr [eax]
00423892  or         dword ptr [ecx + 0x47c], edx
00423898  mov        eax, dword ptr [eax + 4]
0042389b  test       eax, eax
0042389d  jne        0x423890

// block 0042389f
0042389f  mov        dword ptr [esi], 0
004238a5  fld        dword ptr [ebp + 0x7c]
004238a8  fmul       qword ptr [0x4a4078]
004238ae  fadd       dword ptr [ebp + 4]
004238b1  fstp       dword ptr [ebp + 4]
004238b4  mov        esi, dword ptr [0x4cee70]
004238ba  fld1       
004238bc  fcomp      dword ptr [ebp + 0x7c]
004238bf  fnstsw     ax
004238c1  test       ah, 1
004238c4  jne        0x423a22

// block 004238ca
004238ca  mov        eax, dword ptr [edi + 0x1a4]
004238d0  add        eax, 4
004238d3  test       dword ptr [0x4cee78], 0x8000
004238dd  mov        dword ptr [esp + 0x10], eax
004238e1  je         0x4238f4

// block 004238e3
004238e3  push       0x4cf1d0
004238e8  call       dword ptr [0x498088]

// block 004238ee
004238ee  inc        byte ptr [0x4cf221]

// block 004238f4
004238f4  inc        dword ptr [esi + 0x130]
004238fa  call       0x4621c0

// block 004238ff
004238ff  mov        ebx, eax
00423901  or         dword ptr [ebx + 0x480], 1
00423908  mov        eax, dword ptr [esp + 0x10]
0042390c  mov        dword ptr [ebx + 0x20], 0x17
00423913  fld        dword ptr [ebp]
00423916  fadd       qword ptr [0x4a3d70]
0042391c  push       eax
0042391d  push       ebx
0042391e  mov        ecx, esi
00423920  fadd       qword ptr [0x4a3d28]
00423926  fstp       dword ptr [ebx + 0x430]
0042392c  fld        dword ptr [ebp + 4]
0042392f  fadd       qword ptr [0x4a3d68]
00423935  fstp       dword ptr [ebx + 0x434]
0042393b  fld        dword ptr [ebp + 8]
0042393e  fstp       dword ptr [ebx + 0x438]
00423944  call       0x454d10

// block 00423949
00423949  lea        eax, [esp + 0x1c]
0042394d  call       0x461250

// block 00423952
00423952  test       dword ptr [0x4cee78], 0x8000
0042395c  mov        esi, dword ptr [eax]
0042395e  je         0x423971

// block 00423960
00423960  push       0x4cf1d0
00423965  call       dword ptr [0x49808c]

// block 0042396b
0042396b  dec        byte ptr [0x4cf221]

// block 00423971
00423971  mov        ecx, dword ptr [edi + 0x1a4]
00423977  mov        dword ptr [edi + ecx*4 + 0xdc], esi
0042397e  mov        edx, dword ptr [edi + 0x1a4]
00423984  mov        ecx, dword ptr [edi + edx*4 + 0xdc]
0042398b  lea        esi, [edi + edx*4 + 0xdc]
00423992  test       ecx, ecx
00423994  jne        0x42399a

// block 00423996
00423996  xor        eax, eax
00423998  jmp        0x4239e0

// block 0042399a
0042399a  mov        edx, dword ptr [0x4ce8cc]
004239a0  mov        eax, dword ptr [edx + 0x8856b8]
004239a6  test       eax, eax
004239a8  je         0x4239bd

// block 004239aa
004239aa  lea        ebx, [ebx]

// block 004239b0
004239b0  mov        ebx, dword ptr [eax]
004239b2  cmp        dword ptr [ebx], ecx
004239b4  je         0x4239d6

// block 004239b6
004239b6  mov        eax, dword ptr [eax + 4]
004239b9  test       eax, eax
004239bb  jne        0x4239b0

// block 004239bd
004239bd  mov        eax, dword ptr [edx + 0x8856c0]
004239c3  test       eax, eax
004239c5  je         0x423996

// block 004239c7
004239c7  mov        edx, dword ptr [eax]
004239c9  cmp        dword ptr [edx], ecx
004239cb  je         0x4239da

// block 004239cd
004239cd  mov        eax, dword ptr [eax + 4]
004239d0  test       eax, eax
004239d2  jne        0x4239c7

// block 004239d4
004239d4  jmp        0x4239e0

// block 004239d6
004239d6  mov        eax, ebx
004239d8  jmp        0x4239dc

// block 004239da
004239da  mov        eax, edx

// block 004239dc
004239dc  test       eax, eax
004239de  jne        0x4239e6

// block 004239e0
004239e0  mov        dword ptr [esi], 0

// block 004239e6
004239e6  mov        cl, 0xf
004239e8  mov        byte ptr [eax + 0x49d], cl
004239ee  mov        byte ptr [eax + 0x49c], cl
004239f4  fld        dword ptr [ebp + 0x7c]
004239f7  fmul       qword ptr [0x4a4220]
004239fd  lea        ecx, [esp + 0x28]
00423a01  lea        edx, [esp + 0x30]
00423a05  fstp       dword ptr [esp + 0x30]
00423a09  fldz       
00423a0b  fstp       dword ptr [esp + 0x34]
00423a0f  fld        dword ptr [ebp + 0x7c]
00423a12  fstp       dword ptr [esp + 0x28]
00423a16  fld        dword ptr [ebp + 0x7c]
00423a19  fstp       dword ptr [esp + 0x2c]
00423a1d  jmp        0x423c98

// block 00423a22
00423a22  fld        dword ptr [0x4a4014]
00423a28  fcomp      dword ptr [ebp + 0x7c]
00423a2b  fnstsw     ax
00423a2d  test       ah, 1
00423a30  mov        eax, dword ptr [edi + 0x1a4]
00423a36  jne        0x423b83

// block 00423a3c
00423a3c  add        eax, 0xe
00423a3f  test       dword ptr [0x4cee78], 0x8000
00423a49  mov        dword ptr [esp + 0x10], eax
00423a4d  je         0x423a60

// block 00423a4f
00423a4f  push       0x4cf1d0
00423a54  call       dword ptr [0x498088]

// block 00423a5a
00423a5a  inc        byte ptr [0x4cf221]

// block 00423a60
00423a60  inc        dword ptr [esi + 0x130]
00423a66  call       0x4621c0

// block 00423a6b
00423a6b  mov        ebx, eax
00423a6d  or         dword ptr [ebx + 0x480], 1
00423a74  mov        eax, dword ptr [esp + 0x10]
00423a78  mov        dword ptr [ebx + 0x20], 0x17
00423a7f  fld        dword ptr [ebp]
00423a82  fadd       qword ptr [0x4a3d70]
00423a88  push       eax
00423a89  push       ebx
00423a8a  mov        ecx, esi
00423a8c  fadd       qword ptr [0x4a3d28]
00423a92  fstp       dword ptr [ebx + 0x430]
00423a98  fld        dword ptr [ebp + 4]
00423a9b  fadd       qword ptr [0x4a3d68]
00423aa1  fstp       dword ptr [ebx + 0x434]
00423aa7  fld        dword ptr [ebp + 8]
00423aaa  fstp       dword ptr [ebx + 0x438]
00423ab0  call       0x454d10

// block 00423ab5
00423ab5  lea        eax, [esp + 0x20]
00423ab9  call       0x461250

// block 00423abe
00423abe  test       dword ptr [0x4cee78], 0x8000
00423ac8  mov        esi, dword ptr [eax]
00423aca  je         0x423add

// block 00423acc
00423acc  push       0x4cf1d0
00423ad1  call       dword ptr [0x49808c]

// block 00423ad7
00423ad7  dec        byte ptr [0x4cf221]

// block 00423add
00423add  mov        ecx, dword ptr [edi + 0x1a4]
00423ae3  mov        dword ptr [edi + ecx*4 + 0xdc], esi
00423aea  mov        edx, dword ptr [edi + 0x1a4]
00423af0  mov        eax, dword ptr [edi + edx*4 + 0xdc]
00423af7  lea        ebx, [edi + edx*4 + 0xdc]
00423afe  mov        edx, dword ptr [0x4ce8cc]
00423b04  push       eax
00423b05  call       0x461920

// block 00423b0a
00423b0a  mov        esi, eax
00423b0c  test       esi, esi
00423b0e  jne        0x423b12

// block 00423b10
00423b10  mov        dword ptr [ebx], eax

// block 00423b12
00423b12  fld        dword ptr [ebp + 0x7c]
00423b15  lea        ecx, [esp + 0x38]
00423b19  fmul       qword ptr [0x4a3ef8]
00423b1f  lea        edx, [esp + 0x40]
00423b23  fnstcw     word ptr [esp + 0x10]
00423b27  movzx      eax, word ptr [esp + 0x10]
00423b2c  or         eax, 0xc00
00423b31  mov        dword ptr [esp + 0x14], eax
00423b35  fldcw      word ptr [esp + 0x14]
00423b39  fistp      dword ptr [esp + 0x14]
00423b3d  mov        al, byte ptr [esp + 0x14]
00423b41  mov        byte ptr [esi + 0x49d], al
00423b47  mov        byte ptr [esi + 0x49c], al
00423b4d  fldcw      word ptr [esp + 0x10]
00423b51  mov        eax, esi
00423b53  fld        dword ptr [ebp + 0x7c]
00423b56  fmul       qword ptr [0x4a3cf8]
00423b5c  fld        qword ptr [0x4a4220]
00423b62  fmul       st(1)
00423b64  fstp       dword ptr [esp + 0x40]
00423b68  fldz       
00423b6a  fstp       dword ptr [esp + 0x44]
00423b6e  fstp       dword ptr [esp + 0x10]
00423b72  fld        dword ptr [esp + 0x10]
00423b76  fst        dword ptr [esp + 0x38]
00423b7a  fstp       dword ptr [esp + 0x3c]
00423b7e  jmp        0x423c9a

// block 00423b83
00423b83  add        eax, 0xe
00423b86  test       dword ptr [0x4cee78], 0x8000
00423b90  mov        dword ptr [esp + 0x14], eax
00423b94  je         0x423ba7

// block 00423b96
00423b96  push       0x4cf1d0
00423b9b  call       dword ptr [0x498088]

// block 00423ba1
00423ba1  inc        byte ptr [0x4cf221]

// block 00423ba7
00423ba7  inc        dword ptr [esi + 0x130]
00423bad  call       0x4621c0

// block 00423bb2
00423bb2  mov        ecx, dword ptr [esp + 0x14]
00423bb6  mov        ebx, eax
00423bb8  or         dword ptr [ebx + 0x480], 1
00423bbf  mov        dword ptr [ebx + 0x20], 0x17
00423bc6  fld        dword ptr [ebp]
00423bc9  fadd       qword ptr [0x4a3d70]
00423bcf  push       ecx
00423bd0  push       ebx
00423bd1  mov        ecx, esi
00423bd3  fadd       qword ptr [0x4a3d28]
00423bd9  fstp       dword ptr [ebx + 0x430]
00423bdf  fld        dword ptr [ebp + 4]
00423be2  fadd       qword ptr [0x4a3d68]
00423be8  fstp       dword ptr [ebx + 0x434]
00423bee  fld        dword ptr [ebp + 8]
00423bf1  fstp       dword ptr [ebx + 0x438]
00423bf7  call       0x454d10

// block 00423bfc
00423bfc  lea        eax, [esp + 0x24]
00423c00  call       0x461250

// block 00423c05
00423c05  test       dword ptr [0x4cee78], 0x8000
00423c0f  mov        esi, dword ptr [eax]
00423c11  je         0x423c24

// block 00423c13
00423c13  push       0x4cf1d0
00423c18  call       dword ptr [0x49808c]

// block 00423c1e
00423c1e  dec        byte ptr [0x4cf221]

// block 00423c24
00423c24  mov        edx, dword ptr [edi + 0x1a4]
00423c2a  mov        dword ptr [edi + edx*4 + 0xdc], esi
00423c31  mov        eax, dword ptr [edi + 0x1a4]
00423c37  mov        ecx, dword ptr [edi + eax*4 + 0xdc]
00423c3e  mov        edx, dword ptr [0x4ce8cc]
00423c44  lea        esi, [edi + eax*4 + 0xdc]
00423c4b  push       ecx
00423c4c  call       0x461920

// block 00423c51
00423c51  test       eax, eax
00423c53  jne        0x423c57

// block 00423c55
00423c55  mov        dword ptr [esi], eax

// block 00423c57
00423c57  mov        cl, 0x1e
00423c59  mov        byte ptr [eax + 0x49d], cl
00423c5f  mov        byte ptr [eax + 0x49c], cl
00423c65  fld        dword ptr [ebp + 0x7c]
00423c68  fmul       qword ptr [0x4a3cf8]
00423c6e  lea        ecx, [esp + 0x48]
00423c72  fld        qword ptr [0x4a4220]
00423c78  lea        edx, [esp + 0x50]
00423c7c  fmul       st(1)
00423c7e  fstp       dword ptr [esp + 0x50]
00423c82  fldz       
00423c84  fstp       dword ptr [esp + 0x54]
00423c88  fstp       dword ptr [esp + 0x10]
00423c8c  fld        dword ptr [esp + 0x10]
00423c90  fst        dword ptr [esp + 0x48]
00423c94  fstp       dword ptr [esp + 0x4c]

// block 00423c98
00423c98  mov        esi, eax

// block 00423c9a
00423c9a  push       4
00423c9c  push       8
00423c9e  call       0x425790

// block 00423ca3
00423ca3  mov        eax, dword ptr [ebp + 0x64]
00423ca6  xor        ebx, ebx
00423ca8  sub        eax, ebx
00423caa  je         0x423df2

// block 00423cb0
00423cb0  sub        eax, 1
00423cb3  je         0x423d5c

// block 00423cb9
00423cb9  sub        eax, 1
00423cbc  jne        0x423e48

// block 00423cc2
00423cc2  mov        eax, dword ptr [edi + 0x1a4]
00423cc8  mov        ecx, dword ptr [esi + 0x430]
00423cce  lea        edx, [eax + eax*2]
00423cd1  mov        dword ptr [edi + edx*4 + 0x104], ecx
00423cd8  lea        eax, [edi + edx*4 + 0x104]
00423cdf  mov        edx, dword ptr [esi + 0x434]
00423ce5  mov        dword ptr [eax + 4], edx
00423ce8  mov        ecx, dword ptr [esi + 0x438]
00423cee  mov        dword ptr [eax + 8], ecx
00423cf1  mov        eax, dword ptr [edi + 0x1a4]
00423cf7  lea        edx, [eax + eax*2]
00423cfa  fld        dword ptr [edi + edx*4 + 0x104]
00423d01  lea        eax, [edi + edx*4 + 0x104]
00423d08  fld        dword ptr [ebp + 0x80]
00423d0e  lea        ecx, [ebp + 0x1c]
00423d11  fmul       qword ptr [0x4a3cf8]
00423d17  push       ecx
00423d18  push       ebx
00423d19  push       ebx
00423d1a  fsubp      st(1)
00423d1c  push       ebx
00423d1d  push       0xffffff
00423d22  fstp       dword ptr [eax]
00423d24  mov        eax, dword ptr [edi + 0x1a4]
00423d2a  fld        dword ptr [ebp + 0x80]
00423d30  fstp       dword ptr [edi + eax*4 + 0x17c]
00423d37  call       0x460800

// block 00423d3c
00423d3c  mov        edx, dword ptr [esi + 0x47c]
00423d42  and        edx, 0xfff7ffff
00423d48  add        esp, 0x14
00423d4b  or         edx, 0x100000
00423d51  mov        dword ptr [esi + 0x47c], edx
00423d57  jmp        0x423e48

// block 00423d5c
00423d5c  mov        eax, dword ptr [edi + 0x1a4]
00423d62  mov        ecx, dword ptr [esi + 0x430]
00423d68  lea        eax, [eax + eax*2]
00423d6b  mov        dword ptr [edi + eax*4 + 0x104], ecx
00423d72  mov        edx, dword ptr [esi + 0x434]
00423d78  lea        eax, [edi + eax*4 + 0x104]
00423d7f  mov        dword ptr [eax + 4], edx
00423d82  mov        ecx, dword ptr [esi + 0x438]
00423d88  mov        dword ptr [eax + 8], ecx
00423d8b  fld        dword ptr [ebp + 0x80]
00423d91  mov        eax, dword ptr [edi + 0x1a4]
00423d97  fmul       qword ptr [0x4a3cf8]
00423d9d  lea        edx, [eax + eax*2]
00423da0  lea        eax, [edi + edx*4 + 0x104]
00423da7  fadd       dword ptr [eax]
00423da9  lea        ecx, [ebp + 0x1c]
00423dac  push       ecx
00423dad  push       ebx
00423dae  fstp       dword ptr [eax]
00423db0  mov        eax, dword ptr [edi + 0x1a4]
00423db6  fld        dword ptr [ebp + 0x80]
00423dbc  push       ebx
00423dbd  fstp       dword ptr [edi + eax*4 + 0x17c]
00423dc4  push       ebx
00423dc5  push       0xffffff
00423dca  xor        edi, edi
00423dcc  call       0x460760

// block 00423dd1
00423dd1  mov        edx, dword ptr [esi + 0x47c]
00423dd7  mov        edi, dword ptr [esp + 0x70]
00423ddb  and        edx, 0xffefffff
00423de1  add        esp, 0x14
00423de4  or         edx, 0x80000
00423dea  mov        dword ptr [esi + 0x47c], edx
00423df0  jmp        0x423e48

// block 00423df2
00423df2  mov        eax, dword ptr [edi + 0x1a4]
00423df8  mov        ecx, dword ptr [esi + 0x430]
00423dfe  lea        eax, [eax + eax*2]
00423e01  mov        dword ptr [edi + eax*4 + 0x104], ecx
00423e08  mov        edx, dword ptr [esi + 0x434]
00423e0e  lea        eax, [edi + eax*4 + 0x104]
00423e15  mov        dword ptr [eax + 4], edx
00423e18  mov        ecx, dword ptr [esi + 0x438]
00423e1e  mov        dword ptr [eax + 8], ecx
00423e21  fld        dword ptr [ebp + 0x80]
00423e27  mov        edx, dword ptr [edi + 0x1a4]
00423e2d  lea        eax, [ebp + 0x1c]
00423e30  fstp       dword ptr [edi + edx*4 + 0x17c]
00423e37  push       eax
00423e38  push       ebx
00423e39  push       ebx
00423e3a  push       ebx
00423e3b  push       0xffffff
00423e40  call       0x4608f0

// block 00423e45
00423e45  add        esp, 0x14

// block 00423e48
00423e48  mov        ecx, dword ptr [ebp + 0x84]
00423e4e  mov        dword ptr [esi + 0x3bc], ecx
00423e54  mov        byte ptr [esi + 0x3bf], bl
00423e5a  mov        edx, dword ptr [ebp + 0x6c]
00423e5d  mov        dword ptr [esi + 0x3fc], edx
00423e63  movzx      eax, byte ptr [ebp + 0x87]
00423e6a  mov        dword ptr [esi + 0x400], eax
00423e70  mov        eax, dword ptr [edi + 0x1a4]
00423e76  inc        eax
00423e77  cdq        
00423e78  mov        ecx, 0xa
00423e7d  idiv       ecx
00423e7f  mov        dword ptr [edi + 0x1a4], edx
00423e85  mov        eax, dword ptr [ebp + 0x10]
00423e88  cmp        eax, ebx
00423e8a  je         0x423e92

// block 00423e8c
00423e8c  mov        edx, dword ptr [ebp + 0x14]
00423e8f  mov        dword ptr [eax + 8], edx

// block 00423e92
00423e92  mov        eax, dword ptr [ebp + 0x14]
00423e95  cmp        eax, ebx
00423e97  je         0x423e9f

// block 00423e99
00423e99  mov        ecx, dword ptr [ebp + 0x10]
00423e9c  mov        dword ptr [eax + 4], ecx

// block 00423e9f
00423e9f  push       ebp
00423ea0  mov        dword ptr [ebp + 0x10], ebx
00423ea3  mov        dword ptr [ebp + 0x14], ebx
00423ea6  call       0x46ca4f

// block 00423eab
00423eab  mov        ecx, dword ptr [esp + 0x1c]
00423eaf  add        esp, 4

// block 00423eb2
00423eb2  mov        eax, ecx
00423eb4  test       ecx, ecx
00423eb6  jne        0x4237f0

// block 00423ebc
00423ebc  pop        edi
00423ebd  pop        esi
00423ebe  pop        ebp
00423ebf  xor        eax, eax
00423ec1  pop        ebx
00423ec2  add        esp, 0x48
00423ec5  ret        4
