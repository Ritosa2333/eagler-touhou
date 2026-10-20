
// block 00431b90
00431b90  mov        eax, dword ptr [0x4b0ca8]
00431b95  push       ebx
00431b96  mov        ecx, eax
00431b98  xor        ebx, ebx
00431b9a  imul       ecx, ecx, 0x118
00431ba0  push       ebp
00431ba1  mov        ebp, dword ptr [esp + 0xc]
00431ba5  push       esi
00431ba6  mov        esi, dword ptr [0x4b0c44]
00431bac  lea        ecx, [ecx + ebp + 0x10]

// block 00431bb0
00431bb0  cmp        dword ptr [ecx], esi
00431bb2  jle        0x431bc6

// block 00431bb4
00431bb4  inc        ebx
00431bb5  add        ecx, 0x1c
00431bb8  cmp        ebx, 0xa
00431bbb  jl         0x431bb0

// block 00431bbd
00431bbd  pop        esi
00431bbe  pop        ebp
00431bbf  or         eax, 0xffffffff
00431bc2  pop        ebx
00431bc3  ret        4

// block 00431bc6
00431bc6  cmp        ebx, 0xa
00431bc9  jl         0x431bd4

// block 00431bcb
00431bcb  pop        esi
00431bcc  pop        ebp
00431bcd  or         eax, 0xffffffff
00431bd0  pop        ebx
00431bd1  ret        4

// block 00431bd4
00431bd4  mov        edx, 9
00431bd9  cmp        ebx, edx
00431bdb  jge        0x431c11

// block 00431bdd
00431bdd  push       edi
00431bde  mov        edi, edi

// block 00431be0
00431be0  lea        eax, [eax + eax*4]
00431be3  lea        eax, [edx + eax*2]
00431be6  lea        ecx, [eax*8]
00431bed  sub        ecx, eax
00431bef  lea        eax, [ebp + ecx*4]
00431bf3  lea        esi, [eax - 0xc]
00431bf6  lea        edi, [eax + 0x10]
00431bf9  mov        ecx, 7
00431bfe  dec        edx
00431bff  cmp        edx, ebx

// block 00431c01
00431c01  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00431c03
00431c03  mov        eax, dword ptr [0x4b0ca8]
00431c08  jg         0x431be0

// block 00431c0a
00431c0a  mov        esi, dword ptr [0x4b0c44]
00431c10  pop        edi

// block 00431c11
00431c11  lea        edx, [eax + eax*4]
00431c14  lea        eax, [ebx + edx*2]
00431c17  lea        ecx, [eax*8]
00431c1e  sub        ecx, eax
00431c20  mov        dword ptr [ebp + ecx*4 + 0x10], esi
00431c24  mov        eax, dword ptr [0x4b0ca8]
00431c29  lea        edx, [eax + eax*4]
00431c2c  lea        eax, [ebx + edx*2]
00431c2f  movzx      edx, byte ptr [0x4b0cc4]
00431c36  lea        ecx, [eax*8]
00431c3d  sub        ecx, eax
00431c3f  mov        byte ptr [ebp + ecx*4 + 0x15], dl
00431c43  mov        eax, dword ptr [0x4b0ca8]
00431c48  movzx      edx, byte ptr [0x4b0cb0]
00431c4f  lea        eax, [eax + eax*4]
00431c52  lea        eax, [ebx + eax*2]
00431c55  lea        ecx, [eax*8]
00431c5c  sub        ecx, eax
00431c5e  mov        byte ptr [ebp + ecx*4 + 0x14], dl
00431c62  mov        eax, dword ptr [0x4b0ca8]
00431c67  lea        eax, [eax + eax*4]
00431c6a  lea        eax, [ebx + eax*2]
00431c6d  lea        ecx, [eax*8]
00431c74  sub        ecx, eax
00431c76  lea        edx, [ebp + ecx*4 + 0x20]
00431c7a  push       edx
00431c7b  call       0x46d5b7

// block 00431c80
00431c80  mov        eax, dword ptr [0x4b0ca8]
00431c85  mov        edx, dword ptr [0x4a0ee4]
00431c8b  lea        eax, [eax + eax*4]
00431c8e  lea        eax, [ebx + eax*2]
00431c91  lea        ecx, [eax*8]
00431c98  sub        ecx, eax
00431c9a  mov        dword ptr [ebp + ecx*4 + 0x16], edx
00431c9e  lea        eax, [ebp + ecx*4 + 0x16]
00431ca2  mov        ecx, dword ptr [0x4a0ee8]
00431ca8  mov        dword ptr [eax + 4], ecx
00431cab  movzx      edx, byte ptr [0x4a0eec]
00431cb2  mov        byte ptr [eax + 8], dl
00431cb5  mov        eax, dword ptr [0x4b43e0]
00431cba  add        esp, 4
00431cbd  pop        esi
00431cbe  fld        qword ptr [eax + 0x24]
00431cc1  fdiv       qword ptr [eax + 0x2c]
00431cc4  mov        eax, dword ptr [0x4b0ca8]
00431cc9  lea        eax, [eax + eax*4]
00431ccc  lea        eax, [ebx + eax*2]
00431ccf  lea        ecx, [eax*8]
00431cd6  sub        ecx, eax
00431cd8  mov        eax, ebx
00431cda  fstp       dword ptr [esp + 0xc]
00431cde  fld        dword ptr [esp + 0xc]
00431ce2  fld        qword ptr [0x4a3ce8]
00431ce8  fmul       st(1), st(0)
00431cea  fsubrp     st(1)
00431cec  fstp       dword ptr [esp + 0xc]
00431cf0  fld        dword ptr [esp + 0xc]
00431cf4  fstp       dword ptr [ebp + ecx*4 + 0x28]
00431cf8  pop        ebp
00431cf9  pop        ebx
00431cfa  ret        4
