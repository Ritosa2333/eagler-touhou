
// block 0041c020
0041c020  sub        esp, 0x28
0041c023  mov        eax, dword ptr [edi + 0x34]
0041c026  push       esi
0041c027  mov        dword ptr [esp + 4], eax
0041c02b  test       eax, eax
0041c02d  jle        0x41c0aa

// block 0041c02f
0041c02f  lea        esi, [edi + 0x20]
0041c032  call       0x464a80

// block 0041c037
0041c037  mov        ecx, dword ptr [edi + 0x34]
0041c03a  cmp        dword ptr [edi + 0x24], ecx
0041c03d  mov        dword ptr [esp + 4], ecx
0041c041  jl         0x41c0aa

// block 0041c043
0041c043  mov        eax, dword ptr [esi + 0x10]
0041c046  mov        dword ptr [esp + 4], ecx
0041c04a  test       al, 1
0041c04c  jne        0x41c06d

// block 0041c04e
0041c04e  fldz       
0041c050  or         eax, 1
0041c053  fstp       dword ptr [esi + 8]
0041c056  mov        dword ptr [esi + 4], 0
0041c05d  mov        dword ptr [esi], 0xfff0bdc1
0041c063  mov        dword ptr [esi + 0xc], 0x4b2ed0
0041c06a  mov        dword ptr [esi + 0x10], eax

// block 0041c06d
0041c06d  fild       dword ptr [esp + 4]
0041c071  mov        dword ptr [esi + 4], ecx
0041c074  dec        ecx
0041c075  mov        dword ptr [esi], ecx
0041c077  fstp       dword ptr [esi + 8]
0041c07a  cmp        dword ptr [edi + 0x38], 7
0041c07e  mov        dword ptr [edi + 0x34], 0
0041c085  je         0x41c099

// block 0041c087
0041c087  mov        eax, dword ptr [edi + 8]
0041c08a  mov        ecx, dword ptr [edi + 0xc]
0041c08d  mov        dword ptr [ebx], eax
0041c08f  mov        dword ptr [ebx + 4], ecx
0041c092  mov        eax, ebx
0041c094  pop        esi
0041c095  add        esp, 0x28
0041c098  ret        

// block 0041c099
0041c099  mov        eax, dword ptr [edi + 4]
0041c09c  mov        edx, dword ptr [edi]
0041c09e  mov        dword ptr [ebx + 4], eax
0041c0a1  mov        dword ptr [ebx], edx
0041c0a3  mov        eax, ebx
0041c0a5  pop        esi
0041c0a6  add        esp, 0x28
0041c0a9  ret        

// block 0041c0aa
0041c0aa  mov        eax, dword ptr [edi + 0x38]
0041c0ad  cmp        eax, 7
0041c0b0  jne        0x41c0ee

// block 0041c0b2
0041c0b2  mov        ecx, dword ptr [edi]
0041c0b4  fld        dword ptr [edi + 8]
0041c0b7  mov        edx, dword ptr [edi + 4]
0041c0ba  mov        dword ptr [esp + 0x1c], ecx
0041c0be  fadd       dword ptr [esp + 0x1c]
0041c0c2  mov        dword ptr [esp + 0x20], edx
0041c0c6  fstp       dword ptr [esp + 0x14]
0041c0ca  mov        eax, dword ptr [esp + 0x14]
0041c0ce  fld        dword ptr [edi + 0xc]
0041c0d1  mov        dword ptr [edi], eax
0041c0d3  fadd       dword ptr [esp + 0x20]
0041c0d7  mov        dword ptr [ebx], eax
0041c0d9  mov        eax, ebx
0041c0db  fstp       dword ptr [esp + 0x18]
0041c0df  mov        ecx, dword ptr [esp + 0x18]
0041c0e3  mov        dword ptr [edi + 4], ecx
0041c0e6  mov        dword ptr [ebx + 4], ecx
0041c0e9  pop        esi
0041c0ea  add        esp, 0x28
0041c0ed  ret        

// block 0041c0ee
0041c0ee  cmp        eax, 0x11
0041c0f1  jne        0x41c151

// block 0041c0f3
0041c0f3  fld        dword ptr [edi + 0x18]
0041c0f6  mov        eax, dword ptr [edi]
0041c0f8  mov        ecx, dword ptr [edi + 4]
0041c0fb  mov        dword ptr [esp + 0x1c], eax
0041c0ff  fadd       dword ptr [esp + 0x1c]
0041c103  mov        dword ptr [esp + 0x20], ecx
0041c107  fstp       dword ptr [esp + 0x14]
0041c10b  mov        eax, dword ptr [esp + 0x14]
0041c10f  fld        dword ptr [edi + 0x1c]
0041c112  mov        dword ptr [edi], eax
0041c114  fadd       dword ptr [esp + 0x20]
0041c118  mov        dword ptr [ebx], eax
0041c11a  mov        eax, ebx
0041c11c  fstp       dword ptr [esp + 0x18]
0041c120  mov        ecx, dword ptr [esp + 0x18]
0041c124  mov        dword ptr [edi + 4], ecx
0041c127  fld        dword ptr [edi + 0x18]
0041c12a  fadd       dword ptr [edi + 8]
0041c12d  mov        dword ptr [ebx + 4], ecx
0041c130  fstp       dword ptr [esp + 0x1c]
0041c134  mov        edx, dword ptr [esp + 0x1c]
0041c138  fld        dword ptr [edi + 0x1c]
0041c13b  fadd       dword ptr [edi + 0xc]
0041c13e  mov        dword ptr [edi + 0x18], edx
0041c141  fstp       dword ptr [esp + 0x20]
0041c145  mov        edx, dword ptr [esp + 0x20]
0041c149  mov        dword ptr [edi + 0x1c], edx
0041c14c  pop        esi
0041c14d  add        esp, 0x28
0041c150  ret        

// block 0041c151
0041c151  cmp        eax, 8
0041c154  jne        0x41c260

// block 0041c15a
0041c15a  fld        dword ptr [edi + 0x28]
0041c15d  fidiv      dword ptr [esp + 4]
0041c161  fstp       dword ptr [esp + 4]
0041c165  fld        dword ptr [esp + 4]
0041c169  fld        st(0)
0041c16b  fld1       
0041c16d  fsub       st(1), st(0)
0041c16f  fld        st(1)
0041c171  fld        st(3)
0041c173  fadd       st(0), st(0)
0041c175  fld        st(1)
0041c177  fmulp      st(2)
0041c179  fld        st(0)
0041c17b  fadd       st(3)
0041c17d  fmulp      st(2)
0041c17f  fxch       st(1)
0041c181  fstp       dword ptr [esp + 0x1c]
0041c185  fld        st(3)
0041c187  fmul       st(0), st(0)
0041c189  fld        qword ptr [0x4a3fd8]
0041c18f  fsubrp     st(2)
0041c191  fmulp      st(1)
0041c193  fstp       dword ptr [esp + 0x14]
0041c197  fsub       st(2)
0041c199  fmul       st(0), st(0)
0041c19b  fmul       st(2)
0041c19d  fstp       dword ptr [esp + 0xc]
0041c1a1  fmul       st(1)
0041c1a3  fmulp      st(1)
0041c1a5  fstp       dword ptr [esp + 4]
0041c1a9  fld        dword ptr [esp + 4]
0041c1ad  fld        st(0)
0041c1af  fmul       dword ptr [edi + 0x18]
0041c1b2  fstp       dword ptr [esp + 0x24]
0041c1b6  fmul       dword ptr [edi + 0x1c]
0041c1b9  fstp       dword ptr [esp + 0x28]
0041c1bd  fld        dword ptr [edi + 0x10]
0041c1c0  fld        dword ptr [esp + 0xc]
0041c1c4  fld        st(0)
0041c1c6  fmulp      st(2)
0041c1c8  fxch       st(1)
0041c1ca  fstp       dword ptr [esp + 4]
0041c1ce  fmul       dword ptr [edi + 0x14]
0041c1d1  fstp       dword ptr [esp + 8]
0041c1d5  fld        dword ptr [edi + 8]
0041c1d8  fld        dword ptr [esp + 0x14]
0041c1dc  fld        st(0)
0041c1de  fmulp      st(2)
0041c1e0  fxch       st(1)
0041c1e2  fstp       dword ptr [esp + 0x14]
0041c1e6  fmul       dword ptr [edi + 0xc]
0041c1e9  fstp       dword ptr [esp + 0x18]
0041c1ed  fld        dword ptr [edi]
0041c1ef  fld        dword ptr [esp + 0x1c]
0041c1f3  fld        st(0)
0041c1f5  fmulp      st(2)
0041c1f7  fxch       st(1)
0041c1f9  fstp       dword ptr [esp + 0x1c]
0041c1fd  fmul       dword ptr [edi + 4]
0041c200  fstp       dword ptr [esp + 0x20]
0041c204  fld        dword ptr [esp + 0x1c]
0041c208  fadd       dword ptr [esp + 0x14]
0041c20c  fstp       dword ptr [esp + 0xc]
0041c210  fld        dword ptr [esp + 0x20]
0041c214  fadd       dword ptr [esp + 0x18]
0041c218  fstp       dword ptr [esp + 0x10]
0041c21c  fld        dword ptr [esp + 0xc]
0041c220  fadd       dword ptr [esp + 4]
0041c224  fstp       dword ptr [esp + 0x1c]
0041c228  fld        dword ptr [esp + 0x10]
0041c22c  fadd       dword ptr [esp + 8]
0041c230  fstp       dword ptr [esp + 0x20]
0041c234  fld        dword ptr [esp + 0x1c]
0041c238  fadd       dword ptr [esp + 0x24]
0041c23c  fstp       dword ptr [esp + 0x14]
0041c240  mov        eax, dword ptr [esp + 0x14]
0041c244  fld        dword ptr [esp + 0x20]
0041c248  mov        dword ptr [ebx], eax
0041c24a  fadd       dword ptr [esp + 0x28]
0041c24e  fstp       dword ptr [esp + 0x18]
0041c252  mov        ecx, dword ptr [esp + 0x18]
0041c256  mov        dword ptr [ebx + 4], ecx
0041c259  mov        eax, ebx
0041c25b  pop        esi
0041c25c  add        esp, 0x28
0041c25f  ret        

// block 0041c260
0041c260  mov        eax, edi
0041c262  call       0x41c3b0

// block 0041c267
0041c267  fstp       dword ptr [esp + 0x1c]
0041c26b  fld        dword ptr [edi + 8]
0041c26e  pop        esi
0041c26f  fsub       dword ptr [edi]
0041c271  fstp       dword ptr [esp + 0x20]
0041c275  fld        dword ptr [edi + 0xc]
0041c278  fsub       dword ptr [edi + 4]
0041c27b  fstp       dword ptr [esp + 0x24]
0041c27f  fld        dword ptr [esp + 0x20]
0041c283  fld        dword ptr [esp + 0x18]
0041c287  fld        st(0)
0041c289  fmulp      st(2)
0041c28b  fxch       st(1)
0041c28d  fstp       dword ptr [esp + 0x18]
0041c291  fmul       dword ptr [esp + 0x24]
0041c295  fstp       dword ptr [esp + 0x1c]
0041c299  fld        dword ptr [esp + 0x18]
0041c29d  fadd       dword ptr [edi]
0041c29f  fstp       dword ptr [esp + 0x20]
0041c2a3  mov        edx, dword ptr [esp + 0x20]
0041c2a7  fld        dword ptr [edi + 4]
0041c2aa  mov        dword ptr [ebx], edx
0041c2ac  fadd       dword ptr [esp + 0x1c]
0041c2b0  fstp       dword ptr [esp + 0x24]
0041c2b4  mov        eax, dword ptr [esp + 0x24]
0041c2b8  mov        dword ptr [ebx + 4], eax
0041c2bb  mov        eax, ebx
0041c2bd  add        esp, 0x28
0041c2c0  ret        
