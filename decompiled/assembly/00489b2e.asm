
// block 00489b2e
00489b2e  mov        edi, edi
00489b30  push       ebp
00489b31  mov        ebp, esp
00489b33  sub        esp, 0x2c
00489b36  mov        eax, dword ptr [ebp + 8]
00489b39  movzx      ecx, word ptr [eax + 0xa]
00489b3d  push       ebx
00489b3e  mov        ebx, ecx
00489b40  and        ecx, 0x8000
00489b46  mov        dword ptr [ebp - 0x14], ecx
00489b49  mov        ecx, dword ptr [eax + 6]
00489b4c  mov        dword ptr [ebp - 0x20], ecx
00489b4f  mov        ecx, dword ptr [eax + 2]
00489b52  movzx      eax, word ptr [eax]
00489b55  and        ebx, 0x7fff
00489b5b  sub        ebx, 0x3fff
00489b61  shl        eax, 0x10
00489b64  push       edi
00489b65  mov        dword ptr [ebp - 0x1c], ecx
00489b68  mov        dword ptr [ebp - 0x18], eax
00489b6b  cmp        ebx, 0xffffc001
00489b71  jne        0x489b9a

// block 00489b73
00489b73  xor        ebx, ebx
00489b75  xor        eax, eax

// block 00489b77
00489b77  cmp        dword ptr [ebp + eax*4 - 0x20], ebx
00489b7b  jne        0x489b8a

// block 00489b7d
00489b7d  inc        eax
00489b7e  cmp        eax, 3
00489b81  jl         0x489b77

// block 00489b83
00489b83  xor        eax, eax
00489b85  jmp        0x48a02f

// block 00489b8a
00489b8a  xor        eax, eax
00489b8c  lea        edi, [ebp - 0x20]
00489b8f  stosd      dword ptr es:[edi], eax
00489b90  stosd      dword ptr es:[edi], eax
00489b91  push       2
00489b93  stosd      dword ptr es:[edi], eax
00489b94  pop        eax
00489b95  jmp        0x48a02f

// block 00489b9a
00489b9a  and        dword ptr [ebp + 8], 0
00489b9e  push       esi
00489b9f  lea        esi, [ebp - 0x20]
00489ba2  lea        edi, [ebp - 0x2c]
00489ba5  movsd      dword ptr es:[edi], dword ptr [esi]
00489ba6  movsd      dword ptr es:[edi], dword ptr [esi]
00489ba7  movsd      dword ptr es:[edi], dword ptr [esi]
00489ba8  mov        esi, dword ptr [0x4adfd0]
00489bae  dec        esi
00489baf  lea        ecx, [esi + 1]
00489bb2  mov        eax, ecx
00489bb4  cdq        
00489bb5  and        edx, 0x1f
00489bb8  add        eax, edx
00489bba  sar        eax, 5
00489bbd  mov        edx, ecx
00489bbf  and        edx, 0x8000001f
00489bc5  mov        dword ptr [ebp - 0x10], ebx
00489bc8  mov        dword ptr [ebp - 0xc], eax
00489bcb  jns        0x489bd2

// block 00489bcd
00489bcd  dec        edx
00489bce  or         edx, 0xffffffe0
00489bd1  inc        edx

// block 00489bd2
00489bd2  lea        edi, [ebp + eax*4 - 0x20]
00489bd6  push       0x1f
00489bd8  xor        eax, eax
00489bda  pop        ecx
00489bdb  sub        ecx, edx
00489bdd  inc        eax
00489bde  shl        eax, cl
00489be0  mov        dword ptr [ebp - 8], ecx
00489be3  test       dword ptr [edi], eax
00489be5  je         0x489c78

// block 00489beb
00489beb  mov        eax, dword ptr [ebp - 0xc]
00489bee  or         edx, 0xffffffff
00489bf1  shl        edx, cl
00489bf3  not        edx
00489bf5  test       dword ptr [ebp + eax*4 - 0x20], edx
00489bf9  jmp        0x489c00

// block 00489bfb
00489bfb  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 00489c00
00489c00  jne        0x489c0a

// block 00489c02
00489c02  inc        eax
00489c03  cmp        eax, 3
00489c06  jl         0x489bfb

// block 00489c08
00489c08  jmp        0x489c78

// block 00489c0a
00489c0a  mov        eax, esi
00489c0c  cdq        
00489c0d  push       0x1f
00489c0f  pop        ecx
00489c10  and        edx, ecx
00489c12  add        eax, edx
00489c14  sar        eax, 5
00489c17  and        esi, 0x8000001f
00489c1d  jns        0x489c24

// block 00489c1f
00489c1f  dec        esi
00489c20  or         esi, 0xffffffe0
00489c23  inc        esi

// block 00489c24
00489c24  and        dword ptr [ebp - 4], 0
00489c28  sub        ecx, esi
00489c2a  xor        edx, edx
00489c2c  inc        edx
00489c2d  shl        edx, cl
00489c2f  lea        ecx, [ebp + eax*4 - 0x20]
00489c33  mov        esi, dword ptr [ecx]
00489c35  add        esi, edx
00489c37  mov        dword ptr [ebp + 8], esi
00489c3a  mov        esi, dword ptr [ecx]
00489c3c  cmp        dword ptr [ebp + 8], esi
00489c3f  jb         0x489c63

// block 00489c41
00489c41  cmp        dword ptr [ebp + 8], edx
00489c44  jmp        0x489c61

// block 00489c46
00489c46  test       ecx, ecx
00489c48  je         0x489c75

// block 00489c4a
00489c4a  and        dword ptr [ebp - 4], 0
00489c4e  lea        ecx, [ebp + eax*4 - 0x20]
00489c52  mov        edx, dword ptr [ecx]
00489c54  lea        esi, [edx + 1]
00489c57  mov        dword ptr [ebp + 8], esi
00489c5a  cmp        esi, edx
00489c5c  jb         0x489c63

// block 00489c5e
00489c5e  cmp        esi, 1

// block 00489c61
00489c61  jae        0x489c6a

// block 00489c63
00489c63  mov        dword ptr [ebp - 4], 1

// block 00489c6a
00489c6a  dec        eax
00489c6b  mov        edx, dword ptr [ebp + 8]
00489c6e  mov        dword ptr [ecx], edx
00489c70  mov        ecx, dword ptr [ebp - 4]
00489c73  jns        0x489c46

// block 00489c75
00489c75  mov        dword ptr [ebp + 8], ecx

// block 00489c78
00489c78  mov        ecx, dword ptr [ebp - 8]
00489c7b  or         eax, 0xffffffff
00489c7e  shl        eax, cl
00489c80  and        dword ptr [edi], eax
00489c82  mov        eax, dword ptr [ebp - 0xc]
00489c85  inc        eax
00489c86  cmp        eax, 3
00489c89  jge        0x489c98

// block 00489c8b
00489c8b  push       3
00489c8d  pop        ecx
00489c8e  lea        edi, [ebp + eax*4 - 0x20]
00489c92  sub        ecx, eax
00489c94  xor        eax, eax

// block 00489c96
00489c96  rep stosd  dword ptr es:[edi], eax

// block 00489c98
00489c98  cmp        dword ptr [ebp + 8], 0
00489c9c  je         0x489c9f

// block 00489c9e
00489c9e  inc        ebx

// block 00489c9f
00489c9f  mov        eax, dword ptr [0x4adfcc]
00489ca4  mov        ecx, eax
00489ca6  sub        ecx, dword ptr [0x4adfd0]
00489cac  cmp        ebx, ecx
00489cae  jge        0x489cbd

// block 00489cb0
00489cb0  xor        eax, eax
00489cb2  lea        edi, [ebp - 0x20]
00489cb5  stosd      dword ptr es:[edi], eax
00489cb6  stosd      dword ptr es:[edi], eax
00489cb7  stosd      dword ptr es:[edi], eax
00489cb8  jmp        0x489eca

// block 00489cbd
00489cbd  cmp        ebx, eax
00489cbf  jg         0x489ed4

// block 00489cc5
00489cc5  sub        eax, dword ptr [ebp - 0x10]
00489cc8  lea        esi, [ebp - 0x2c]
00489ccb  mov        ecx, eax
00489ccd  lea        edi, [ebp - 0x20]
00489cd0  movsd      dword ptr es:[edi], dword ptr [esi]
00489cd1  cdq        
00489cd2  and        edx, 0x1f
00489cd5  add        eax, edx
00489cd7  movsd      dword ptr es:[edi], dword ptr [esi]
00489cd8  mov        edx, ecx
00489cda  sar        eax, 5
00489cdd  and        edx, 0x8000001f
00489ce3  movsd      dword ptr es:[edi], dword ptr [esi]
00489ce4  jns        0x489ceb

// block 00489ce6
00489ce6  dec        edx
00489ce7  or         edx, 0xffffffe0
00489cea  inc        edx

// block 00489ceb
00489ceb  and        dword ptr [ebp - 0xc], 0
00489cef  and        dword ptr [ebp + 8], 0
00489cf3  or         edi, 0xffffffff
00489cf6  mov        ecx, edx
00489cf8  shl        edi, cl
00489cfa  mov        dword ptr [ebp - 4], 0x20
00489d01  sub        dword ptr [ebp - 4], edx
00489d04  not        edi

// block 00489d06
00489d06  mov        ebx, dword ptr [ebp + 8]
00489d09  lea        ebx, [ebp + ebx*4 - 0x20]
00489d0d  mov        esi, dword ptr [ebx]
00489d0f  mov        ecx, esi
00489d11  and        ecx, edi
00489d13  mov        dword ptr [ebp - 0x10], ecx
00489d16  mov        ecx, edx
00489d18  shr        esi, cl
00489d1a  mov        ecx, dword ptr [ebp - 4]
00489d1d  or         esi, dword ptr [ebp - 0xc]
00489d20  mov        dword ptr [ebx], esi
00489d22  mov        esi, dword ptr [ebp - 0x10]
00489d25  shl        esi, cl
00489d27  inc        dword ptr [ebp + 8]
00489d2a  cmp        dword ptr [ebp + 8], 3
00489d2e  mov        dword ptr [ebp - 0xc], esi
00489d31  jl         0x489d06

// block 00489d33
00489d33  mov        esi, eax
00489d35  push       2
00489d37  shl        esi, 2
00489d3a  lea        ecx, [ebp - 0x18]
00489d3d  pop        edx
00489d3e  sub        ecx, esi

// block 00489d40
00489d40  cmp        edx, eax
00489d42  jl         0x489d4c

// block 00489d44
00489d44  mov        esi, dword ptr [ecx]
00489d46  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489d4a  jmp        0x489d51

// block 00489d4c
00489d4c  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489d51
00489d51  dec        edx
00489d52  sub        ecx, 4
00489d55  test       edx, edx
00489d57  jge        0x489d40

// block 00489d59
00489d59  mov        esi, dword ptr [0x4adfd0]
00489d5f  dec        esi
00489d60  lea        ecx, [esi + 1]
00489d63  mov        eax, ecx
00489d65  cdq        
00489d66  and        edx, 0x1f
00489d69  add        eax, edx
00489d6b  sar        eax, 5
00489d6e  mov        edx, ecx
00489d70  and        edx, 0x8000001f
00489d76  mov        dword ptr [ebp - 0xc], eax
00489d79  jns        0x489d80

// block 00489d7b
00489d7b  dec        edx
00489d7c  or         edx, 0xffffffe0
00489d7f  inc        edx

// block 00489d80
00489d80  push       0x1f
00489d82  pop        ecx
00489d83  sub        ecx, edx
00489d85  xor        edx, edx
00489d87  inc        edx
00489d88  shl        edx, cl
00489d8a  lea        ebx, [ebp + eax*4 - 0x20]
00489d8e  mov        dword ptr [ebp - 0x10], ecx
00489d91  test       dword ptr [ebx], edx
00489d93  je         0x489e1b

// block 00489d99
00489d99  or         edx, 0xffffffff
00489d9c  shl        edx, cl
00489d9e  not        edx
00489da0  test       dword ptr [ebp + eax*4 - 0x20], edx
00489da4  jmp        0x489dab

// block 00489da6
00489da6  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 00489dab
00489dab  jne        0x489db5

// block 00489dad
00489dad  inc        eax
00489dae  cmp        eax, 3
00489db1  jl         0x489da6

// block 00489db3
00489db3  jmp        0x489e1b

// block 00489db5
00489db5  mov        eax, esi
00489db7  cdq        
00489db8  push       0x1f
00489dba  pop        ecx
00489dbb  and        edx, ecx
00489dbd  add        eax, edx
00489dbf  sar        eax, 5
00489dc2  and        esi, 0x8000001f
00489dc8  jns        0x489dcf

// block 00489dca
00489dca  dec        esi
00489dcb  or         esi, 0xffffffe0
00489dce  inc        esi

// block 00489dcf
00489dcf  and        dword ptr [ebp + 8], 0
00489dd3  xor        edx, edx
00489dd5  sub        ecx, esi
00489dd7  inc        edx
00489dd8  shl        edx, cl
00489dda  lea        ecx, [ebp + eax*4 - 0x20]
00489dde  mov        esi, dword ptr [ecx]
00489de0  lea        edi, [esi + edx]
00489de3  cmp        edi, esi
00489de5  jb         0x489deb

// block 00489de7
00489de7  cmp        edi, edx
00489de9  jae        0x489df2

// block 00489deb
00489deb  mov        dword ptr [ebp + 8], 1

// block 00489df2
00489df2  mov        dword ptr [ecx], edi
00489df4  mov        ecx, dword ptr [ebp + 8]
00489df7  jmp        0x489e18

// block 00489df9
00489df9  test       ecx, ecx
00489dfb  je         0x489e1b

// block 00489dfd
00489dfd  lea        ecx, [ebp + eax*4 - 0x20]
00489e01  mov        edx, dword ptr [ecx]
00489e03  lea        esi, [edx + 1]
00489e06  xor        edi, edi
00489e08  cmp        esi, edx
00489e0a  jb         0x489e11

// block 00489e0c
00489e0c  cmp        esi, 1
00489e0f  jae        0x489e14

// block 00489e11
00489e11  xor        edi, edi
00489e13  inc        edi

// block 00489e14
00489e14  mov        dword ptr [ecx], esi
00489e16  mov        ecx, edi

// block 00489e18
00489e18  dec        eax
00489e19  jns        0x489df9

// block 00489e1b
00489e1b  mov        ecx, dword ptr [ebp - 0x10]
00489e1e  or         eax, 0xffffffff
00489e21  shl        eax, cl
00489e23  and        dword ptr [ebx], eax
00489e25  mov        eax, dword ptr [ebp - 0xc]
00489e28  inc        eax
00489e29  cmp        eax, 3
00489e2c  jge        0x489e3b

// block 00489e2e
00489e2e  push       3
00489e30  pop        ecx
00489e31  lea        edi, [ebp + eax*4 - 0x20]
00489e35  sub        ecx, eax
00489e37  xor        eax, eax

// block 00489e39
00489e39  rep stosd  dword ptr es:[edi], eax

// block 00489e3b
00489e3b  mov        ecx, dword ptr [0x4adfd4]
00489e41  inc        ecx
00489e42  mov        eax, ecx
00489e44  cdq        
00489e45  and        edx, 0x1f
00489e48  add        eax, edx
00489e4a  mov        edx, ecx
00489e4c  sar        eax, 5
00489e4f  and        edx, 0x8000001f
00489e55  jns        0x489e5c

// block 00489e57
00489e57  dec        edx
00489e58  or         edx, 0xffffffe0
00489e5b  inc        edx

// block 00489e5c
00489e5c  and        dword ptr [ebp - 0xc], 0
00489e60  and        dword ptr [ebp + 8], 0
00489e64  or         edi, 0xffffffff
00489e67  mov        ecx, edx
00489e69  shl        edi, cl
00489e6b  mov        dword ptr [ebp - 4], 0x20
00489e72  sub        dword ptr [ebp - 4], edx
00489e75  not        edi

// block 00489e77
00489e77  mov        ebx, dword ptr [ebp + 8]
00489e7a  lea        ebx, [ebp + ebx*4 - 0x20]
00489e7e  mov        esi, dword ptr [ebx]
00489e80  mov        ecx, esi
00489e82  and        ecx, edi
00489e84  mov        dword ptr [ebp - 0x10], ecx
00489e87  mov        ecx, edx
00489e89  shr        esi, cl
00489e8b  mov        ecx, dword ptr [ebp - 4]
00489e8e  or         esi, dword ptr [ebp - 0xc]
00489e91  mov        dword ptr [ebx], esi
00489e93  mov        esi, dword ptr [ebp - 0x10]
00489e96  shl        esi, cl
00489e98  inc        dword ptr [ebp + 8]
00489e9b  cmp        dword ptr [ebp + 8], 3
00489e9f  mov        dword ptr [ebp - 0xc], esi
00489ea2  jl         0x489e77

// block 00489ea4
00489ea4  mov        esi, eax
00489ea6  push       2
00489ea8  shl        esi, 2
00489eab  lea        ecx, [ebp - 0x18]
00489eae  pop        edx
00489eaf  sub        ecx, esi

// block 00489eb1
00489eb1  cmp        edx, eax
00489eb3  jl         0x489ebd

// block 00489eb5
00489eb5  mov        esi, dword ptr [ecx]
00489eb7  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489ebb  jmp        0x489ec2

// block 00489ebd
00489ebd  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489ec2
00489ec2  dec        edx
00489ec3  sub        ecx, 4
00489ec6  test       edx, edx
00489ec8  jge        0x489eb1

// block 00489eca
00489eca  push       2
00489ecc  xor        ebx, ebx
00489ece  pop        eax
00489ecf  jmp        0x48a02e

// block 00489ed4
00489ed4  cmp        ebx, dword ptr [0x4adfc8]
00489eda  mov        ecx, dword ptr [0x4adfd4]
00489ee0  jl         0x489f93

// block 00489ee6
00489ee6  xor        eax, eax
00489ee8  lea        edi, [ebp - 0x20]
00489eeb  stosd      dword ptr es:[edi], eax
00489eec  stosd      dword ptr es:[edi], eax
00489eed  stosd      dword ptr es:[edi], eax
00489eee  or         dword ptr [ebp - 0x20], 0x80000000
00489ef5  mov        eax, ecx
00489ef7  cdq        
00489ef8  and        edx, 0x1f
00489efb  add        eax, edx
00489efd  mov        edx, ecx
00489eff  sar        eax, 5
00489f02  and        edx, 0x8000001f
00489f08  jns        0x489f0f

// block 00489f0a
00489f0a  dec        edx
00489f0b  or         edx, 0xffffffe0
00489f0e  inc        edx

// block 00489f0f
00489f0f  and        dword ptr [ebp - 0xc], 0
00489f13  and        dword ptr [ebp + 8], 0
00489f17  or         edi, 0xffffffff
00489f1a  mov        ecx, edx
00489f1c  shl        edi, cl
00489f1e  mov        dword ptr [ebp - 4], 0x20
00489f25  sub        dword ptr [ebp - 4], edx
00489f28  not        edi

// block 00489f2a
00489f2a  mov        ebx, dword ptr [ebp + 8]
00489f2d  lea        ebx, [ebp + ebx*4 - 0x20]
00489f31  mov        esi, dword ptr [ebx]
00489f33  mov        ecx, esi
00489f35  and        ecx, edi
00489f37  mov        dword ptr [ebp - 0x10], ecx
00489f3a  mov        ecx, edx
00489f3c  shr        esi, cl
00489f3e  mov        ecx, dword ptr [ebp - 4]
00489f41  or         esi, dword ptr [ebp - 0xc]
00489f44  mov        dword ptr [ebx], esi
00489f46  mov        esi, dword ptr [ebp - 0x10]
00489f49  shl        esi, cl
00489f4b  inc        dword ptr [ebp + 8]
00489f4e  cmp        dword ptr [ebp + 8], 3
00489f52  mov        dword ptr [ebp - 0xc], esi
00489f55  jl         0x489f2a

// block 00489f57
00489f57  mov        esi, eax
00489f59  push       2
00489f5b  shl        esi, 2
00489f5e  lea        ecx, [ebp - 0x18]
00489f61  pop        edx
00489f62  sub        ecx, esi

// block 00489f64
00489f64  cmp        edx, eax
00489f66  jl         0x489f70

// block 00489f68
00489f68  mov        esi, dword ptr [ecx]
00489f6a  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489f6e  jmp        0x489f75

// block 00489f70
00489f70  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489f75
00489f75  dec        edx
00489f76  sub        ecx, 4
00489f79  test       edx, edx
00489f7b  jge        0x489f64

// block 00489f7d
00489f7d  mov        eax, dword ptr [0x4adfc8]
00489f82  mov        ecx, dword ptr [0x4adfdc]
00489f88  lea        ebx, [ecx + eax]
00489f8b  xor        eax, eax
00489f8d  inc        eax
00489f8e  jmp        0x48a02e

// block 00489f93
00489f93  mov        eax, dword ptr [0x4adfdc]
00489f98  and        dword ptr [ebp - 0x20], 0x7fffffff
00489f9f  add        ebx, eax
00489fa1  mov        eax, ecx
00489fa3  cdq        
00489fa4  and        edx, 0x1f
00489fa7  add        eax, edx
00489fa9  mov        edx, ecx
00489fab  sar        eax, 5
00489fae  and        edx, 0x8000001f
00489fb4  jns        0x489fbb

// block 00489fb6
00489fb6  dec        edx
00489fb7  or         edx, 0xffffffe0
00489fba  inc        edx

// block 00489fbb
00489fbb  and        dword ptr [ebp - 0xc], 0
00489fbf  and        dword ptr [ebp + 8], 0
00489fc3  or         esi, 0xffffffff
00489fc6  mov        ecx, edx
00489fc8  shl        esi, cl
00489fca  mov        dword ptr [ebp - 4], 0x20
00489fd1  sub        dword ptr [ebp - 4], edx
00489fd4  not        esi

// block 00489fd6
00489fd6  mov        ecx, dword ptr [ebp + 8]
00489fd9  mov        edi, dword ptr [ebp + ecx*4 - 0x20]
00489fdd  mov        ecx, edi
00489fdf  and        ecx, esi
00489fe1  mov        dword ptr [ebp - 0x10], ecx
00489fe4  mov        ecx, edx
00489fe6  shr        edi, cl
00489fe8  mov        ecx, dword ptr [ebp + 8]
00489feb  or         edi, dword ptr [ebp - 0xc]
00489fee  mov        dword ptr [ebp + ecx*4 - 0x20], edi
00489ff2  mov        edi, dword ptr [ebp - 0x10]
00489ff5  mov        ecx, dword ptr [ebp - 4]
00489ff8  shl        edi, cl
00489ffa  inc        dword ptr [ebp + 8]
00489ffd  cmp        dword ptr [ebp + 8], 3
0048a001  mov        dword ptr [ebp - 0xc], edi
0048a004  jl         0x489fd6

// block 0048a006
0048a006  mov        esi, eax
0048a008  push       2
0048a00a  shl        esi, 2
0048a00d  lea        ecx, [ebp - 0x18]
0048a010  pop        edx
0048a011  sub        ecx, esi

// block 0048a013
0048a013  cmp        edx, eax
0048a015  jl         0x48a01f

// block 0048a017
0048a017  mov        esi, dword ptr [ecx]
0048a019  mov        dword ptr [ebp + edx*4 - 0x20], esi
0048a01d  jmp        0x48a024

// block 0048a01f
0048a01f  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 0048a024
0048a024  dec        edx
0048a025  sub        ecx, 4
0048a028  test       edx, edx
0048a02a  jge        0x48a013

// block 0048a02c
0048a02c  xor        eax, eax

// block 0048a02e
0048a02e  pop        esi

// block 0048a02f
0048a02f  push       0x1f
0048a031  pop        ecx
0048a032  sub        ecx, dword ptr [0x4adfd4]
0048a038  shl        ebx, cl
0048a03a  mov        ecx, dword ptr [ebp - 0x14]
0048a03d  neg        ecx
0048a03f  sbb        ecx, ecx
0048a041  and        ecx, 0x80000000
0048a047  or         ebx, ecx
0048a049  mov        ecx, dword ptr [0x4adfd8]
0048a04f  or         ebx, dword ptr [ebp - 0x20]
0048a052  cmp        ecx, 0x40
0048a055  jne        0x48a064

// block 0048a057
0048a057  mov        ecx, dword ptr [ebp + 0xc]
0048a05a  mov        edx, dword ptr [ebp - 0x1c]
0048a05d  mov        dword ptr [ecx + 4], ebx
0048a060  mov        dword ptr [ecx], edx
0048a062  jmp        0x48a06e

// block 0048a064
0048a064  cmp        ecx, 0x20
0048a067  jne        0x48a06e

// block 0048a069
0048a069  mov        ecx, dword ptr [ebp + 0xc]
0048a06c  mov        dword ptr [ecx], ebx

// block 0048a06e
0048a06e  pop        edi
0048a06f  pop        ebx
0048a070  leave      
0048a071  ret        
