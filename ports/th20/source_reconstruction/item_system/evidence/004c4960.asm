004c4960 55                       push       ebp
004c4961 8bec                     mov        ebp, esp
004c4963 83ec28                   sub        esp, 0x28
004c4966 56                       push       esi
004c4967 57                       push       edi
004c4968 894df8                   mov        dword ptr [ebp - 8], ecx
004c496b 6a00                     push       0
004c496d e80ef7f9ff               call       0x464080
004c4972 83c404                   add        esp, 4
004c4975 8bc8                     mov        ecx, eax
004c4977 e8b4010000               call       0x4c4b30
004c497c 99                       cdq        
004c497d 2bc2                     sub        eax, edx
004c497f 8bf0                     mov        esi, eax
004c4981 d1fe                     sar        esi, 1
004c4983 6a00                     push       0
004c4985 e8f6f6f9ff               call       0x464080
004c498a 83c404                   add        esp, 4
004c498d 8bc8                     mov        ecx, eax
004c498f e82c39ffff               call       0x4b82c0
004c4994 8bf8                     mov        edi, eax
004c4996 6a00                     push       0
004c4998 e8e3f6f9ff               call       0x464080
004c499d 83c404                   add        esp, 4
004c49a0 8bc8                     mov        ecx, eax
004c49a2 e889010000               call       0x4c4b30
004c49a7 0faff8                   imul       edi, eax
004c49aa 6a00                     push       0
004c49ac e8cff6f9ff               call       0x464080
004c49b1 83c404                   add        esp, 4
004c49b4 8bc8                     mov        ecx, eax
004c49b6 e84539ffff               call       0x4b8300
004c49bb 8bc8                     mov        ecx, eax
004c49bd 8bc7                     mov        eax, edi
004c49bf 99                       cdq        
004c49c0 f7f9                     idiv       ecx
004c49c2 03f0                     add        esi, eax
004c49c4 8975f4                   mov        dword ptr [ebp - 0xc], esi
004c49c7 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c49ca f30f1082e80b0000         movss      xmm0, dword ptr [edx + 0xbe8]
004c49d2 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004c49d7 6a00                     push       0
004c49d9 e852bef9ff               call       0x460830
004c49de 83c404                   add        esp, 4
004c49e1 8bc8                     mov        ecx, eax
004c49e3 e86836ffff               call       0x4b8050
004c49e8 d95dec                   fstp       dword ptr [ebp - 0x14]
004c49eb f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004c49f0 0f2f45f0                 comiss     xmm0, dword ptr [ebp - 0x10]
004c49f4 730c                     jae        0x4c4a02
004c49f6 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c49f9 83b8240c000003           cmp        dword ptr [eax + 0xc24], 3
004c4a00 7554                     jne        0x4c4a56
004c4a02 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c4a05 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4a08 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4a0b 99                       cdq        
004c4a0c b90a000000               mov        ecx, 0xa
004c4a11 f7f9                     idiv       ecx
004c4a13 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4a16 2bc2                     sub        eax, edx
004c4a18 8945fc                   mov        dword ptr [ebp - 4], eax
004c4a1b 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c4a1f 7f07                     jg         0x4c4a28
004c4a21 c745fc0a000000           mov        dword ptr [ebp - 4], 0xa
004c4a28 6a00                     push       0
004c4a2a e8718bffff               call       0x4bd5a0
004c4a2f 83c404                   add        esp, 4
004c4a32 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c4a35 6800ffffff               push       0xffffff00
004c4a3a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4a3d 51                       push       ecx
004c4a3e 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4a41 81c2e40b0000             add        edx, 0xbe4
004c4a47 52                       push       edx
004c4a48 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c4a4b e8c0bc0400               call       0x510710
004c4a50 90                       nop        
004c4a51 e996000000               jmp        0x4c4aec
004c4a56 6b45f409                 imul       eax, dword ptr [ebp - 0xc], 9
004c4a5a 99                       cdq        
004c4a5b b90a000000               mov        ecx, 0xa
004c4a60 f7f9                     idiv       ecx
004c4a62 8bf0                     mov        esi, eax
004c4a64 6b45f409                 imul       eax, dword ptr [ebp - 0xc], 9
004c4a68 99                       cdq        
004c4a69 b90a000000               mov        ecx, 0xa
004c4a6e f7f9                     idiv       ecx
004c4a70 8bf8                     mov        edi, eax
004c4a72 6a00                     push       0
004c4a74 e8b7bdf9ff               call       0x460830
004c4a79 83c404                   add        esp, 4
004c4a7c 8bc8                     mov        ecx, eax
004c4a7e e8cd35ffff               call       0x4b8050
004c4a83 d95de4                   fstp       dword ptr [ebp - 0x1c]
004c4a86 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c4a8b f30f5c45e4               subss      xmm0, dword ptr [ebp - 0x1c]
004c4a90 f30f2cd0                 cvttss2si  edx, xmm0
004c4a94 0faffa                   imul       edi, edx
004c4a97 8bc7                     mov        eax, edi
004c4a99 99                       cdq        
004c4a9a b9c2010000               mov        ecx, 0x1c2
004c4a9f f7f9                     idiv       ecx
004c4aa1 2bf0                     sub        esi, eax
004c4aa3 8975fc                   mov        dword ptr [ebp - 4], esi
004c4aa6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4aa9 99                       cdq        
004c4aaa b90a000000               mov        ecx, 0xa
004c4aaf f7f9                     idiv       ecx
004c4ab1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4ab4 2bc2                     sub        eax, edx
004c4ab6 8945fc                   mov        dword ptr [ebp - 4], eax
004c4ab9 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c4abd 7f07                     jg         0x4c4ac6
004c4abf c745fc0a000000           mov        dword ptr [ebp - 4], 0xa
004c4ac6 6a00                     push       0
004c4ac8 e8d38affff               call       0x4bd5a0
004c4acd 83c404                   add        esp, 4
004c4ad0 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c4ad3 6aff                     push       -1
004c4ad5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4ad8 51                       push       ecx
004c4ad9 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4adc 81c2e40b0000             add        edx, 0xbe4
004c4ae2 52                       push       edx
004c4ae3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c4ae6 e825bc0400               call       0x510710
004c4aeb 90                       nop        
004c4aec 6a00                     push       0
004c4aee e88df5f9ff               call       0x464080
004c4af3 83c404                   add        esp, 4
004c4af6 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c4af9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4afc 99                       cdq        
004c4afd 52                       push       edx
004c4afe 50                       push       eax
004c4aff 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c4b02 e8a9c90100               call       0x4e14b0
004c4b07 6a00                     push       0
004c4b09 e872f5f9ff               call       0x464080
004c4b0e 83c404                   add        esp, 4
004c4b11 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c4b14 6a01                     push       1
004c4b16 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c4b19 e882fbffff               call       0x4c46a0
004c4b1e 90                       nop        
004c4b1f 5f                       pop        edi
004c4b20 5e                       pop        esi
004c4b21 8be5                     mov        esp, ebp
004c4b23 5d                       pop        ebp
004c4b24 c3                       ret        
004c4b25 cc                       int3       
004c4b26 cc                       int3       
004c4b27 cc                       int3       
004c4b28 cc                       int3       
004c4b29 cc                       int3       
004c4b2a cc                       int3       
004c4b2b cc                       int3       
004c4b2c cc                       int3       
004c4b2d cc                       int3       
004c4b2e cc                       int3       
004c4b2f cc                       int3       
