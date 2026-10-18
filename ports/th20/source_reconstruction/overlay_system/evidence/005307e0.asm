005307e0 55                       push       ebp
005307e1 8bec                     mov        ebp, esp
005307e3 83ec40                   sub        esp, 0x40
005307e6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005307eb 33c5                     xor        eax, ebp
005307ed 8945fc                   mov        dword ptr [ebp - 4], eax
005307f0 56                       push       esi
005307f1 894de0                   mov        dword ptr [ebp - 0x20], ecx
005307f4 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005307f7 83c014                   add        eax, 0x14
005307fa 8945d8                   mov        dword ptr [ebp - 0x28], eax
005307fd 6a00                     push       0
005307ff 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00530802 e8892defff               call       0x423590
00530807 0fb6c8                   movzx      ecx, al
0053080a 85c9                     test       ecx, ecx
0053080c 740f                     je         0x53081d
0053080e b801000000               mov        eax, 1
00530813 e9a6010000               jmp        0x5309be
00530818 e98c010000               jmp        0x5309a9
0053081d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00530820 83c138                   add        ecx, 0x38
00530823 e808f2f1ff               call       0x44fa30
00530828 6a00                     push       0
0053082a e85138f3ff               call       0x464080
0053082f 83c404                   add        esp, 4
00530832 8945d0                   mov        dword ptr [ebp - 0x30], eax
00530835 6a00                     push       0
00530837 e84438f3ff               call       0x464080
0053083c 83c404                   add        esp, 4
0053083f 8bc8                     mov        ecx, eax
00530841 e8dab0feff               call       0x51b920
00530846 8bf0                     mov        esi, eax
00530848 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053084b 83c114                   add        ecx, 0x14
0053084e e83df7edff               call       0x40ff90
00530853 0faff0                   imul       esi, eax
00530856 e8053a0000               call       0x534260
0053085b 8bc8                     mov        ecx, eax
0053085d 8bc6                     mov        eax, esi
0053085f 99                       cdq        
00530860 f7f9                     idiv       ecx
00530862 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00530865 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00530868 52                       push       edx
00530869 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0053086c e8cfe0f8ff               call       0x4be940
00530871 6a00                     push       0
00530873 e8b8fff2ff               call       0x460830
00530878 83c404                   add        esp, 4
0053087b 8bc8                     mov        ecx, eax
0053087d e8cefff2ff               call       0x460850
00530882 f30f104004               movss      xmm0, dword ptr [eax + 4]
00530887 f30f5805bcd75600         addss      xmm0, dword ptr [0x56d7bc]
0053088f f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00530894 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
0053089c 0f2f45dc                 comiss     xmm0, dword ptr [ebp - 0x24]
005308a0 760d                     jbe        0x5308af
005308a2 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
005308aa f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
005308af 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005308b2 f30f10403c               movss      xmm0, dword ptr [eax + 0x3c]
005308b7 f30f5c45dc               subss      xmm0, dword ptr [ebp - 0x24]
005308bc f30f59051c5a5700         mulss      xmm0, dword ptr [0x575a1c]
005308c4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005308c7 f30f10493c               movss      xmm1, dword ptr [ecx + 0x3c]
005308cc f30f5cc8                 subss      xmm1, xmm0
005308d0 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005308d3 f30f114a3c               movss      dword ptr [edx + 0x3c], xmm1
005308d8 51                       push       ecx
005308d9 0f57c0                   xorps      xmm0, xmm0
005308dc f30f110424               movss      dword ptr [esp], xmm0
005308e1 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005308e4 51                       push       ecx
005308e5 f30f10403c               movss      xmm0, dword ptr [eax + 0x3c]
005308ea f30f110424               movss      dword ptr [esp], xmm0
005308ef 51                       push       ecx
005308f0 0f57c0                   xorps      xmm0, xmm0
005308f3 f30f110424               movss      dword ptr [esp], xmm0
005308f8 8d4df0                   lea        ecx, [ebp - 0x10]
005308fb e8d024efff               call       0x422dd0
00530900 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00530903 83c138                   add        ecx, 0x38
00530906 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00530909 8d55f0                   lea        edx, [ebp - 0x10]
0053090c 52                       push       edx
0053090d 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00530910 e8cb3cf3ff               call       0x4645e0
00530915 51                       push       ecx
00530916 0f57c0                   xorps      xmm0, xmm0
00530919 f30f110424               movss      dword ptr [esp], xmm0
0053091e 51                       push       ecx
0053091f f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00530927 f30f110424               movss      dword ptr [esp], xmm0
0053092c 51                       push       ecx
0053092d f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00530935 f30f110424               movss      dword ptr [esp], xmm0
0053093a 8d4de4                   lea        ecx, [ebp - 0x1c]
0053093d e88e24efff               call       0x422dd0
00530942 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00530947 f30f580590f05600         addss      xmm0, dword ptr [0x56f090]
0053094f f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00530954 6a00                     push       0
00530956 e82585f4ff               call       0x478e80
0053095b 83c404                   add        esp, 4
0053095e 8945c8                   mov        dword ptr [ebp - 0x38], eax
00530961 6a00                     push       0
00530963 6a00                     push       0
00530965 51                       push       ecx
00530966 0f57c0                   xorps      xmm0, xmm0
00530969 f30f110424               movss      dword ptr [esp], xmm0
0053096e 8d45e4                   lea        eax, [ebp - 0x1c]
00530971 50                       push       eax
00530972 8d4df0                   lea        ecx, [ebp - 0x10]
00530975 51                       push       ecx
00530976 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00530979 e812c3f4ff               call       0x47cc90
0053097e 6a00                     push       0
00530980 e85b85f4ff               call       0x478ee0
00530985 83c404                   add        esp, 4
00530988 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0053098b 6a00                     push       0
0053098d 6a00                     push       0
0053098f 51                       push       ecx
00530990 0f57c0                   xorps      xmm0, xmm0
00530993 f30f110424               movss      dword ptr [esp], xmm0
00530998 8d55e4                   lea        edx, [ebp - 0x1c]
0053099b 52                       push       edx
0053099c 8d45f0                   lea        eax, [ebp - 0x10]
0053099f 50                       push       eax
005309a0 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
005309a3 e80894f9ff               call       0x4c9db0
005309a8 90                       nop        
005309a9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005309ac 83c114                   add        ecx, 0x14
005309af 894dc0                   mov        dword ptr [ebp - 0x40], ecx
005309b2 6a00                     push       0
005309b4 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005309b7 e8648aefff               call       0x429420
005309bc 33c0                     xor        eax, eax
005309be 5e                       pop        esi
005309bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005309c2 33cd                     xor        ecx, ebp
005309c4 e825200100               call       0x5429ee
005309c9 8be5                     mov        esp, ebp
005309cb 5d                       pop        ebp
005309cc c3                       ret        
005309cd cc                       int3       
005309ce cc                       int3       
005309cf cc                       int3       
