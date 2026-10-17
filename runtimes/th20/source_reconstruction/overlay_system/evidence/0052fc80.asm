0052fc80 55                       push       ebp
0052fc81 8bec                     mov        ebp, esp
0052fc83 83ec0c                   sub        esp, 0xc
0052fc86 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052fc89 8b4508                   mov        eax, dword ptr [ebp + 8]
0052fc8c c7801c01000010005300     mov        dword ptr [eax + 0x11c], 0x530010
0052fc96 6a00                     push       0
0052fc98 e8e343f3ff               call       0x464080
0052fc9d 83c404                   add        esp, 4
0052fca0 8bc8                     mov        ecx, eax
0052fca2 e8f984f8ff               call       0x4b81a0
0052fca7 85c0                     test       eax, eax
0052fca9 7455                     je         0x52fd00
0052fcab f30f2a450c               cvtsi2ss   xmm0, dword ptr [ebp + 0xc]
0052fcb0 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
0052fcb8 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
0052fcc0 6a00                     push       0
0052fcc2 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0052fcc7 f30f114dfc               movss      dword ptr [ebp - 4], xmm1
0052fccc e8af43f3ff               call       0x464080
0052fcd1 83c404                   add        esp, 4
0052fcd4 8bc8                     mov        ecx, eax
0052fcd6 e8c584f8ff               call       0x4b81a0
0052fcdb f30f2ac0                 cvtsi2ss   xmm0, eax
0052fcdf f30f104dfc               movss      xmm1, dword ptr [ebp - 4]
0052fce4 f30f5ec8                 divss      xmm1, xmm0
0052fce8 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
0052fced f30f59c1                 mulss      xmm0, xmm1
0052fcf1 51                       push       ecx
0052fcf2 f30f110424               movss      dword ptr [esp], xmm0
0052fcf7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052fcfa e8c1080000               call       0x5305c0
0052fcff 90                       nop        
0052fd00 8be5                     mov        esp, ebp
0052fd02 5d                       pop        ebp
0052fd03 c20800                   ret        8
0052fd06 cc                       int3       
0052fd07 cc                       int3       
0052fd08 cc                       int3       
0052fd09 cc                       int3       
0052fd0a cc                       int3       
0052fd0b cc                       int3       
0052fd0c cc                       int3       
0052fd0d cc                       int3       
0052fd0e cc                       int3       
0052fd0f cc                       int3       
