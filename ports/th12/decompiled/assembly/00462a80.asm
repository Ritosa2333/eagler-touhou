
// block 00462a80
00462a80  push       ebp
00462a81  mov        ebp, esp
00462a83  and        esp, 0xfffffff8
00462a86  sub        esp, 0x14c
00462a8c  mov        eax, dword ptr [0x4ad138]
00462a91  xor        eax, esp
00462a93  mov        dword ptr [esp + 0x148], eax
00462a9a  test       dword ptr [0x4cee78], 0x800
00462aa4  push       ebx
00462aa5  push       esi
00462aa6  push       edi
00462aa7  mov        esi, ecx
00462aa9  jne        0x462c0f

// block 00462aaf
00462aaf  push       0x34
00462ab1  lea        eax, [esp + 0x10]
00462ab5  push       0
00462ab7  push       eax
00462ab8  call       0x477420

// block 00462abd
00462abd  add        esp, 0xc
00462ac0  lea        ecx, [esp + 0xc]
00462ac4  push       ecx
00462ac5  push       0
00462ac7  mov        dword ptr [esp + 0x14], 0x34
00462acf  mov        dword ptr [esp + 0x18], 0xff
00462ad7  call       dword ptr [0x49829c]

// block 00462add
00462add  test       eax, eax
00462adf  je         0x462af8

// block 00462ae1
00462ae1  mov        eax, esi
00462ae3  pop        edi
00462ae4  pop        esi
00462ae5  pop        ebx
00462ae6  mov        ecx, dword ptr [esp + 0x148]
00462aed  xor        ecx, esp
00462aef  call       0x46c932

// block 00462af4
00462af4  mov        esp, ebp
00462af6  pop        ebp
00462af7  ret        

// block 00462af8
00462af8  mov        ecx, dword ptr [0x4d49ec]
00462afe  mov        eax, dword ptr [esp + 0x2c]
00462b02  test       cx, cx
00462b05  jl         0x462b18

// block 00462b07
00462b07  mov        edx, 1
00462b0c  shl        edx, cl
00462b0e  and        edx, eax
00462b10  neg        edx
00462b12  sbb        edx, edx
00462b14  neg        edx
00462b16  or         esi, edx

// block 00462b18
00462b18  mov        cx, word ptr [0x4d49ee]
00462b1f  test       cx, cx
00462b22  jl         0x462b36

// block 00462b24
00462b24  mov        edx, 1
00462b29  shl        edx, cl
00462b2b  and        edx, eax
00462b2d  neg        edx
00462b2f  sbb        edx, edx
00462b31  and        edx, 2
00462b34  or         esi, edx

// block 00462b36
00462b36  mov        cx, word ptr [0x4d49f2]
00462b3d  test       cx, cx
00462b40  jl         0x462b57

// block 00462b42
00462b42  mov        edx, 1
00462b47  shl        edx, cl
00462b49  and        edx, eax
00462b4b  neg        edx
00462b4d  sbb        edx, edx
00462b4f  and        edx, 0x100
00462b55  or         esi, edx

// block 00462b57
00462b57  mov        ecx, dword ptr [0x4d49f0]
00462b5d  test       cx, cx
00462b60  jl         0x462b74

// block 00462b62
00462b62  mov        edx, 1
00462b67  shl        edx, cl
00462b69  and        edx, eax
00462b6b  neg        edx
00462b6d  sbb        edx, edx
00462b6f  and        edx, 8
00462b72  or         esi, edx

// block 00462b74
00462b74  mov        ecx, dword ptr [0x4d49fc]
00462b7a  test       cx, cx
00462b7d  jl         0x462b94

// block 00462b7f
00462b7f  mov        edx, 1
00462b84  shl        edx, cl
00462b86  and        edx, eax
00462b88  neg        edx
00462b8a  sbb        edx, edx
00462b8c  and        edx, 0x200
00462b92  or         esi, edx

// block 00462b94
00462b94  mov        ecx, dword ptr [0x4ce598]
00462b9a  mov        edx, dword ptr [0x4ce594]
00462ba0  mov        eax, ecx
00462ba2  sub        eax, edx
00462ba4  add        ecx, edx
00462ba6  mov        edx, dword ptr [esp + 0x14]
00462baa  shr        eax, 2
00462bad  shr        ecx, 1
00462baf  lea        edi, [ecx + eax]
00462bb2  cmp        edi, edx
00462bb4  sbb        edi, edi
00462bb6  sub        ecx, eax
00462bb8  and        edi, 0x80
00462bbe  cmp        edx, ecx
00462bc0  mov        edx, dword ptr [0x4ce59c]
00462bc6  sbb        eax, eax
00462bc8  and        eax, 0x40
00462bcb  or         edi, eax
00462bcd  mov        eax, dword ptr [0x4ce5a0]
00462bd2  mov        ecx, eax
00462bd4  sub        ecx, edx
00462bd6  add        edx, eax
00462bd8  shr        ecx, 2
00462bdb  shr        edx, 1
00462bdd  or         esi, edi
00462bdf  mov        edi, dword ptr [esp + 0x18]
00462be3  lea        eax, [edx + ecx]
00462be6  cmp        eax, edi
00462be8  sbb        eax, eax
00462bea  sub        edx, ecx
00462bec  and        eax, 0x20
00462bef  cmp        edi, edx
00462bf1  sbb        ecx, ecx
00462bf3  and        ecx, 0x10
00462bf6  or         eax, ecx
00462bf8  or         eax, esi
00462bfa  pop        edi
00462bfb  pop        esi
00462bfc  pop        ebx
00462bfd  mov        ecx, dword ptr [esp + 0x148]
00462c04  xor        ecx, esp
00462c06  call       0x46c932

// block 00462c0b
00462c0b  mov        esp, ebp
00462c0d  pop        ebp
00462c0e  ret        

// block 00462c0f
00462c0f  mov        eax, dword ptr [0x4ce90c]
00462c14  mov        edx, dword ptr [eax]
00462c16  push       eax
00462c17  mov        eax, dword ptr [edx + 0x64]
00462c1a  call       eax

// block 00462c1c
00462c1c  test       eax, eax
00462c1e  jge        0x462c86

// block 00462c20
00462c20  mov        eax, dword ptr [0x4ce90c]
00462c25  mov        ecx, dword ptr [eax]
00462c27  mov        edx, dword ptr [ecx + 0x1c]
00462c2a  push       eax
00462c2b  xor        edi, edi
00462c2d  call       edx

// block 00462c2f
00462c2f  cmp        eax, 0x8007001e
00462c34  jne        0x462ae1

// block 00462c3a
00462c3a  lea        ebx, [ebx]

// block 00462c40
00462c40  mov        eax, dword ptr [0x4ce90c]
00462c45  mov        ecx, dword ptr [eax]
00462c47  mov        edx, dword ptr [ecx + 0x1c]
00462c4a  push       eax
00462c4b  call       edx

// block 00462c4d
00462c4d  push       edi
00462c4e  push       0x4a35b8
00462c53  call       0x4654b0

// block 00462c58
00462c58  inc        edi
00462c59  add        esp, 8
00462c5c  cmp        edi, 0x190
00462c62  jge        0x462ae1

// block 00462c68
00462c68  cmp        eax, 0x8007001e
00462c6d  je         0x462c40

// block 00462c6f
00462c6f  mov        eax, esi
00462c71  pop        edi
00462c72  pop        esi
00462c73  pop        ebx
00462c74  mov        ecx, dword ptr [esp + 0x148]
00462c7b  xor        ecx, esp
00462c7d  call       0x46c932

// block 00462c82
00462c82  mov        esp, ebp
00462c84  pop        ebp
00462c85  ret        

// block 00462c86
00462c86  push       0x110
00462c8b  lea        eax, [esp + 0x44]
00462c8f  push       0
00462c91  push       eax
00462c92  call       0x477420

// block 00462c97
00462c97  mov        eax, dword ptr [0x4ce90c]
00462c9c  mov        ecx, dword ptr [eax]
00462c9e  add        esp, 0xc
00462ca1  lea        edx, [esp + 0x40]
00462ca5  push       edx
00462ca6  push       0x110
00462cab  push       eax
00462cac  mov        eax, dword ptr [ecx + 0x24]
00462caf  call       eax

// block 00462cb1
00462cb1  test       eax, eax
00462cb3  jl         0x462ae1

// block 00462cb9
00462cb9  mov        eax, dword ptr [0x4d49ec]
00462cbe  test       ax, ax
00462cc1  jl         0x462cd0

// block 00462cc3
00462cc3  movsx      ecx, ax
00462cc6  movzx      edx, byte ptr [esp + ecx + 0x70]
00462ccb  shr        edx, 7
00462cce  or         esi, edx

// block 00462cd0
00462cd0  mov        ax, word ptr [0x4d49ee]
00462cd6  test       ax, ax
00462cd9  jl         0x462ce9

// block 00462cdb
00462cdb  cwde       
00462cdc  movzx      ecx, byte ptr [esp + eax + 0x70]
00462ce1  shr        ecx, 6
00462ce4  and        ecx, 2
00462ce7  or         esi, ecx

// block 00462ce9
00462ce9  mov        ax, word ptr [0x4d49f2]
00462cef  test       ax, ax
00462cf2  jl         0x462d05

// block 00462cf4
00462cf4  movsx      edx, ax
00462cf7  movzx      eax, byte ptr [esp + edx + 0x70]
00462cfc  and        eax, 0x80
00462d01  add        eax, eax
00462d03  or         esi, eax

// block 00462d05
00462d05  mov        eax, dword ptr [0x4d49f0]
00462d0a  test       ax, ax
00462d0d  jl         0x462d1f

// block 00462d0f
00462d0f  movsx      ecx, ax
00462d12  movzx      edx, byte ptr [esp + ecx + 0x70]
00462d17  shr        edx, 4
00462d1a  and        edx, 8
00462d1d  or         esi, edx

// block 00462d1f
00462d1f  mov        eax, dword ptr [0x4d49fc]
00462d24  test       ax, ax
00462d27  jl         0x462d3b

// block 00462d29
00462d29  cwde       
00462d2a  movzx      ecx, byte ptr [esp + eax + 0x70]
00462d2f  and        ecx, 0x80
00462d35  add        ecx, ecx
00462d37  add        ecx, ecx
00462d39  or         esi, ecx

// block 00462d3b
00462d3b  movsx      ecx, word ptr [0x4ceac6]
00462d42  movsx      eax, word ptr [0x4ceac8]
00462d49  neg        ecx
00462d4b  xor        edx, edx
00462d4d  cmp        dword ptr [esp + 0x40], ecx
00462d51  movsx      edi, word ptr [0x4ceac6]
00462d58  setge      dl
00462d5b  xor        ebx, ebx
00462d5d  mov        ecx, eax
00462d5f  neg        ecx
00462d61  dec        edx
00462d62  and        edx, 0x40
00462d65  cmp        dword ptr [esp + 0x44], ecx
00462d69  setge      bl
00462d6c  xor        ecx, ecx
00462d6e  dec        ebx
00462d6f  and        ebx, 0x10
00462d72  or         edx, ebx
00462d74  cmp        dword ptr [esp + 0x40], edi
00462d78  pop        edi
00462d79  setle      cl
00462d7c  dec        ecx
00462d7d  and        ecx, 0x80
00462d83  or         edx, ecx
00462d85  xor        ecx, ecx
00462d87  cmp        dword ptr [esp + 0x40], eax
00462d8b  setle      cl
00462d8e  dec        ecx
00462d8f  and        ecx, 0x20
00462d92  or         edx, ecx
00462d94  mov        ecx, dword ptr [esp + 0x150]
00462d9b  or         edx, esi
00462d9d  pop        esi
00462d9e  pop        ebx
00462d9f  xor        ecx, esp
00462da1  mov        eax, edx
00462da3  call       0x46c932

// block 00462da8
00462da8  mov        esp, ebp
00462daa  pop        ebp
00462dab  ret        
