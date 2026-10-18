004c1b80 55                       push       ebp
004c1b81 8bec                     mov        ebp, esp
004c1b83 83ec08                   sub        esp, 8
004c1b86 894df8                   mov        dword ptr [ebp - 8], ecx
004c1b89 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c1b8c 8b8810c40000             mov        ecx, dword ptr [eax + 0xc410]
004c1b92 51                       push       ecx
004c1b93 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1b96 e80527f5ff               call       0x4142a0
004c1b9b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c1b9e e8bdf2ffff               call       0x4c0e60
004c1ba3 8945fc                   mov        dword ptr [ebp - 4], eax
004c1ba6 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c1baa 744c                     je         0x4c1bf8
004c1bac 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c1baf 8b8258c40000             mov        eax, dword ptr [edx + 0xc458]
004c1bb5 50                       push       eax
004c1bb6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1bb9 e8626efcff               call       0x488a20
004c1bbe 8b4d1c                   mov        ecx, dword ptr [ebp + 0x1c]
004c1bc1 51                       push       ecx
004c1bc2 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004c1bc5 52                       push       edx
004c1bc6 51                       push       ecx
004c1bc7 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c1bcc f30f110424               movss      dword ptr [esp], xmm0
004c1bd1 51                       push       ecx
004c1bd2 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c1bd7 f30f110424               movss      dword ptr [esp], xmm0
004c1bdc 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004c1bdf 50                       push       eax
004c1be0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1be3 e898010000               call       0x4c1d80
004c1be8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1beb 8b918c000000             mov        edx, dword ptr [ecx + 0x8c]
004c1bf1 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1bf4 8910                     mov        dword ptr [eax], edx
004c1bf6 eb0b                     jmp        0x4c1c03
004c1bf8 6a00                     push       0
004c1bfa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1bfd e89efbf4ff               call       0x4117a0
004c1c02 90                       nop        
004c1c03 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1c06 8be5                     mov        esp, ebp
004c1c08 5d                       pop        ebp
004c1c09 c21800                   ret        0x18
004c1c0c cc                       int3       
004c1c0d cc                       int3       
004c1c0e cc                       int3       
004c1c0f cc                       int3       
