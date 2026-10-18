00531850 55                       push       ebp
00531851 8bec                     mov        ebp, esp
00531853 83ec38                   sub        esp, 0x38
00531856 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053185b 33c5                     xor        eax, ebp
0053185d 8945fc                   mov        dword ptr [ebp - 4], eax
00531860 56                       push       esi
00531861 894df4                   mov        dword ptr [ebp - 0xc], ecx
00531864 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531867 83c014                   add        eax, 0x14
0053186a 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053186d 6a00                     push       0
0053186f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00531872 e8191defff               call       0x423590
00531877 0fb6c8                   movzx      ecx, al
0053187a 85c9                     test       ecx, ecx
0053187c 741a                     je         0x531898
0053187e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531881 83c138                   add        ecx, 0x38
00531884 e847e4f1ff               call       0x44fcd0
00531889 b801000000               mov        eax, 1
0053188e e949010000               jmp        0x5319dc
00531893 e92f010000               jmp        0x5319c7
00531898 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053189b 83c138                   add        ecx, 0x38
0053189e e88dd8f1ff               call       0x44f130
005318a3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005318a6 83c138                   add        ecx, 0x38
005318a9 e882e1f1ff               call       0x44fa30
005318ae 6a00                     push       0
005318b0 e8cb27f3ff               call       0x464080
005318b5 83c404                   add        esp, 4
005318b8 8945e8                   mov        dword ptr [ebp - 0x18], eax
005318bb 6a00                     push       0
005318bd e8be27f3ff               call       0x464080
005318c2 83c404                   add        esp, 4
005318c5 8bc8                     mov        ecx, eax
005318c7 e854a0feff               call       0x51b920
005318cc 8bf0                     mov        esi, eax
005318ce 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005318d1 83c114                   add        ecx, 0x14
005318d4 e8b7e6edff               call       0x40ff90
005318d9 0faff0                   imul       esi, eax
005318dc e87f290000               call       0x534260
005318e1 8bc8                     mov        ecx, eax
005318e3 8bc6                     mov        eax, esi
005318e5 99                       cdq        
005318e6 f7f9                     idiv       ecx
005318e8 8945ec                   mov        dword ptr [ebp - 0x14], eax
005318eb 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005318ee 52                       push       edx
005318ef 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005318f2 e849d0f8ff               call       0x4be940
005318f7 6a00                     push       0
005318f9 e88275f4ff               call       0x478e80
005318fe 83c404                   add        esp, 4
00531901 8945e0                   mov        dword ptr [ebp - 0x20], eax
00531904 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531907 f30f104068               movss      xmm0, dword ptr [eax + 0x68]
0053190c f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00531911 6a00                     push       0
00531913 689f860100               push       0x1869f
00531918 6a00                     push       0
0053191a 51                       push       ecx
0053191b f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00531920 f30f110424               movss      dword ptr [esp], xmm0
00531925 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531928 83c15c                   add        ecx, 0x5c
0053192b 51                       push       ecx
0053192c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053192f e82cb6f4ff               call       0x47cf60
00531934 6a00                     push       0
00531936 e8a575f4ff               call       0x478ee0
0053193b 83c404                   add        esp, 4
0053193e 8945d8                   mov        dword ptr [ebp - 0x28], eax
00531941 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00531944 f30f104268               movss      xmm0, dword ptr [edx + 0x68]
00531949 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
0053194e 6a01                     push       1
00531950 6a00                     push       0
00531952 51                       push       ecx
00531953 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00531958 f30f110424               movss      dword ptr [esp], xmm0
0053195d 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531960 83c05c                   add        eax, 0x5c
00531963 50                       push       eax
00531964 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00531967 e86491f9ff               call       0x4caad0
0053196c 6a00                     push       0
0053196e e84d75f4ff               call       0x478ec0
00531973 83c404                   add        esp, 4
00531976 8945d0                   mov        dword ptr [ebp - 0x30], eax
00531979 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053197c f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
00531981 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00531986 6a14                     push       0x14
00531988 6a02                     push       2
0053198a 51                       push       ecx
0053198b 0f57c0                   xorps      xmm0, xmm0
0053198e f30f110424               movss      dword ptr [esp], xmm0
00531993 51                       push       ecx
00531994 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00531999 f30f110424               movss      dword ptr [esp], xmm0
0053199e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005319a1 83c25c                   add        edx, 0x5c
005319a4 52                       push       edx
005319a5 8d45f8                   lea        eax, [ebp - 8]
005319a8 50                       push       eax
005319a9 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
005319ac e8cf01f9ff               call       0x4c1b80
005319b1 8d4df8                   lea        ecx, [ebp - 8]
005319b4 e857f4f8ff               call       0x4c0e10
005319b9 8945cc                   mov        dword ptr [ebp - 0x34], eax
005319bc 6a01                     push       1
005319be 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
005319c1 e82a53fdff               call       0x506cf0
005319c6 90                       nop        
005319c7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005319ca 83c114                   add        ecx, 0x14
005319cd 894dc8                   mov        dword ptr [ebp - 0x38], ecx
005319d0 6a00                     push       0
005319d2 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005319d5 e8467aefff               call       0x429420
005319da 33c0                     xor        eax, eax
005319dc 5e                       pop        esi
005319dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005319e0 33cd                     xor        ecx, ebp
005319e2 e807100100               call       0x5429ee
005319e7 8be5                     mov        esp, ebp
005319e9 5d                       pop        ebp
005319ea c3                       ret        
005319eb cc                       int3       
005319ec cc                       int3       
005319ed cc                       int3       
005319ee cc                       int3       
005319ef cc                       int3       
