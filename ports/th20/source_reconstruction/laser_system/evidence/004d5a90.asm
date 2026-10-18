004d5a90 55                       push       ebp
004d5a91 8bec                     mov        ebp, esp
004d5a93 83ec0c                   sub        esp, 0xc
004d5a96 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d5a99 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d5a9c 0530130000               add        eax, 0x1330
004d5aa1 8945fc                   mov        dword ptr [ebp - 4], eax
004d5aa4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5aa7 833900                   cmp        dword ptr [ecx], 0
004d5aaa 740a                     je         0x4d5ab6
004d5aac 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5aaf 8b02                     mov        eax, dword ptr [edx]
004d5ab1 8945fc                   mov        dword ptr [ebp - 4], eax
004d5ab4 ebee                     jmp        0x4d5aa4
004d5ab6 68a0125700               push       0x5712a0
004d5abb 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d5ac1 e8ca24ffff               call       0x4c7f90
004d5ac6 8945f8                   mov        dword ptr [ebp - 8], eax
004d5ac9 837df800                 cmp        dword ptr [ebp - 8], 0
004d5acd 742c                     je         0x4d5afb
004d5acf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5ad2 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004d5ad7 f30f11410c               movss      dword ptr [ecx + 0xc], xmm0
004d5adc 8b55f8                   mov        edx, dword ptr [ebp - 8]
004d5adf f30f104508               movss      xmm0, dword ptr [ebp + 8]
004d5ae4 f30f114208               movss      dword ptr [edx + 8], xmm0
004d5ae9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d5aec c70000000000             mov        dword ptr [eax], 0
004d5af2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d5af5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5af8 895104                   mov        dword ptr [ecx + 4], edx
004d5afb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d5afe 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d5b01 8908                     mov        dword ptr [eax], ecx
004d5b03 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d5b06 8b02                     mov        eax, dword ptr [edx]
004d5b08 8be5                     mov        esp, ebp
004d5b0a 5d                       pop        ebp
004d5b0b c20400                   ret        4
004d5b0e cc                       int3       
004d5b0f cc                       int3       
