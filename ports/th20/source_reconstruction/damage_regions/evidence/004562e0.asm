004562e0 55                       push       ebp
004562e1 8bec                     mov        ebp, esp
004562e3 83ec40                   sub        esp, 0x40
004562e6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004562eb 33c5                     xor        eax, ebp
004562ed 8945fc                   mov        dword ptr [ebp - 4], eax
004562f0 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004562f5 0f2f4520                 comiss     xmm0, dword ptr [ebp + 0x20]
004562f9 760c                     jbe        0x456307
004562fb f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456300 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00456305 eb0a                     jmp        0x456311
00456307 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
0045630c f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00456311 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00456316 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
0045631b f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456320 0f2f451c                 comiss     xmm0, dword ptr [ebp + 0x1c]
00456324 760c                     jbe        0x456332
00456326 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0045632b f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
00456330 eb0a                     jmp        0x45633c
00456332 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456337 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
0045633c f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00456341 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00456346 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
0045634b 0f2f4510                 comiss     xmm0, dword ptr [ebp + 0x10]
0045634f 766d                     jbe        0x4563be
00456351 51                       push       ecx
00456352 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456357 f30f110424               movss      dword ptr [esp], xmm0
0045635c f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456361 f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00456366 51                       push       ecx
00456367 f30f110424               movss      dword ptr [esp], xmm0
0045636c f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456371 f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00456376 51                       push       ecx
00456377 f30f110424               movss      dword ptr [esp], xmm0
0045637c 51                       push       ecx
0045637d f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456382 f30f110424               movss      dword ptr [esp], xmm0
00456387 51                       push       ecx
00456388 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045638d f30f110424               movss      dword ptr [esp], xmm0
00456392 51                       push       ecx
00456393 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456398 f30f110424               movss      dword ptr [esp], xmm0
0045639d 51                       push       ecx
0045639e f30f104508               movss      xmm0, dword ptr [ebp + 8]
004563a3 f30f110424               movss      dword ptr [esp], xmm0
004563a8 e8930c0000               call       0x457040
004563ad 83c41c                   add        esp, 0x1c
004563b0 0fb6c0                   movzx      eax, al
004563b3 85c0                     test       eax, eax
004563b5 7407                     je         0x4563be
004563b7 b001                     mov        al, 1
004563b9 e9b8020000               jmp        0x456676
004563be 8d4df0                   lea        ecx, [ebp - 0x10]
004563c1 e84acafcff               call       0x422e10
004563c6 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004563cb f30f5c4514               subss      xmm0, dword ptr [ebp + 0x14]
004563d0 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004563d5 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004563da f30f5c4518               subss      xmm0, dword ptr [ebp + 0x18]
004563df f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004563e4 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004563e9 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004563f0 51                       push       ecx
004563f1 f30f110424               movss      dword ptr [esp], xmm0
004563f6 8d4df0                   lea        ecx, [ebp - 0x10]
004563f9 51                       push       ecx
004563fa 8d55f0                   lea        edx, [ebp - 0x10]
004563fd 52                       push       edx
004563fe e89d2b0000               call       0x458fa0
00456403 83c40c                   add        esp, 0xc
00456406 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045640b 0f2f45dc                 comiss     xmm0, dword ptr [ebp - 0x24]
0045640f 763c                     jbe        0x45644d
00456411 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00456416 f30f5945f0               mulss      xmm0, dword ptr [ebp - 0x10]
0045641b f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
00456420 f30f594df4               mulss      xmm1, dword ptr [ebp - 0xc]
00456425 f30f58c1                 addss      xmm0, xmm1
00456429 f30f104d10               movss      xmm1, dword ptr [ebp + 0x10]
0045642e f30f5c4ddc               subss      xmm1, dword ptr [ebp - 0x24]
00456433 f30f105510               movss      xmm2, dword ptr [ebp + 0x10]
00456438 f30f5c55dc               subss      xmm2, dword ptr [ebp - 0x24]
0045643d f30f59ca                 mulss      xmm1, xmm2
00456441 0f2fc8                   comiss     xmm1, xmm0
00456444 7207                     jb         0x45644d
00456446 b001                     mov        al, 1
00456448 e929020000               jmp        0x456676
0045644d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00456455 0f2f4510                 comiss     xmm0, dword ptr [ebp + 0x10]
00456459 7607                     jbe        0x456462
0045645b 32c0                     xor        al, al
0045645d e914020000               jmp        0x456676
00456462 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0045646a 0f2f45dc                 comiss     xmm0, dword ptr [ebp - 0x24]
0045646e 7607                     jbe        0x456477
00456470 32c0                     xor        al, al
00456472 e9ff010000               jmp        0x456676
00456477 8d4de4                   lea        ecx, [ebp - 0x1c]
0045647a e891c9fcff               call       0x422e10
0045647f f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00456487 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
0045648c f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456491 0f2f45dc                 comiss     xmm0, dword ptr [ebp - 0x24]
00456495 0f86ec000000             jbe        0x456587
0045649b f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004564a0 f30f584520               addss      xmm0, dword ptr [ebp + 0x20]
004564a5 f30f2cc0                 cvttss2si  eax, xmm0
004564a9 99                       cdq        
004564aa 83e203                   and        edx, 3
004564ad 03c2                     add        eax, edx
004564af c1f802                   sar        eax, 2
004564b2 8945d8                   mov        dword ptr [ebp - 0x28], eax
004564b5 837dd808                 cmp        dword ptr [ebp - 0x28], 8
004564b9 7d07                     jge        0x4564c2
004564bb c745d808000000           mov        dword ptr [ebp - 0x28], 8
004564c2 c745d000000000           mov        dword ptr [ebp - 0x30], 0
004564c9 eb09                     jmp        0x4564d4
004564cb 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004564ce 83c001                   add        eax, 1
004564d1 8945d0                   mov        dword ptr [ebp - 0x30], eax
004564d4 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004564d7 3b4dd8                   cmp        ecx, dword ptr [ebp - 0x28]
004564da 0f8da2000000             jge        0x456582
004564e0 51                       push       ecx
004564e1 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004564e6 f30f110424               movss      dword ptr [esp], xmm0
004564eb 51                       push       ecx
004564ec f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004564f1 f30f110424               movss      dword ptr [esp], xmm0
004564f6 51                       push       ecx
004564f7 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004564fc f30f110424               movss      dword ptr [esp], xmm0
00456501 8d55e4                   lea        edx, [ebp - 0x1c]
00456504 52                       push       edx
00456505 e8862c0000               call       0x459190
0045650a 83c410                   add        esp, 0x10
0045650d f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00456515 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045651d f30f2a4dd8               cvtsi2ss   xmm1, dword ptr [ebp - 0x28]
00456522 f30f5ec1                 divss      xmm0, xmm1
00456526 f30f5845e0               addss      xmm0, dword ptr [ebp - 0x20]
0045652b f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00456530 51                       push       ecx
00456531 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456536 f30f110424               movss      dword ptr [esp], xmm0
0045653b 51                       push       ecx
0045653c f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00456541 f30f110424               movss      dword ptr [esp], xmm0
00456546 51                       push       ecx
00456547 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0045654c f30f110424               movss      dword ptr [esp], xmm0
00456551 51                       push       ecx
00456552 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00456557 f30f110424               movss      dword ptr [esp], xmm0
0045655c 51                       push       ecx
0045655d f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00456562 f30f110424               movss      dword ptr [esp], xmm0
00456567 e8740a0000               call       0x456fe0
0045656c 83c414                   add        esp, 0x14
0045656f 0fb6c0                   movzx      eax, al
00456572 85c0                     test       eax, eax
00456574 7407                     je         0x45657d
00456576 b001                     mov        al, 1
00456578 e9f9000000               jmp        0x456676
0045657d e949ffffff               jmp        0x4564cb
00456582 e9ed000000               jmp        0x456674
00456587 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045658c f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
00456594 f30f2cc8                 cvttss2si  ecx, xmm0
00456598 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0045659b 837dd408                 cmp        dword ptr [ebp - 0x2c], 8
0045659f 7d07                     jge        0x4565a8
004565a1 c745d408000000           mov        dword ptr [ebp - 0x2c], 8
004565a8 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
004565af eb09                     jmp        0x4565ba
004565b1 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004565b4 83c201                   add        edx, 1
004565b7 8955cc                   mov        dword ptr [ebp - 0x34], edx
004565ba 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004565bd 3b45d4                   cmp        eax, dword ptr [ebp - 0x2c]
004565c0 0f8dae000000             jge        0x456674
004565c6 51                       push       ecx
004565c7 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004565cc f30f110424               movss      dword ptr [esp], xmm0
004565d1 51                       push       ecx
004565d2 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004565d7 f30f110424               movss      dword ptr [esp], xmm0
004565dc 8d4de4                   lea        ecx, [ebp - 0x1c]
004565df 51                       push       ecx
004565e0 e84b2dfeff               call       0x439330
004565e5 83c40c                   add        esp, 0xc
004565e8 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004565f0 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
004565f8 f30f2a4dd4               cvtsi2ss   xmm1, dword ptr [ebp - 0x2c]
004565fd f30f5ec1                 divss      xmm0, xmm1
00456601 f30f5845e0               addss      xmm0, dword ptr [ebp - 0x20]
00456606 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
0045660b 51                       push       ecx
0045660c 0f57c0                   xorps      xmm0, xmm0
0045660f f30f110424               movss      dword ptr [esp], xmm0
00456614 51                       push       ecx
00456615 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
0045661a f30f110424               movss      dword ptr [esp], xmm0
0045661f 51                       push       ecx
00456620 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456625 f30f110424               movss      dword ptr [esp], xmm0
0045662a 51                       push       ecx
0045662b 0f57c0                   xorps      xmm0, xmm0
0045662e f30f110424               movss      dword ptr [esp], xmm0
00456633 51                       push       ecx
00456634 0f57c0                   xorps      xmm0, xmm0
00456637 f30f110424               movss      dword ptr [esp], xmm0
0045663c f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00456641 f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
00456646 51                       push       ecx
00456647 f30f110424               movss      dword ptr [esp], xmm0
0045664c f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00456651 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
00456656 51                       push       ecx
00456657 f30f110424               movss      dword ptr [esp], xmm0
0045665c e8df090000               call       0x457040
00456661 83c41c                   add        esp, 0x1c
00456664 0fb6d0                   movzx      edx, al
00456667 85d2                     test       edx, edx
00456669 7404                     je         0x45666f
0045666b b001                     mov        al, 1
0045666d eb07                     jmp        0x456676
0045666f e93dffffff               jmp        0x4565b1
00456674 32c0                     xor        al, al
00456676 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00456679 33cd                     xor        ecx, ebp
0045667b e86ec30e00               call       0x5429ee
00456680 8be5                     mov        esp, ebp
00456682 5d                       pop        ebp
00456683 c3                       ret        
00456684 cc                       int3       
00456685 cc                       int3       
00456686 cc                       int3       
00456687 cc                       int3       
00456688 cc                       int3       
00456689 cc                       int3       
0045668a cc                       int3       
0045668b cc                       int3       
0045668c cc                       int3       
0045668d cc                       int3       
0045668e cc                       int3       
0045668f cc                       int3       
