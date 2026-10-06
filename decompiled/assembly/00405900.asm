
// block 00405900
00405900  sub        esp, 0x4c
00405903  mov        eax, dword ptr [edi + 0x44]
00405906  push       esi
00405907  mov        dword ptr [esp + 4], eax
0040590b  test       eax, eax
0040590d  jle        0x40599a

// block 00405913
00405913  lea        esi, [edi + 0x30]
00405916  call       0x464a80

// block 0040591b
0040591b  mov        ecx, dword ptr [edi + 0x44]
0040591e  cmp        dword ptr [edi + 0x34], ecx
00405921  mov        dword ptr [esp + 4], ecx
00405925  jl         0x40599a

// block 00405927
00405927  mov        eax, dword ptr [esi + 0x10]
0040592a  mov        dword ptr [esp + 4], ecx
0040592e  test       al, 1
00405930  jne        0x405951

// block 00405932
00405932  fldz       
00405934  or         eax, 1
00405937  fstp       dword ptr [esi + 8]
0040593a  mov        dword ptr [esi + 4], 0
00405941  mov        dword ptr [esi], 0xfff0bdc1
00405947  mov        dword ptr [esi + 0xc], 0x4b2ed0
0040594e  mov        dword ptr [esi + 0x10], eax

// block 00405951
00405951  fild       dword ptr [esp + 4]
00405955  mov        dword ptr [esi + 4], ecx
00405958  dec        ecx
00405959  mov        dword ptr [esi], ecx
0040595b  fstp       dword ptr [esi + 8]
0040595e  cmp        dword ptr [edi + 0x48], 7
00405962  mov        dword ptr [edi + 0x44], 0
00405969  je         0x405983

// block 0040596b
0040596b  mov        eax, dword ptr [edi + 0xc]
0040596e  mov        ecx, dword ptr [edi + 0x10]
00405971  mov        edx, dword ptr [edi + 0x14]
00405974  mov        dword ptr [ebx], eax
00405976  mov        dword ptr [ebx + 4], ecx
00405979  mov        eax, ebx
0040597b  mov        dword ptr [ebx + 8], edx
0040597e  pop        esi
0040597f  add        esp, 0x4c
00405982  ret        

// block 00405983
00405983  mov        eax, dword ptr [edi]
00405985  mov        ecx, dword ptr [edi + 4]
00405988  mov        edx, dword ptr [edi + 8]
0040598b  mov        dword ptr [ebx], eax
0040598d  mov        dword ptr [ebx + 4], ecx
00405990  mov        eax, ebx
00405992  mov        dword ptr [ebx + 8], edx
00405995  pop        esi
00405996  add        esp, 0x4c
00405999  ret        

// block 0040599a
0040599a  mov        eax, dword ptr [edi + 0x48]
0040599d  cmp        eax, 7
004059a0  jne        0x4059fa

// block 004059a2
004059a2  mov        eax, dword ptr [edi]
004059a4  fld        dword ptr [edi + 0xc]
004059a7  mov        ecx, dword ptr [edi + 4]
004059aa  mov        edx, dword ptr [edi + 8]
004059ad  mov        dword ptr [esp + 0x14], eax
004059b1  fadd       dword ptr [esp + 0x14]
004059b5  mov        dword ptr [esp + 0x18], ecx
004059b9  mov        dword ptr [esp + 0x1c], edx
004059bd  fstp       dword ptr [esp + 0x20]
004059c1  mov        eax, dword ptr [esp + 0x20]
004059c5  fld        dword ptr [edi + 0x10]
004059c8  mov        dword ptr [ebx], eax
004059ca  fadd       dword ptr [esp + 0x18]
004059ce  pop        esi
004059cf  fstp       dword ptr [esp + 0x20]
004059d3  mov        ecx, dword ptr [esp + 0x20]
004059d7  fld        dword ptr [edi + 0x14]
004059da  mov        dword ptr [edi], eax
004059dc  fadd       dword ptr [esp + 0x18]
004059e0  mov        dword ptr [edi + 4], ecx
004059e3  mov        dword ptr [ebx + 4], ecx
004059e6  mov        eax, ebx
004059e8  fstp       dword ptr [esp + 0x24]
004059ec  mov        edx, dword ptr [esp + 0x24]
004059f0  mov        dword ptr [edi + 8], edx
004059f3  mov        dword ptr [ebx + 8], edx
004059f6  add        esp, 0x4c
004059f9  ret        

// block 004059fa
004059fa  cmp        eax, 0x11
004059fd  jne        0x405a8e

// block 00405a03
00405a03  fld        dword ptr [edi + 0x24]
00405a06  mov        eax, dword ptr [edi]
00405a08  mov        ecx, dword ptr [edi + 4]
00405a0b  mov        edx, dword ptr [edi + 8]
00405a0e  mov        dword ptr [esp + 0x20], eax
00405a12  fadd       dword ptr [esp + 0x20]
00405a16  mov        dword ptr [esp + 0x24], ecx
00405a1a  mov        dword ptr [esp + 0x28], edx
00405a1e  fstp       dword ptr [esp + 0x14]
00405a22  mov        eax, dword ptr [esp + 0x14]
00405a26  fld        dword ptr [edi + 0x28]
00405a29  mov        dword ptr [ebx], eax
00405a2b  fadd       dword ptr [esp + 0x24]
00405a2f  fstp       dword ptr [esp + 0x18]
00405a33  mov        ecx, dword ptr [esp + 0x18]
00405a37  fld        dword ptr [edi + 0x2c]
00405a3a  mov        dword ptr [edi], eax
00405a3c  fadd       dword ptr [esp + 0x28]
00405a40  mov        dword ptr [edi + 4], ecx
00405a43  mov        dword ptr [ebx + 4], ecx
00405a46  mov        eax, ebx
00405a48  fstp       dword ptr [esp + 0x1c]
00405a4c  mov        edx, dword ptr [esp + 0x1c]
00405a50  mov        dword ptr [edi + 8], edx
00405a53  fld        dword ptr [edi + 0x24]
00405a56  fadd       dword ptr [edi + 0xc]
00405a59  mov        dword ptr [ebx + 8], edx
00405a5c  fstp       dword ptr [esp + 0x20]
00405a60  mov        esi, dword ptr [esp + 0x20]
00405a64  fld        dword ptr [edi + 0x28]
00405a67  fadd       dword ptr [edi + 0x10]
00405a6a  fstp       dword ptr [esp + 0x24]
00405a6e  fld        dword ptr [edi + 0x2c]
00405a71  fadd       dword ptr [edi + 0x14]
00405a74  mov        dword ptr [edi + 0x24], esi
00405a77  mov        esi, dword ptr [esp + 0x24]
00405a7b  mov        dword ptr [edi + 0x28], esi
00405a7e  fstp       dword ptr [esp + 0x28]
00405a82  mov        esi, dword ptr [esp + 0x28]
00405a86  mov        dword ptr [edi + 0x2c], esi
00405a89  pop        esi
00405a8a  add        esp, 0x4c
00405a8d  ret        

// block 00405a8e
00405a8e  cmp        eax, 8
00405a91  jne        0x405bec

// block 00405a97
00405a97  fld        dword ptr [edi + 0x38]
00405a9a  fidiv      dword ptr [esp + 4]
00405a9e  fstp       dword ptr [esp + 4]
00405aa2  fld        dword ptr [esp + 4]
00405aa6  fld        st(0)
00405aa8  fld1       
00405aaa  fsub       st(1), st(0)
00405aac  fld        st(1)
00405aae  fld        st(3)
00405ab0  fadd       st(0), st(0)
00405ab2  fld        st(1)
00405ab4  fmulp      st(2)
00405ab6  fld        st(0)
00405ab8  fadd       st(3)
00405aba  fmulp      st(2)
00405abc  fxch       st(1)
00405abe  fstp       dword ptr [esp + 0x10]
00405ac2  fld        st(3)
00405ac4  fmul       st(0), st(0)
00405ac6  fld        qword ptr [0x4a3fd8]
00405acc  fsubrp     st(2)
00405ace  fmulp      st(1)
00405ad0  fstp       dword ptr [esp + 0xc]
00405ad4  fsub       st(2)
00405ad6  fmul       st(0), st(0)
00405ad8  fmul       st(2)
00405ada  fstp       dword ptr [esp + 8]
00405ade  fmul       st(1)
00405ae0  fmulp      st(1)
00405ae2  fstp       dword ptr [esp + 4]
00405ae6  fld        dword ptr [edi + 0x24]
00405ae9  fld        dword ptr [esp + 4]
00405aed  fld        st(0)
00405aef  fmulp      st(2)
00405af1  fxch       st(1)
00405af3  fstp       dword ptr [esp + 0x44]
00405af7  fld        dword ptr [edi + 0x28]
00405afa  fmul       st(1)
00405afc  fstp       dword ptr [esp + 0x48]
00405b00  fmul       dword ptr [edi + 0x2c]
00405b03  fstp       dword ptr [esp + 0x4c]
00405b07  fld        dword ptr [edi + 0x18]
00405b0a  fld        dword ptr [esp + 8]
00405b0e  fld        st(0)
00405b10  fmulp      st(2)
00405b12  fxch       st(1)
00405b14  fstp       dword ptr [esp + 0x38]
00405b18  fld        dword ptr [edi + 0x1c]
00405b1b  fmul       st(1)
00405b1d  fstp       dword ptr [esp + 0x3c]
00405b21  fmul       dword ptr [edi + 0x20]
00405b24  fstp       dword ptr [esp + 0x40]
00405b28  fld        dword ptr [esp + 0xc]
00405b2c  fld        st(0)
00405b2e  fmul       dword ptr [edi + 0xc]
00405b31  fstp       dword ptr [esp + 0x14]
00405b35  fld        dword ptr [edi + 0x10]
00405b38  fmul       st(1)
00405b3a  fstp       dword ptr [esp + 0x18]
00405b3e  fmul       dword ptr [edi + 0x14]
00405b41  fstp       dword ptr [esp + 0x1c]
00405b45  fld        dword ptr [edi]
00405b47  fld        dword ptr [esp + 0x10]
00405b4b  fld        st(0)
00405b4d  fmulp      st(2)
00405b4f  fxch       st(1)
00405b51  fstp       dword ptr [esp + 0x20]
00405b55  fld        dword ptr [edi + 4]
00405b58  fmul       st(1)
00405b5a  fstp       dword ptr [esp + 0x24]
00405b5e  fmul       dword ptr [edi + 8]
00405b61  fstp       dword ptr [esp + 0x28]
00405b65  fld        dword ptr [esp + 0x20]
00405b69  fadd       dword ptr [esp + 0x14]
00405b6d  fstp       dword ptr [esp + 0x2c]
00405b71  fld        dword ptr [esp + 0x24]
00405b75  fadd       dword ptr [esp + 0x18]
00405b79  fstp       dword ptr [esp + 0x30]
00405b7d  fld        dword ptr [esp + 0x28]
00405b81  fadd       dword ptr [esp + 0x1c]
00405b85  fstp       dword ptr [esp + 0x34]
00405b89  fld        dword ptr [esp + 0x2c]
00405b8d  pop        esi
00405b8e  fadd       dword ptr [esp + 0x34]
00405b92  fstp       dword ptr [esp + 0x1c]
00405b96  fld        dword ptr [esp + 0x2c]
00405b9a  fadd       dword ptr [esp + 0x38]
00405b9e  fstp       dword ptr [esp + 0x20]
00405ba2  fld        dword ptr [esp + 0x30]
00405ba6  fadd       dword ptr [esp + 0x3c]
00405baa  fstp       dword ptr [esp + 0x24]
00405bae  fld        dword ptr [esp + 0x1c]
00405bb2  fadd       dword ptr [esp + 0x40]
00405bb6  fstp       dword ptr [esp + 0x34]
00405bba  mov        eax, dword ptr [esp + 0x34]
00405bbe  fld        dword ptr [esp + 0x20]
00405bc2  mov        dword ptr [ebx], eax
00405bc4  fadd       dword ptr [esp + 0x44]
00405bc8  mov        eax, ebx

// block 00405bca
00405bca  fstp       dword ptr [esp + 0x38]
00405bce  mov        ecx, dword ptr [esp + 0x38]
00405bd2  fld        dword ptr [esp + 0x24]
00405bd6  mov        dword ptr [ebx + 4], ecx
00405bd9  fadd       dword ptr [esp + 0x48]
00405bdd  fstp       dword ptr [esp + 0x3c]
00405be1  mov        edx, dword ptr [esp + 0x3c]
00405be5  mov        dword ptr [ebx + 8], edx
00405be8  add        esp, 0x4c
00405beb  ret        

// block 00405bec
00405bec  mov        eax, edi
00405bee  call       0x406020

// block 00405bf3
00405bf3  fstp       dword ptr [esp + 0x10]
00405bf7  fld        dword ptr [edi + 0xc]
00405bfa  pop        esi
00405bfb  fsub       dword ptr [edi]
00405bfd  fstp       dword ptr [esp + 0x40]
00405c01  fld        dword ptr [edi + 0x10]
00405c04  fsub       dword ptr [edi + 4]
00405c07  fstp       dword ptr [esp + 0x44]
00405c0b  fld        dword ptr [edi + 0x14]
00405c0e  fsub       dword ptr [edi + 8]
00405c11  fstp       dword ptr [esp + 0x48]
00405c15  fld        dword ptr [esp + 0x40]
00405c19  fld        dword ptr [esp + 0xc]
00405c1d  fld        st(0)
00405c1f  fmulp      st(2)
00405c21  fxch       st(1)
00405c23  fstp       dword ptr [esp + 0x34]
00405c27  fld        dword ptr [esp + 0x44]
00405c2b  fmul       st(1)
00405c2d  fstp       dword ptr [esp + 0x38]
00405c31  fmul       dword ptr [esp + 0x48]
00405c35  fstp       dword ptr [esp + 0x3c]
00405c39  fld        dword ptr [esp + 0x34]
00405c3d  fadd       dword ptr [edi]
00405c3f  fstp       dword ptr [esp + 0x40]
00405c43  mov        eax, dword ptr [esp + 0x40]
00405c47  fld        dword ptr [esp + 0x38]
00405c4b  mov        dword ptr [ebx], eax
00405c4d  fadd       dword ptr [edi + 4]
00405c50  mov        eax, ebx
00405c52  fstp       dword ptr [esp + 0x44]
00405c56  mov        ecx, dword ptr [esp + 0x44]
00405c5a  fld        dword ptr [edi + 8]
00405c5d  mov        dword ptr [ebx + 4], ecx
00405c60  fadd       dword ptr [esp + 0x3c]
00405c64  fstp       dword ptr [esp + 0x48]
00405c68  mov        edx, dword ptr [esp + 0x48]
00405c6c  mov        dword ptr [ebx + 8], edx
00405c6f  add        esp, 0x4c
00405c72  ret        
