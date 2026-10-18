004bed50 55                       push       ebp
004bed51 8bec                     mov        ebp, esp
004bed53 83ec0c                   sub        esp, 0xc
004bed56 894dfc                   mov        dword ptr [ebp - 4], ecx
004bed59 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bed5c 83780800                 cmp        dword ptr [eax + 8], 0
004bed60 7478                     je         0x4bedda
004bed62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bed65 8b5508                   mov        edx, dword ptr [ebp + 8]
004bed68 3b5108                   cmp        edx, dword ptr [ecx + 8]
004bed6b 7c0e                     jl         0x4bed7b
004bed6d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bed70 8b4808                   mov        ecx, dword ptr [eax + 8]
004bed73 83e901                   sub        ecx, 1
004bed76 894df4                   mov        dword ptr [ebp - 0xc], ecx
004bed79 eb1b                     jmp        0x4bed96
004bed7b 837d0800                 cmp        dword ptr [ebp + 8], 0
004bed7f 7d09                     jge        0x4bed8a
004bed81 c745f800000000           mov        dword ptr [ebp - 8], 0
004bed88 eb06                     jmp        0x4bed90
004bed8a 8b5508                   mov        edx, dword ptr [ebp + 8]
004bed8d 8955f8                   mov        dword ptr [ebp - 8], edx
004bed90 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bed93 8945f4                   mov        dword ptr [ebp - 0xc], eax
004bed96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bed99 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004bed9c 8911                     mov        dword ptr [ecx], edx
004bed9e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004beda1 8b08                     mov        ecx, dword ptr [eax]
004beda3 51                       push       ecx
004beda4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004beda7 e824b7faff               call       0x46a4d0
004bedac 0fb6d0                   movzx      edx, al
004bedaf 85d2                     test       edx, edx
004bedb1 7425                     je         0x4bedd8
004bedb3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bedb6 8b08                     mov        ecx, dword ptr [eax]
004bedb8 83c101                   add        ecx, 1
004bedbb 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bedbe 890a                     mov        dword ptr [edx], ecx
004bedc0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bedc3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bedc6 8b10                     mov        edx, dword ptr [eax]
004bedc8 3b5108                   cmp        edx, dword ptr [ecx + 8]
004bedcb 7c09                     jl         0x4bedd6
004bedcd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bedd0 c70000000000             mov        dword ptr [eax], 0
004bedd6 ebc6                     jmp        0x4bed9e
004bedd8 eb08                     jmp        0x4bede2
004bedda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004beddd 8b5508                   mov        edx, dword ptr [ebp + 8]
004bede0 8911                     mov        dword ptr [ecx], edx
004bede2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bede5 8b00                     mov        eax, dword ptr [eax]
004bede7 8be5                     mov        esp, ebp
004bede9 5d                       pop        ebp
004bedea c20400                   ret        4
004beded cc                       int3       
004bedee cc                       int3       
004bedef cc                       int3       
