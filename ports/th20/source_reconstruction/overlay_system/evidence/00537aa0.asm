00537aa0 55                       push       ebp
00537aa1 8bec                     mov        ebp, esp
00537aa3 83ec7c                   sub        esp, 0x7c
00537aa6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00537aab 33c5                     xor        eax, ebp
00537aad 8945fc                   mov        dword ptr [ebp - 4], eax
00537ab0 894db0                   mov        dword ptr [ebp - 0x50], ecx
00537ab3 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
00537aba eb09                     jmp        0x537ac5
00537abc 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00537abf 83c001                   add        eax, 1
00537ac2 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00537ac5 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00537ac8 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
00537acb 0f8d7c010000             jge        0x537c4d
00537ad1 6a00                     push       0
00537ad3 e8588df2ff               call       0x460830
00537ad8 83c404                   add        esp, 4
00537adb 8945a8                   mov        dword ptr [ebp - 0x58], eax
00537ade 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00537ae1 52                       push       edx
00537ae2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00537ae5 e8d6dcffff               call       0x5357c0
00537aea 8bc8                     mov        ecx, eax
00537aec e87f9eeeff               call       0x421970
00537af1 8b08                     mov        ecx, dword ptr [eax]
00537af3 894df0                   mov        dword ptr [ebp - 0x10], ecx
00537af6 8b5004                   mov        edx, dword ptr [eax + 4]
00537af9 8955f4                   mov        dword ptr [ebp - 0xc], edx
00537afc 8b4008                   mov        eax, dword ptr [eax + 8]
00537aff 8945f8                   mov        dword ptr [ebp - 8], eax
00537b02 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00537b05 51                       push       ecx
00537b06 b9a8695c00               mov        ecx, 0x5c69a8
00537b0b e870caedff               call       0x414580
00537b10 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00537b13 6a00                     push       0
00537b15 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00537b18 e81319f1ff               call       0x449430
00537b1d 0fb6d0                   movzx      edx, al
00537b20 85d2                     test       edx, edx
00537b22 0f8499000000             je         0x537bc1
00537b28 8d4db8                   lea        ecx, [ebp - 0x48]
00537b2b e8003ff4ff               call       0x47ba30
00537b30 6a00                     push       0
00537b32 e8f98cf2ff               call       0x460830
00537b37 83c404                   add        esp, 4
00537b3a 8bc8                     mov        ecx, eax
00537b3c e80f8df2ff               call       0x460850
00537b41 8b08                     mov        ecx, dword ptr [eax]
00537b43 894db8                   mov        dword ptr [ebp - 0x48], ecx
00537b46 8b5004                   mov        edx, dword ptr [eax + 4]
00537b49 8955bc                   mov        dword ptr [ebp - 0x44], edx
00537b4c 8b4008                   mov        eax, dword ptr [eax + 8]
00537b4f 8945c0                   mov        dword ptr [ebp - 0x40], eax
00537b52 6a00                     push       0
00537b54 e8c7f9efff               call       0x437520
00537b59 83c404                   add        esp, 4
00537b5c 8945a0                   mov        dword ptr [ebp - 0x60], eax
00537b5f 6a00                     push       0
00537b61 8d4db8                   lea        ecx, [ebp - 0x48]
00537b64 51                       push       ecx
00537b65 6a08                     push       8
00537b67 8d5584                   lea        edx, [ebp - 0x7c]
00537b6a 52                       push       edx
00537b6b 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00537b6e e8fd5ff6ff               call       0x49db70
00537b73 8b00                     mov        eax, dword ptr [eax]
00537b75 89459c                   mov        dword ptr [ebp - 0x64], eax
00537b78 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00537b7b 51                       push       ecx
00537b7c b9a8695c00               mov        ecx, 0x5c69a8
00537b81 e8fac9edff               call       0x414580
00537b86 8b559c                   mov        edx, dword ptr [ebp - 0x64]
00537b89 8910                     mov        dword ptr [eax], edx
00537b8b 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00537b8e 50                       push       eax
00537b8f b9a8695c00               mov        ecx, 0x5c69a8
00537b94 e8e7c9edff               call       0x414580
00537b99 894598                   mov        dword ptr [ebp - 0x68], eax
00537b9c 51                       push       ecx
00537b9d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00537ba5 f30f110424               movss      dword ptr [esp], xmm0
00537baa 51                       push       ecx
00537bab f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00537bb3 f30f110424               movss      dword ptr [esp], xmm0
00537bb8 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00537bbb e8f08af1ff               call       0x4506b0
00537bc0 90                       nop        
00537bc1 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00537bc4 51                       push       ecx
00537bc5 b9a8695c00               mov        ecx, 0x5c69a8
00537bca e8b1c9edff               call       0x414580
00537bcf 894594                   mov        dword ptr [ebp - 0x6c], eax
00537bd2 8d55f0                   lea        edx, [ebp - 0x10]
00537bd5 52                       push       edx
00537bd6 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00537bd9 e802caf2ff               call       0x4645e0
00537bde 90                       nop        
00537bdf 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537be2 83785800                 cmp        dword ptr [eax + 0x58], 0
00537be6 740f                     je         0x537bf7
00537be8 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00537beb 8b5158                   mov        edx, dword ptr [ecx + 0x58]
00537bee 83ea01                   sub        edx, 1
00537bf1 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537bf4 895058                   mov        dword ptr [eax + 0x58], edx
00537bf7 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00537bfa 83795800                 cmp        dword ptr [ecx + 0x58], 0
00537bfe 7548                     jne        0x537c48
00537c00 6a00                     push       0
00537c02 e87912f4ff               call       0x478e80
00537c07 83c404                   add        esp, 4
00537c0a 894590                   mov        dword ptr [ebp - 0x70], eax
00537c0d 6a00                     push       0
00537c0f 689f860100               push       0x1869f
00537c14 6a00                     push       0
00537c16 51                       push       ecx
00537c17 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00537c1f f30f110424               movss      dword ptr [esp], xmm0
00537c24 8d55f0                   lea        edx, [ebp - 0x10]
00537c27 52                       push       edx
00537c28 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00537c2b e83053f4ff               call       0x47cf60
00537c30 894588                   mov        dword ptr [ebp - 0x78], eax
00537c33 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537c36 8b4858                   mov        ecx, dword ptr [eax + 0x58]
00537c39 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00537c3c 8b558c                   mov        edx, dword ptr [ebp - 0x74]
00537c3f 035588                   add        edx, dword ptr [ebp - 0x78]
00537c42 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537c45 895058                   mov        dword ptr [eax + 0x58], edx
00537c48 e96ffeffff               jmp        0x537abc
00537c4d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00537c50 894dac                   mov        dword ptr [ebp - 0x54], ecx
00537c53 eb09                     jmp        0x537c5e
00537c55 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00537c58 83c201                   add        edx, 1
00537c5b 8955ac                   mov        dword ptr [ebp - 0x54], edx
00537c5e 837dac04                 cmp        dword ptr [ebp - 0x54], 4
00537c62 7d2d                     jge        0x537c91
00537c64 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00537c67 50                       push       eax
00537c68 b9a8695c00               mov        ecx, 0x5c69a8
00537c6d e80ec9edff               call       0x414580
00537c72 8bc8                     mov        ecx, eax
00537c74 e85780f1ff               call       0x44fcd0
00537c79 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00537c7c 51                       push       ecx
00537c7d b9bc695c00               mov        ecx, 0x5c69bc
00537c82 e8f9c8edff               call       0x414580
00537c87 8bc8                     mov        ecx, eax
00537c89 e872a2f8ff               call       0x4c1f00
00537c8e 90                       nop        
00537c8f ebc4                     jmp        0x537c55
00537c91 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537c94 33cd                     xor        ecx, ebp
00537c96 e853ad0000               call       0x5429ee
00537c9b 8be5                     mov        esp, ebp
00537c9d 5d                       pop        ebp
00537c9e c20400                   ret        4
00537ca1 cc                       int3       
00537ca2 cc                       int3       
00537ca3 cc                       int3       
00537ca4 cc                       int3       
00537ca5 cc                       int3       
00537ca6 cc                       int3       
00537ca7 cc                       int3       
00537ca8 cc                       int3       
00537ca9 cc                       int3       
00537caa cc                       int3       
00537cab cc                       int3       
00537cac cc                       int3       
00537cad cc                       int3       
00537cae cc                       int3       
00537caf cc                       int3       
