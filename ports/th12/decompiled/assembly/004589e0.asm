
// block 004589e0
004589e0  sub        esp, 0x40
004589e3  push       ebp
004589e4  push       esi
004589e5  push       edi
004589e6  mov        edi, eax
004589e8  mov        eax, dword ptr [edi + 0x44]
004589eb  xor        ebp, ebp
004589ed  cmp        eax, ebp
004589ef  mov        dword ptr [esp + 0xc], eax
004589f3  jle        0x458a7c

// block 004589f9
004589f9  lea        esi, [edi + 0x30]
004589fc  call       0x464a80

// block 00458a01
00458a01  mov        ecx, dword ptr [edi + 0x44]
00458a04  cmp        dword ptr [edi + 0x34], ecx
00458a07  mov        dword ptr [esp + 0xc], ecx
00458a0b  jl         0x458a7c

// block 00458a0d
00458a0d  mov        eax, dword ptr [esi + 0x10]
00458a10  mov        dword ptr [esp + 0xc], ecx
00458a14  test       al, 1
00458a16  jne        0x458a33

// block 00458a18
00458a18  fldz       
00458a1a  or         eax, 1
00458a1d  fstp       dword ptr [esi + 8]
00458a20  mov        dword ptr [esi + 4], ebp
00458a23  mov        dword ptr [esi], 0xfff0bdc1
00458a29  mov        dword ptr [esi + 0xc], 0x4b2ed0
00458a30  mov        dword ptr [esi + 0x10], eax

// block 00458a33
00458a33  fild       dword ptr [esp + 0xc]
00458a37  mov        dword ptr [esi + 4], ecx
00458a3a  dec        ecx
00458a3b  mov        dword ptr [esi], ecx
00458a3d  fstp       dword ptr [esi + 8]
00458a40  cmp        dword ptr [edi + 0x48], 7
00458a44  mov        dword ptr [edi + 0x44], ebp
00458a47  je         0x458a63

// block 00458a49
00458a49  mov        eax, dword ptr [edi + 0xc]
00458a4c  mov        ecx, dword ptr [edi + 0x10]
00458a4f  mov        edx, dword ptr [edi + 0x14]
00458a52  pop        edi
00458a53  mov        dword ptr [ebx], eax
00458a55  pop        esi
00458a56  mov        dword ptr [ebx + 4], ecx
00458a59  mov        dword ptr [ebx + 8], edx
00458a5c  mov        eax, ebx
00458a5e  pop        ebp
00458a5f  add        esp, 0x40
00458a62  ret        

// block 00458a63
00458a63  mov        eax, dword ptr [edi]
00458a65  mov        ecx, dword ptr [edi + 4]
00458a68  mov        edx, dword ptr [edi + 8]
00458a6b  pop        edi
00458a6c  mov        dword ptr [ebx], eax
00458a6e  pop        esi
00458a6f  mov        dword ptr [ebx + 4], ecx
00458a72  mov        dword ptr [ebx + 8], edx
00458a75  mov        eax, ebx
00458a77  pop        ebp
00458a78  add        esp, 0x40
00458a7b  ret        

// block 00458a7c
00458a7c  mov        eax, dword ptr [edi + 0x48]
00458a7f  cmp        eax, 7
00458a82  jne        0x458ab4

// block 00458a84
00458a84  mov        ecx, dword ptr [edi]
00458a86  mov        eax, dword ptr [edi + 0xc]
00458a89  mov        edx, dword ptr [edi + 4]
00458a8c  mov        esi, dword ptr [edi + 8]
00458a8f  add        eax, ecx
00458a91  mov        ecx, dword ptr [edi + 0x10]
00458a94  add        ecx, edx
00458a96  mov        edx, dword ptr [edi + 0x14]
00458a99  mov        dword ptr [edi], eax
00458a9b  add        edx, esi
00458a9d  mov        dword ptr [edi + 4], ecx
00458aa0  mov        dword ptr [edi + 8], edx
00458aa3  pop        edi
00458aa4  mov        dword ptr [ebx], eax
00458aa6  pop        esi
00458aa7  mov        dword ptr [ebx + 4], ecx
00458aaa  mov        dword ptr [ebx + 8], edx
00458aad  mov        eax, ebx
00458aaf  pop        ebp
00458ab0  add        esp, 0x40
00458ab3  ret        

// block 00458ab4
00458ab4  cmp        eax, 0x11
00458ab7  jne        0x458b0c

// block 00458ab9
00458ab9  mov        eax, dword ptr [edi + 0x24]
00458abc  mov        ecx, dword ptr [edi]
00458abe  mov        edx, dword ptr [edi + 4]
00458ac1  mov        esi, dword ptr [edi + 8]
00458ac4  add        eax, ecx
00458ac6  mov        ecx, dword ptr [edi + 0x28]
00458ac9  add        ecx, edx
00458acb  mov        edx, dword ptr [edi + 0x2c]
00458ace  mov        dword ptr [edi], eax
00458ad0  add        edx, esi
00458ad2  mov        dword ptr [edi + 4], ecx
00458ad5  mov        dword ptr [edi + 8], edx
00458ad8  mov        ebp, dword ptr [edi + 0x28]
00458adb  add        ebp, dword ptr [edi + 0x10]
00458ade  mov        esi, dword ptr [edi + 0x24]
00458ae1  add        esi, dword ptr [edi + 0xc]
00458ae4  mov        dword ptr [esp + 0x38], ebp
00458ae8  mov        ebp, dword ptr [edi + 0x2c]
00458aeb  add        ebp, dword ptr [edi + 0x14]
00458aee  mov        dword ptr [edi + 0x24], esi
00458af1  mov        esi, dword ptr [esp + 0x38]
00458af5  mov        dword ptr [edi + 0x28], esi
00458af8  mov        dword ptr [edi + 0x2c], ebp
00458afb  pop        edi
00458afc  mov        dword ptr [ebx], eax
00458afe  pop        esi
00458aff  mov        dword ptr [ebx + 4], ecx
00458b02  mov        dword ptr [ebx + 8], edx
00458b05  mov        eax, ebx
00458b07  pop        ebp
00458b08  add        esp, 0x40
00458b0b  ret        

// block 00458b0c
00458b0c  cmp        eax, 8
00458b0f  jne        0x458c62

// block 00458b15
00458b15  fld        dword ptr [edi + 0x38]
00458b18  fidiv      dword ptr [esp + 0xc]
00458b1c  fstp       dword ptr [esp + 0xc]
00458b20  fld        dword ptr [esp + 0xc]
00458b24  fld        st(0)
00458b26  fld1       
00458b28  fsub       st(1), st(0)
00458b2a  fld        st(1)
00458b2c  fld        st(3)
00458b2e  fadd       st(0), st(0)
00458b30  fld        st(0)
00458b32  fadd       st(3)
00458b34  fld        st(2)
00458b36  fmulp      st(3)
00458b38  fmulp      st(2)
00458b3a  fxch       st(1)
00458b3c  fstp       dword ptr [esp + 0x18]
00458b40  fld        st(3)
00458b42  fld        qword ptr [0x4a3fd8]
00458b48  fsubrp     st(2)
00458b4a  fmul       st(0), st(0)
00458b4c  fmulp      st(1)
00458b4e  fstp       dword ptr [esp + 0x14]
00458b52  fsub       st(2)
00458b54  fmul       st(0), st(0)
00458b56  fmul       st(2)
00458b58  fstp       dword ptr [esp + 0x10]
00458b5c  fmul       st(1)
00458b5e  fmulp      st(1)
00458b60  fstp       dword ptr [esp + 0xc]
00458b64  fild       dword ptr [edi + 0x24]
00458b67  fld        dword ptr [esp + 0xc]
00458b6b  fld        st(0)
00458b6d  fmulp      st(2)
00458b6f  fxch       st(1)
00458b71  call       0x4931e0

// block 00458b76
00458b76  fild       dword ptr [edi + 0x28]
00458b79  mov        esi, eax
00458b7b  fmul       st(1)
00458b7d  call       0x4931e0

// block 00458b82
00458b82  fimul      dword ptr [edi + 0x2c]
00458b85  mov        ebp, eax
00458b87  call       0x4931e0

// block 00458b8c
00458b8c  fild       dword ptr [edi + 0x18]
00458b8f  fld        dword ptr [esp + 0x10]
00458b93  mov        dword ptr [esp + 0x48], eax
00458b97  fld        st(0)
00458b99  fmulp      st(2)
00458b9b  fxch       st(1)
00458b9d  call       0x4931e0

// block 00458ba2
00458ba2  fild       dword ptr [edi + 0x1c]
00458ba5  mov        dword ptr [esp + 0x28], eax
00458ba9  fmul       st(1)
00458bab  call       0x4931e0

// block 00458bb0
00458bb0  fimul      dword ptr [edi + 0x20]
00458bb3  mov        dword ptr [esp + 0x2c], eax
00458bb7  call       0x4931e0

// block 00458bbc
00458bbc  fild       dword ptr [edi + 0xc]
00458bbf  fld        dword ptr [esp + 0x14]
00458bc3  mov        dword ptr [esp + 0x30], eax
00458bc7  fld        st(0)
00458bc9  fmulp      st(2)
00458bcb  fxch       st(1)
00458bcd  call       0x4931e0

// block 00458bd2
00458bd2  fild       dword ptr [edi + 0x10]
00458bd5  mov        dword ptr [esp + 0x1c], eax
00458bd9  fmul       st(1)
00458bdb  call       0x4931e0

// block 00458be0
00458be0  fimul      dword ptr [edi + 0x14]
00458be3  mov        dword ptr [esp + 0x20], eax
00458be7  call       0x4931e0

// block 00458bec
00458bec  fild       dword ptr [edi]
00458bee  fld        dword ptr [esp + 0x18]
00458bf2  mov        dword ptr [esp + 0x24], eax
00458bf6  fld        st(0)
00458bf8  fmulp      st(2)
00458bfa  fxch       st(1)
00458bfc  call       0x4931e0

// block 00458c01
00458c01  fild       dword ptr [edi + 4]
00458c04  mov        dword ptr [esp + 0x34], eax
00458c08  fmul       st(1)
00458c0a  call       0x4931e0

// block 00458c0f
00458c0f  fimul      dword ptr [edi + 8]
00458c12  mov        dword ptr [esp + 0x38], eax
00458c16  call       0x4931e0

// block 00458c1b
00458c1b  mov        edx, dword ptr [esp + 0x1c]
00458c1f  mov        ecx, dword ptr [esp + 0x34]
00458c23  mov        edi, dword ptr [esp + 0x20]
00458c27  add        ecx, edx
00458c29  mov        edx, dword ptr [esp + 0x38]
00458c2d  add        edx, edi
00458c2f  mov        edi, dword ptr [esp + 0x24]
00458c33  add        eax, edi
00458c35  mov        edi, dword ptr [esp + 0x28]
00458c39  add        ecx, edi
00458c3b  mov        edi, dword ptr [esp + 0x2c]
00458c3f  add        edx, edi
00458c41  mov        edi, dword ptr [esp + 0x30]
00458c45  add        ecx, esi
00458c47  mov        esi, dword ptr [esp + 0x48]
00458c4b  add        eax, edi
00458c4d  add        edx, ebp
00458c4f  add        eax, esi
00458c51  pop        edi
00458c52  mov        dword ptr [ebx], ecx
00458c54  mov        dword ptr [ebx + 4], edx
00458c57  pop        esi
00458c58  mov        dword ptr [ebx + 8], eax
00458c5b  mov        eax, ebx
00458c5d  pop        ebp
00458c5e  add        esp, 0x40
00458c61  ret        

// block 00458c62
00458c62  mov        eax, edi
00458c64  call       0x459300

// block 00458c69
00458c69  fstp       dword ptr [esp + 0x18]
00458c6d  mov        esi, dword ptr [edi]
00458c6f  mov        eax, dword ptr [edi + 0xc]
00458c72  mov        ebp, dword ptr [edi + 4]
00458c75  mov        ecx, dword ptr [edi + 0x10]
00458c78  mov        edx, dword ptr [edi + 0x14]
00458c7b  sub        eax, esi
00458c7d  mov        dword ptr [esp + 0x28], eax
00458c81  fild       dword ptr [esp + 0x28]
00458c85  mov        eax, dword ptr [edi + 8]
00458c88  fld        dword ptr [esp + 0x18]
00458c8c  sub        ecx, ebp
00458c8e  fld        st(0)
00458c90  sub        edx, eax
00458c92  fmulp      st(2)
00458c94  mov        dword ptr [esp + 0x2c], ecx
00458c98  fxch       st(1)
00458c9a  mov        dword ptr [esp + 0x14], eax
00458c9e  mov        dword ptr [esp + 0x30], edx
00458ca2  call       0x4931e0

// block 00458ca7
00458ca7  fild       dword ptr [esp + 0x2c]
00458cab  mov        edi, eax
00458cad  fmul       st(1)
00458caf  call       0x4931e0

// block 00458cb4
00458cb4  fimul      dword ptr [esp + 0x30]
00458cb8  mov        dword ptr [esp + 0x44], eax
00458cbc  call       0x4931e0

// block 00458cc1
00458cc1  mov        ecx, dword ptr [esp + 0x44]
00458cc5  mov        edx, dword ptr [esp + 0x14]
00458cc9  add        esi, edi
00458ccb  add        ebp, ecx
00458ccd  mov        dword ptr [ebx], esi
00458ccf  pop        edi
00458cd0  add        eax, edx
00458cd2  mov        dword ptr [ebx + 4], ebp
00458cd5  pop        esi
00458cd6  mov        dword ptr [ebx + 8], eax
00458cd9  mov        eax, ebx
00458cdb  pop        ebp
00458cdc  add        esp, 0x40
00458cdf  ret        
