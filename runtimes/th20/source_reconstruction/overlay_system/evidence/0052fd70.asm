0052fd70 55                       push       ebp
0052fd71 8bec                     mov        ebp, esp
0052fd73 83ec20                   sub        esp, 0x20
0052fd76 56                       push       esi
0052fd77 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fd7a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fd7d 83c014                   add        eax, 0x14
0052fd80 8945f8                   mov        dword ptr [ebp - 8], eax
0052fd83 6a00                     push       0
0052fd85 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052fd88 e80338efff               call       0x423590
0052fd8d 0fb6c8                   movzx      ecx, al
0052fd90 85c9                     test       ecx, ecx
0052fd92 740c                     je         0x52fda0
0052fd94 b801000000               mov        eax, 1
0052fd99 e9d3000000               jmp        0x52fe71
0052fd9e eb4a                     jmp        0x52fdea
0052fda0 6a00                     push       0
0052fda2 e8d942f3ff               call       0x464080
0052fda7 83c404                   add        esp, 4
0052fdaa 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052fdad 6a00                     push       0
0052fdaf e8cc42f3ff               call       0x464080
0052fdb4 83c404                   add        esp, 4
0052fdb7 8bc8                     mov        ecx, eax
0052fdb9 e862bbfeff               call       0x51b920
0052fdbe 8bf0                     mov        esi, eax
0052fdc0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fdc3 83c114                   add        ecx, 0x14
0052fdc6 e8c501eeff               call       0x40ff90
0052fdcb 0faff0                   imul       esi, eax
0052fdce e88d440000               call       0x534260
0052fdd3 8bc8                     mov        ecx, eax
0052fdd5 8bc6                     mov        eax, esi
0052fdd7 99                       cdq        
0052fdd8 f7f9                     idiv       ecx
0052fdda 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052fddd 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0052fde0 52                       push       edx
0052fde1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052fde4 e857ebf8ff               call       0x4be940
0052fde9 90                       nop        
0052fdea 6a00                     push       0
0052fdec e83f0af3ff               call       0x460830
0052fdf1 83c404                   add        esp, 4
0052fdf4 8bc8                     mov        ecx, eax
0052fdf6 e8b5f8fcff               call       0x4ff6b0
0052fdfb 8bc8                     mov        ecx, eax
0052fdfd e84e050000               call       0x530350
0052fe02 85c0                     test       eax, eax
0052fe04 7516                     jne        0x52fe1c
0052fe06 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fe09 83c024                   add        eax, 0x24
0052fe0c 8945ec                   mov        dword ptr [ebp - 0x14], eax
0052fe0f 6a00                     push       0
0052fe11 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052fe14 e82737efff               call       0x423540
0052fe19 90                       nop        
0052fe1a eb0e                     jmp        0x52fe2a
0052fe1c 6a00                     push       0
0052fe1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fe21 83c124                   add        ecx, 0x24
0052fe24 e8f736efff               call       0x423520
0052fe29 90                       nop        
0052fe2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fe2d 83c124                   add        ecx, 0x24
0052fe30 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052fe33 6a1e                     push       0x1e
0052fe35 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052fe38 e82337efff               call       0x423560
0052fe3d 0fb6d0                   movzx      edx, al
0052fe40 85d2                     test       edx, edx
0052fe42 7517                     jne        0x52fe5b
0052fe44 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fe47 83c024                   add        eax, 0x24
0052fe4a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052fe4d 6a04                     push       4
0052fe4f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052fe52 e8e921f4ff               call       0x472040
0052fe57 85c0                     test       eax, eax
0052fe59 7514                     jne        0x52fe6f
0052fe5b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fe5e 83c114                   add        ecx, 0x14
0052fe61 894de0                   mov        dword ptr [ebp - 0x20], ecx
0052fe64 6a00                     push       0
0052fe66 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052fe69 e8b295efff               call       0x429420
0052fe6e 90                       nop        
0052fe6f 33c0                     xor        eax, eax
0052fe71 5e                       pop        esi
0052fe72 8be5                     mov        esp, ebp
0052fe74 5d                       pop        ebp
0052fe75 c3                       ret        
0052fe76 cc                       int3       
0052fe77 cc                       int3       
0052fe78 cc                       int3       
0052fe79 cc                       int3       
0052fe7a cc                       int3       
0052fe7b cc                       int3       
0052fe7c cc                       int3       
0052fe7d cc                       int3       
0052fe7e cc                       int3       
0052fe7f cc                       int3       
