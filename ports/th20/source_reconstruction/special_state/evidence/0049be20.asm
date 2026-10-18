0049be20 55                       push       ebp
0049be21 8bec                     mov        ebp, esp
0049be23 83ec28                   sub        esp, 0x28
0049be26 894df8                   mov        dword ptr [ebp - 8], ecx
0049be29 8a45fd                   mov        al, byte ptr [ebp - 3]
0049be2c 8845ff                   mov        byte ptr [ebp - 1], al
0049be2f 8a4dff                   mov        cl, byte ptr [ebp - 1]
0049be32 884dfe                   mov        byte ptr [ebp - 2], cl
0049be35 8b55f8                   mov        edx, dword ptr [ebp - 8]
0049be38 83c254                   add        edx, 0x54
0049be3b 8955f4                   mov        dword ptr [ebp - 0xc], edx
0049be3e 8d45dc                   lea        eax, [ebp - 0x24]
0049be41 50                       push       eax
0049be42 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049be45 e856c3ffff               call       0x4981a0
0049be4a 8b08                     mov        ecx, dword ptr [eax]
0049be4c 894df0                   mov        dword ptr [ebp - 0x10], ecx
0049be4f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049be52 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0049be55 8b45f8                   mov        eax, dword ptr [ebp - 8]
0049be58 83c054                   add        eax, 0x54
0049be5b 8945ec                   mov        dword ptr [ebp - 0x14], eax
0049be5e 8d4dd8                   lea        ecx, [ebp - 0x28]
0049be61 51                       push       ecx
0049be62 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0049be65 e8f63efcff               call       0x45fd60
0049be6a 8b10                     mov        edx, dword ptr [eax]
0049be6c 8955e8                   mov        dword ptr [ebp - 0x18], edx
0049be6f 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0049be72 8945e0                   mov        dword ptr [ebp - 0x20], eax
0049be75 0fb64dfe                 movzx      ecx, byte ptr [ebp - 2]
0049be79 51                       push       ecx
0049be7a 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0049be7d 52                       push       edx
0049be7e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0049be81 50                       push       eax
0049be82 e809ebfeff               call       0x48a990
0049be87 83c40c                   add        esp, 0xc
0049be8a 0fb6c8                   movzx      ecx, al
0049be8d 85c9                     test       ecx, ecx
0049be8f 7404                     je         0x49be95
0049be91 b001                     mov        al, 1
0049be93 eb02                     jmp        0x49be97
0049be95 32c0                     xor        al, al
0049be97 8be5                     mov        esp, ebp
0049be99 5d                       pop        ebp
0049be9a c3                       ret        
0049be9b cc                       int3       
0049be9c cc                       int3       
0049be9d cc                       int3       
0049be9e cc                       int3       
0049be9f cc                       int3       
