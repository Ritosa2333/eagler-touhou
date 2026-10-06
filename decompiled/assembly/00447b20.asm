
// block 00447b20
00447b20  sub        esp, 0xb8
00447b26  mov        eax, dword ptr [0x4ad138]
00447b2b  xor        eax, esp
00447b2d  mov        dword ptr [esp + 0xb4], eax
00447b34  mov        eax, dword ptr [ecx + 0x1d8]
00447b3a  push       ebx
00447b3b  push       ebp
00447b3c  push       esi
00447b3d  lea        eax, [eax + eax*4]
00447b40  push       edi
00447b41  lea        eax, [eax + eax - 0xa]
00447b45  xor        edi, edi
00447b47  xor        edx, edx
00447b49  mov        dword ptr [esp + 0x18], ecx
00447b4d  test       eax, eax
00447b4f  jle        0x447b75

// block 00447b51
00447b51  mov        eax, dword ptr [ecx + 0x1d8]
00447b57  mov        esi, dword ptr [ecx + 0x100]
00447b5d  lea        eax, [eax + eax*4]
00447b60  lea        eax, [eax + eax - 0xa]

// block 00447b64
00447b64  movsx      ebx, byte ptr [edi + 0x4af208]
00447b6b  cmp        ebx, esi
00447b6d  jne        0x447b70

// block 00447b6f
00447b6f  inc        edx

// block 00447b70
00447b70  inc        edi
00447b71  cmp        edx, eax
00447b73  jl         0x447b64

// block 00447b75
00447b75  xor        edx, edx
00447b77  lea        eax, [ecx + 0x670]
00447b7d  mov        dword ptr [ecx + 0x2b0], 0
00447b87  mov        dword ptr [esp + 0x14], edx
00447b8b  mov        dword ptr [esp + 0x10], eax
00447b8f  nop        

// block 00447b90
00447b90  cmp        edi, 0x71
00447b93  jge        0x447bb1

// block 00447b95
00447b95  mov        eax, dword ptr [ecx + 0x100]
00447b9b  jmp        0x447ba0

// block 00447ba0
00447ba0  movsx      esi, byte ptr [edi + 0x4af208]
00447ba7  cmp        esi, eax
00447ba9  je         0x447be1

// block 00447bab
00447bab  inc        edi
00447bac  cmp        edi, 0x71
00447baf  jl         0x447ba0

// block 00447bb1
00447bb1  cmp        edx, 0xa
00447bb4  jge        0x447df3

// block 00447bba
00447bba  mov        ebx, 0xa
00447bbf  lea        edi, [ecx + edx*4 + 0x670]
00447bc6  sub        ebx, edx
00447bc8  jmp        0x447bd0

// block 00447bd0
00447bd0  mov        ecx, dword ptr [edi]
00447bd2  test       ecx, ecx
00447bd4  jne        0x447d86

// block 00447bda
00447bda  xor        eax, eax
00447bdc  jmp        0x447dca

// block 00447be1
00447be1  cmp        edi, 0x71
00447be4  jge        0x447bb1

// block 00447be6
00447be6  mov        ebp, dword ptr [0x4b451c]
00447bec  lea        ebx, [edi + edi*8]
00447bef  shl        ebx, 4
00447bf2  cmp        dword ptr [ebx + ebp + 0x1aaa8], 0
00447bfa  je         0x447d09

// block 00447c00
00447c00  lea        eax, [ebx + ebp + 0x1aa24]
00447c07  lea        edx, [esp + 0x1c]
00447c0b  sub        edx, eax
00447c0d  lea        ecx, [ecx]

// block 00447c10
00447c10  mov        cl, byte ptr [eax]
00447c12  mov        byte ptr [edx + eax], cl
00447c15  inc        eax
00447c16  test       cl, cl
00447c18  jne        0x447c10

// block 00447c1a
00447c1a  lea        esi, [esp + 0x1c]
00447c1e  lea        ecx, [esi + 1]

// block 00447c21
00447c21  mov        al, byte ptr [esi]
00447c23  inc        esi
00447c24  test       al, al
00447c26  jne        0x447c21

// block 00447c28
00447c28  sub        esi, ecx
00447c2a  cmp        esi, 0x2a
00447c2d  jge        0x447c4e

// block 00447c2f
00447c2f  mov        ebp, 0x2a
00447c34  sub        ebp, esi
00447c36  push       ebp
00447c37  lea        eax, [esp + esi + 0x20]
00447c3b  push       0x20
00447c3d  push       eax
00447c3e  call       0x477420

// block 00447c43
00447c43  add        esp, 0xc
00447c46  add        esi, ebp
00447c48  mov        ebp, dword ptr [0x4b451c]

// block 00447c4e
00447c4e  mov        ecx, dword ptr [esp + 0x18]
00447c52  mov        edx, dword ptr [ecx + 0x28]
00447c55  mov        eax, dword ptr [esp + 0x10]
00447c59  imul       edx, edx, 0x45f4
00447c5f  lea        ecx, [edx + ebp + 8]
00447c63  mov        edx, dword ptr [eax]
00447c65  mov        byte ptr [esp + esi + 0x1c], 0
00447c6a  test       edx, edx
00447c6c  jne        0x447c72

// block 00447c6e
00447c6e  xor        eax, eax
00447c70  jmp        0x447cb9

// block 00447c72
00447c72  mov        esi, dword ptr [0x4ce8cc]
00447c78  mov        eax, dword ptr [esi + 0x8856b8]
00447c7e  test       eax, eax
00447c80  je         0x447c90

// block 00447c82
00447c82  mov        ebp, dword ptr [eax]
00447c84  cmp        dword ptr [ebp], edx
00447c87  je         0x447caf

// block 00447c89
00447c89  mov        eax, dword ptr [eax + 4]
00447c8c  test       eax, eax
00447c8e  jne        0x447c82

// block 00447c90
00447c90  mov        eax, dword ptr [esi + 0x8856c0]
00447c96  test       eax, eax
00447c98  je         0x447c6e

// block 00447c9a
00447c9a  lea        ebx, [ebx]

// block 00447ca0
00447ca0  mov        esi, dword ptr [eax]
00447ca2  cmp        dword ptr [esi], edx
00447ca4  je         0x447cb3

// block 00447ca6
00447ca6  mov        eax, dword ptr [eax + 4]
00447ca9  test       eax, eax
00447cab  jne        0x447ca0

// block 00447cad
00447cad  jmp        0x447cb9

// block 00447caf
00447caf  mov        eax, ebp
00447cb1  jmp        0x447cb5

// block 00447cb3
00447cb3  mov        eax, esi

// block 00447cb5
00447cb5  test       eax, eax
00447cb7  jne        0x447cc3

// block 00447cb9
00447cb9  mov        edx, dword ptr [esp + 0x10]
00447cbd  mov        dword ptr [edx], 0

// block 00447cc3
00447cc3  mov        edx, dword ptr [ebx + ecx + 0x6e8]
00447cca  push       edx
00447ccb  mov        edx, dword ptr [ebx + ecx + 0x6e4]
00447cd2  mov        ecx, dword ptr [ebx + ecx + 0x6e4]
00447cd9  push       edx
00447cda  lea        edx, [esp + 0x24]
00447cde  push       edx
00447cdf  inc        edi
00447ce0  push       edi
00447ce1  push       0x4a1fc4
00447ce6  neg        ecx
00447ce8  push       0
00447cea  sbb        ecx, ecx
00447cec  push       0
00447cee  and        ecx, 0x100f91
00447cf4  add        ecx, 0xefefef
00447cfa  push       0
00447cfc  push       ecx
00447cfd  mov        esi, eax
00447cff  call       0x4608f0

// block 00447d04
00447d04  add        esp, 0x24
00447d07  jmp        0x447d5e

// block 00447d09
00447d09  mov        edx, dword ptr [ecx + 0x28]
00447d0c  mov        eax, dword ptr [esp + 0x10]
00447d10  imul       edx, edx, 0x45f4
00447d16  mov        ecx, dword ptr [eax]
00447d18  lea        esi, [edx + ebp + 8]
00447d1c  mov        edx, dword ptr [0x4ce8cc]
00447d22  push       ecx
00447d23  call       0x461920

// block 00447d28
00447d28  test       eax, eax
00447d2a  jne        0x447d32

// block 00447d2c
00447d2c  mov        edx, dword ptr [esp + 0x10]
00447d30  mov        dword ptr [edx], eax

// block 00447d32
00447d32  mov        ecx, dword ptr [ebx + esi + 0x6e8]
00447d39  mov        edx, dword ptr [ebx + esi + 0x6e4]
00447d40  push       ecx
00447d41  push       edx
00447d42  inc        edi
00447d43  push       edi
00447d44  push       0x4a1fd8
00447d49  push       0
00447d4b  push       0
00447d4d  push       0
00447d4f  push       0x808080
00447d54  mov        esi, eax
00447d56  call       0x4608f0

// block 00447d5b
00447d5b  add        esp, 0x20

// block 00447d5e
00447d5e  mov        eax, dword ptr [esp + 0x18]
00447d62  inc        dword ptr [eax + 0x2b0]
00447d68  mov        eax, dword ptr [esp + 0x14]
00447d6c  add        dword ptr [esp + 0x10], 4
00447d71  inc        eax
00447d72  cmp        eax, 0xa
00447d75  mov        dword ptr [esp + 0x14], eax
00447d79  jge        0x447df3

// block 00447d7b
00447d7b  mov        ecx, dword ptr [esp + 0x18]
00447d7f  mov        edx, eax
00447d81  jmp        0x447b90

// block 00447d86
00447d86  mov        edx, dword ptr [0x4ce8cc]
00447d8c  mov        eax, dword ptr [edx + 0x8856b8]
00447d92  test       eax, eax
00447d94  je         0x447da3

// block 00447d96
00447d96  mov        esi, dword ptr [eax]
00447d98  cmp        dword ptr [esi], ecx
00447d9a  je         0x447dc0

// block 00447d9c
00447d9c  mov        eax, dword ptr [eax + 4]
00447d9f  test       eax, eax
00447da1  jne        0x447d96

// block 00447da3
00447da3  mov        eax, dword ptr [edx + 0x8856c0]
00447da9  test       eax, eax
00447dab  je         0x447bda

// block 00447db1
00447db1  mov        edx, dword ptr [eax]
00447db3  cmp        dword ptr [edx], ecx
00447db5  je         0x447dc4

// block 00447db7
00447db7  mov        eax, dword ptr [eax + 4]
00447dba  test       eax, eax
00447dbc  jne        0x447db1

// block 00447dbe
00447dbe  jmp        0x447dca

// block 00447dc0
00447dc0  mov        eax, esi
00447dc2  jmp        0x447dc6

// block 00447dc4
00447dc4  mov        eax, edx

// block 00447dc6
00447dc6  test       eax, eax
00447dc8  jne        0x447dd0

// block 00447dca
00447dca  mov        dword ptr [edi], 0

// block 00447dd0
00447dd0  push       0x49fb38
00447dd5  push       0
00447dd7  push       0
00447dd9  push       0
00447ddb  push       -1
00447ddd  mov        esi, eax
00447ddf  call       0x4608f0

// block 00447de4
00447de4  add        esp, 0x14
00447de7  add        edi, 4
00447dea  sub        ebx, 1
00447ded  jne        0x447bd0

// block 00447df3
00447df3  mov        ecx, dword ptr [esp + 0xc4]
00447dfa  pop        edi
00447dfb  pop        esi
00447dfc  pop        ebp
00447dfd  pop        ebx
00447dfe  xor        ecx, esp
00447e00  xor        eax, eax
00447e02  call       0x46c932

// block 00447e07
00447e07  add        esp, 0xb8
00447e0d  ret        
