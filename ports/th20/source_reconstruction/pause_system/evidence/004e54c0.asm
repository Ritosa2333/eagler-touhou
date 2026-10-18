004e54c0 55                       push       ebp
004e54c1 8bec                     mov        ebp, esp
004e54c3 83ec20                   sub        esp, 0x20
004e54c6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e54cb 33c5                     xor        eax, ebp
004e54cd 8945fc                   mov        dword ptr [ebp - 4], eax
004e54d0 894dec                   mov        dword ptr [ebp - 0x14], ecx
004e54d3 51                       push       ecx
004e54d4 0f57c0                   xorps      xmm0, xmm0
004e54d7 f30f110424               movss      dword ptr [esp], xmm0
004e54dc 51                       push       ecx
004e54dd f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
004e54e5 f30f110424               movss      dword ptr [esp], xmm0
004e54ea 51                       push       ecx
004e54eb f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
004e54f3 f30f110424               movss      dword ptr [esp], xmm0
004e54f8 8d4df0                   lea        ecx, [ebp - 0x10]
004e54fb e8d0d8f3ff               call       0x422dd0
004e5500 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e5503 83c130                   add        ecx, 0x30
004e5506 e8f56df2ff               call       0x40c300
004e550b 8945e8                   mov        dword ptr [ebp - 0x18], eax
004e550e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e5511 83c010                   add        eax, 0x10
004e5514 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004e5517 6a0a                     push       0xa
004e5519 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004e551c e83fe0f3ff               call       0x423560
004e5521 0fb6c8                   movzx      ecx, al
004e5524 85c9                     test       ecx, ecx
004e5526 7466                     je         0x4e558e
004e5528 f30f2a45e8               cvtsi2ss   xmm0, dword ptr [ebp - 0x18]
004e552d f30f5905fcf25600         mulss      xmm0, dword ptr [0x56f2fc]
004e5535 f30f580528fa5600         addss      xmm0, dword ptr [0x56fa28]
004e553d f30f100d14ed5600         movss      xmm1, dword ptr [0x56ed14]
004e5545 f30f5cc8                 subss      xmm1, xmm0
004e5549 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e554c 83c110                   add        ecx, 0x10
004e554f f30f114de0               movss      dword ptr [ebp - 0x20], xmm1
004e5554 e837aaf2ff               call       0x40ff90
004e5559 f30f2ac0                 cvtsi2ss   xmm0, eax
004e555d f30f104de0               movss      xmm1, dword ptr [ebp - 0x20]
004e5562 f30f59c8                 mulss      xmm1, xmm0
004e5566 f30f5e0d34e75600         divss      xmm1, dword ptr [0x56e734]
004e556e f30f2a45e8               cvtsi2ss   xmm0, dword ptr [ebp - 0x18]
004e5573 f30f5905fcf25600         mulss      xmm0, dword ptr [0x56f2fc]
004e557b f30f580528fa5600         addss      xmm0, dword ptr [0x56fa28]
004e5583 f30f58c8                 addss      xmm1, xmm0
004e5587 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
004e558c eb0d                     jmp        0x4e559b
004e558e f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
004e5596 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e559b f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004e55a0 f30f580550265700         addss      xmm0, dword ptr [0x572650]
004e55a8 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004e55ad 83ec0c                   sub        esp, 0xc
004e55b0 8bd4                     mov        edx, esp
004e55b2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e55b5 8902                     mov        dword ptr [edx], eax
004e55b7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e55ba 894a04                   mov        dword ptr [edx + 4], ecx
004e55bd 8b45f8                   mov        eax, dword ptr [ebp - 8]
004e55c0 894208                   mov        dword ptr [edx + 8], eax
004e55c3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e55c6 e855faffff               call       0x4e5020
004e55cb f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
004e55d3 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004e55d8 68ac245700               push       0x5724ac
004e55dd 6a09                     push       9
004e55df 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004e55e5 e826c2f3ff               call       0x421810
004e55ea 50                       push       eax
004e55eb e820300600               call       0x548610
004e55f0 83c40c                   add        esp, 0xc
004e55f3 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004e55f9 e812c2f3ff               call       0x421810
004e55fe 50                       push       eax
004e55ff 8d4df0                   lea        ecx, [ebp - 0x10]
004e5602 51                       push       ecx
004e5603 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004e5606 52                       push       edx
004e5607 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e560a e881fdffff               call       0x4e5390
004e560f 90                       nop        
004e5610 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5613 33cd                     xor        ecx, ebp
004e5615 e8d4d30500               call       0x5429ee
004e561a 8be5                     mov        esp, ebp
004e561c 5d                       pop        ebp
004e561d c3                       ret        
004e561e cc                       int3       
004e561f cc                       int3       
