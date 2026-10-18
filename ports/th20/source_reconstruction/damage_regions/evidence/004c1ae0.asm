004c1ae0 55                       push       ebp
004c1ae1 8bec                     mov        ebp, esp
004c1ae3 83ec08                   sub        esp, 8
004c1ae6 894df8                   mov        dword ptr [ebp - 8], ecx
004c1ae9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c1aec 8b8810c40000             mov        ecx, dword ptr [eax + 0xc410]
004c1af2 51                       push       ecx
004c1af3 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1af6 e8a527f5ff               call       0x4142a0
004c1afb 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c1afe e85df3ffff               call       0x4c0e60
004c1b03 8945fc                   mov        dword ptr [ebp - 4], eax
004c1b06 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c1b0a 7457                     je         0x4c1b63
004c1b0c 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c1b0f 8b8258c40000             mov        eax, dword ptr [edx + 0xc458]
004c1b15 50                       push       eax
004c1b16 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1b19 e8026ffcff               call       0x488a20
004c1b1e 8b4d20                   mov        ecx, dword ptr [ebp + 0x20]
004c1b21 51                       push       ecx
004c1b22 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
004c1b25 52                       push       edx
004c1b26 51                       push       ecx
004c1b27 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004c1b2c f30f110424               movss      dword ptr [esp], xmm0
004c1b31 51                       push       ecx
004c1b32 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c1b37 f30f110424               movss      dword ptr [esp], xmm0
004c1b3c 51                       push       ecx
004c1b3d f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c1b42 f30f110424               movss      dword ptr [esp], xmm0
004c1b47 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004c1b4a 50                       push       eax
004c1b4b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1b4e e8fd000000               call       0x4c1c50
004c1b53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1b56 8b918c000000             mov        edx, dword ptr [ecx + 0x8c]
004c1b5c 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1b5f 8910                     mov        dword ptr [eax], edx
004c1b61 eb0b                     jmp        0x4c1b6e
004c1b63 6a00                     push       0
004c1b65 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1b68 e833fcf4ff               call       0x4117a0
004c1b6d 90                       nop        
004c1b6e 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1b71 8be5                     mov        esp, ebp
004c1b73 5d                       pop        ebp
004c1b74 c21c00                   ret        0x1c
004c1b77 cc                       int3       
004c1b78 cc                       int3       
004c1b79 cc                       int3       
004c1b7a cc                       int3       
004c1b7b cc                       int3       
004c1b7c cc                       int3       
004c1b7d cc                       int3       
004c1b7e cc                       int3       
004c1b7f cc                       int3       
