004caad0 55                       push       ebp
004caad1 8bec                     mov        ebp, esp
004caad3 6aff                     push       -1
004caad5 68fd855600               push       0x5685fd
004caada 64a100000000             mov        eax, dword ptr fs:[0]
004caae0 50                       push       eax
004caae1 83ec28                   sub        esp, 0x28
004caae4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004caae9 33c5                     xor        eax, ebp
004caaeb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004caaee 50                       push       eax
004caaef 8d45f4                   lea        eax, [ebp - 0xc]
004caaf2 64a300000000             mov        dword ptr fs:[0], eax
004caaf8 894de0                   mov        dword ptr [ebp - 0x20], ecx
004caafb c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004cab02 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004cab05 83c030                   add        eax, 0x30
004cab08 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cab0b 8b11                     mov        edx, dword ptr [ecx]
004cab0d 8910                     mov        dword ptr [eax], edx
004cab0f 8b5104                   mov        edx, dword ptr [ecx + 4]
004cab12 895004                   mov        dword ptr [eax + 4], edx
004cab15 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cab18 894808                   mov        dword ptr [eax + 8], ecx
004cab1b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004cab1e 83c210                   add        edx, 0x10
004cab21 8955dc                   mov        dword ptr [ebp - 0x24], edx
004cab24 6a08                     push       8
004cab26 8d4de8                   lea        ecx, [ebp - 0x18]
004cab29 e85215f4ff               call       0x40c080
004cab2e 8d45e8                   lea        eax, [ebp - 0x18]
004cab31 50                       push       eax
004cab32 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004cab35 e84677f4ff               call       0x412280
004cab3a c745fc00000000           mov        dword ptr [ebp - 4], 0
004cab41 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004cab44 e8f779f4ff               call       0x412540
004cab49 8945d0                   mov        dword ptr [ebp - 0x30], eax
004cab4c eb09                     jmp        0x4cab57
004cab4e 8d4de8                   lea        ecx, [ebp - 0x18]
004cab51 e8da70f4ff               call       0x411c30
004cab56 90                       nop        
004cab57 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004cab5a 51                       push       ecx
004cab5b 8d4de8                   lea        ecx, [ebp - 0x18]
004cab5e e84d70f4ff               call       0x411bb0
004cab63 0fb6d0                   movzx      edx, al
004cab66 85d2                     test       edx, edx
004cab68 7458                     je         0x4cabc2
004cab6a 8d4de8                   lea        ecx, [ebp - 0x18]
004cab6d e88e17f4ff               call       0x40c300
004cab72 8945d8                   mov        dword ptr [ebp - 0x28], eax
004cab75 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004cab78 e8e345f4ff               call       0x40f160
004cab7d 8b00                     mov        eax, dword ptr [eax]
004cab7f 83782001                 cmp        dword ptr [eax + 0x20], 1
004cab83 743b                     je         0x4cabc0
004cab85 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004cab88 e8d345f4ff               call       0x40f160
004cab8d 8b08                     mov        ecx, dword ptr [eax]
004cab8f 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004cab92 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004cab95 8b02                     mov        eax, dword ptr [edx]
004cab97 8b4830                   mov        ecx, dword ptr [eax + 0x30]
004cab9a 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004cab9d 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004caba0 52                       push       edx
004caba1 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004caba4 50                       push       eax
004caba5 51                       push       ecx
004caba6 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cabab f30f110424               movss      dword ptr [esp], xmm0
004cabb0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cabb3 51                       push       ecx
004cabb4 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004cabb7 ff55cc                   call       dword ptr [ebp - 0x34]
004cabba 0345e4                   add        eax, dword ptr [ebp - 0x1c]
004cabbd 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004cabc0 eb8c                     jmp        0x4cab4e
004cabc2 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004cabc9 8d4de8                   lea        ecx, [ebp - 0x18]
004cabcc e82f6ff4ff               call       0x411b00
004cabd1 90                       nop        
004cabd2 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004cabd5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004cabd8 64890d00000000           mov        dword ptr fs:[0], ecx
004cabdf 59                       pop        ecx
004cabe0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004cabe3 33cd                     xor        ecx, ebp
004cabe5 e8047e0700               call       0x5429ee
004cabea 8be5                     mov        esp, ebp
004cabec 5d                       pop        ebp
004cabed c21000                   ret        0x10
