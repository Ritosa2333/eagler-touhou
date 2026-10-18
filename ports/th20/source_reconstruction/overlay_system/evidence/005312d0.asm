005312d0 55                       push       ebp
005312d1 8bec                     mov        ebp, esp
005312d3 83ec28                   sub        esp, 0x28
005312d6 56                       push       esi
005312d7 894dfc                   mov        dword ptr [ebp - 4], ecx
005312da 8b45fc                   mov        eax, dword ptr [ebp - 4]
005312dd 83c014                   add        eax, 0x14
005312e0 8945f8                   mov        dword ptr [ebp - 8], eax
005312e3 6a00                     push       0
005312e5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005312e8 e8a322efff               call       0x423590
005312ed 0fb6c8                   movzx      ecx, al
005312f0 85c9                     test       ecx, ecx
005312f2 0f8487000000             je         0x53137f
005312f8 6a00                     push       0
005312fa e8d17cf6ff               call       0x498fd0
005312ff 83c404                   add        esp, 4
00531302 8945f0                   mov        dword ptr [ebp - 0x10], eax
00531305 51                       push       ecx
00531306 0f57c0                   xorps      xmm0, xmm0
00531309 f30f110424               movss      dword ptr [esp], xmm0
0053130e 51                       push       ecx
0053130f f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
00531317 f30f110424               movss      dword ptr [esp], xmm0
0053131c 51                       push       ecx
0053131d 0f57c0                   xorps      xmm0, xmm0
00531320 f30f110424               movss      dword ptr [esp], xmm0
00531325 8d4dd8                   lea        ecx, [ebp - 0x28]
00531328 e8a31aefff               call       0x422dd0
0053132d 8945f4                   mov        dword ptr [ebp - 0xc], eax
00531330 6aff                     push       -1
00531332 6a00                     push       0
00531334 6a00                     push       0
00531336 51                       push       ecx
00531337 f30f100534fd5600         movss      xmm0, dword ptr [0x56fd34]
0053133f f30f110424               movss      dword ptr [esp], xmm0
00531344 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
0053134c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00531354 51                       push       ecx
00531355 f30f110424               movss      dword ptr [esp], xmm0
0053135a 6aff                     push       -1
0053135c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0053135f 52                       push       edx
00531360 6a06                     push       6
00531362 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00531365 e82629f9ff               call       0x4c3c90
0053136a 90                       nop        
0053136b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053136e 83c138                   add        ecx, 0x38
00531371 e85ae9f1ff               call       0x44fcd0
00531376 b801000000               mov        eax, 1
0053137b eb77                     jmp        0x5313f4
0053137d eb60                     jmp        0x5313df
0053137f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531382 83c138                   add        ecx, 0x38
00531385 e8a6ddf1ff               call       0x44f130
0053138a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053138d 83c138                   add        ecx, 0x38
00531390 e89be6f1ff               call       0x44fa30
00531395 6a00                     push       0
00531397 e8e42cf3ff               call       0x464080
0053139c 83c404                   add        esp, 4
0053139f 8945e8                   mov        dword ptr [ebp - 0x18], eax
005313a2 6a00                     push       0
005313a4 e8d72cf3ff               call       0x464080
005313a9 83c404                   add        esp, 4
005313ac 8bc8                     mov        ecx, eax
005313ae e86da5feff               call       0x51b920
005313b3 8bf0                     mov        esi, eax
005313b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005313b8 83c114                   add        ecx, 0x14
005313bb e8d0ebedff               call       0x40ff90
005313c0 0faff0                   imul       esi, eax
005313c3 e8982e0000               call       0x534260
005313c8 8bc8                     mov        ecx, eax
005313ca 8bc6                     mov        eax, esi
005313cc 99                       cdq        
005313cd f7f9                     idiv       ecx
005313cf 8945ec                   mov        dword ptr [ebp - 0x14], eax
005313d2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005313d5 52                       push       edx
005313d6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005313d9 e862d5f8ff               call       0x4be940
005313de 90                       nop        
005313df 8b45fc                   mov        eax, dword ptr [ebp - 4]
005313e2 83c014                   add        eax, 0x14
005313e5 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005313e8 6a00                     push       0
005313ea 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005313ed e82e80efff               call       0x429420
005313f2 33c0                     xor        eax, eax
005313f4 5e                       pop        esi
005313f5 8be5                     mov        esp, ebp
005313f7 5d                       pop        ebp
005313f8 c3                       ret        
005313f9 cc                       int3       
005313fa cc                       int3       
005313fb cc                       int3       
005313fc cc                       int3       
005313fd cc                       int3       
005313fe cc                       int3       
005313ff cc                       int3       
