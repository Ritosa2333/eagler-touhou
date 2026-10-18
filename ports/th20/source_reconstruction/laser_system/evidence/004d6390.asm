004d6390 55                       push       ebp
004d6391 8bec                     mov        ebp, esp
004d6393 81ecec000000             sub        esp, 0xec
004d6399 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d639e 33c5                     xor        eax, ebp
004d63a0 8945fc                   mov        dword ptr [ebp - 4], eax
004d63a3 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004d63a6 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d63a9 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d63ae f30f5c4008               subss      xmm0, dword ptr [eax + 8]
004d63b3 f30f114514               movss      dword ptr [ebp + 0x14], xmm0
004d63b8 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d63bb 8b5110                   mov        edx, dword ptr [ecx + 0x10]
004d63be 8955b0                   mov        dword ptr [ebp - 0x50], edx
004d63c1 837db000                 cmp        dword ptr [ebp - 0x50], 0
004d63c5 7419                     je         0x4d63e0
004d63c7 837db001                 cmp        dword ptr [ebp - 0x50], 1
004d63cb 0f84a8000000             je         0x4d6479
004d63d1 837db002                 cmp        dword ptr [ebp - 0x50], 2
004d63d5 0f849c020000             je         0x4d6677
004d63db e9e3030000               jmp        0x4d67c3
004d63e0 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d63e3 83c020                   add        eax, 0x20
004d63e6 89459c                   mov        dword ptr [ebp - 0x64], eax
004d63e9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d63ec 83c114                   add        ecx, 0x14
004d63ef 894da8                   mov        dword ptr [ebp - 0x58], ecx
004d63f2 51                       push       ecx
004d63f3 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d63f8 f30f110424               movss      dword ptr [esp], xmm0
004d63fd 8d9574ffffff             lea        edx, [ebp - 0x8c]
004d6403 52                       push       edx
004d6404 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004d6407 e8e42ef5ff               call       0x4292f0
004d640c 8945a0                   mov        dword ptr [ebp - 0x60], eax
004d640f 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d6412 f30f104030               movss      xmm0, dword ptr [eax + 0x30]
004d6417 f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
004d641c 51                       push       ecx
004d641d f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004d6422 f30f110424               movss      dword ptr [esp], xmm0
004d6427 8d8d68ffffff             lea        ecx, [ebp - 0x98]
004d642d 51                       push       ecx
004d642e 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004d6431 e8ba2ef5ff               call       0x4292f0
004d6436 50                       push       eax
004d6437 8d955cffffff             lea        edx, [ebp - 0xa4]
004d643d 52                       push       edx
004d643e 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004d6441 e82a31f5ff               call       0x429570
004d6446 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d6449 8b10                     mov        edx, dword ptr [eax]
004d644b 8911                     mov        dword ptr [ecx], edx
004d644d 8b5004                   mov        edx, dword ptr [eax + 4]
004d6450 895104                   mov        dword ptr [ecx + 4], edx
004d6453 8b4008                   mov        eax, dword ptr [eax + 8]
004d6456 894108                   mov        dword ptr [ecx + 8], eax
004d6459 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004d645c 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d645f 8b4230                   mov        eax, dword ptr [edx + 0x30]
004d6462 8901                     mov        dword ptr [ecx], eax
004d6464 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d6467 83c12c                   add        ecx, 0x2c
004d646a e8712ef5ff               call       0x4292e0
004d646f 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004d6472 d919                     fstp       dword ptr [ecx]
004d6474 e94a030000               jmp        0x4d67c3
004d6479 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d647c f30f100574fd5600         movss      xmm0, dword ptr [0x56fd74]
004d6484 0f2f4238                 comiss     xmm0, dword ptr [edx + 0x38]
004d6488 0f86e1000000             jbe        0x4d656f
004d648e 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d6491 83c020                   add        eax, 0x20
004d6494 89458c                   mov        dword ptr [ebp - 0x74], eax
004d6497 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d649a 83c114                   add        ecx, 0x14
004d649d 894d98                   mov        dword ptr [ebp - 0x68], ecx
004d64a0 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d64a3 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d64a8 f30f594234               mulss      xmm0, dword ptr [edx + 0x34]
004d64ad 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d64b0 f30f104830               movss      xmm1, dword ptr [eax + 0x30]
004d64b5 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
004d64bd f30f58c1                 addss      xmm0, xmm1
004d64c1 51                       push       ecx
004d64c2 f30f110424               movss      dword ptr [esp], xmm0
004d64c7 8d8d50ffffff             lea        ecx, [ebp - 0xb0]
004d64cd 51                       push       ecx
004d64ce 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004d64d1 e81a2ef5ff               call       0x4292f0
004d64d6 894594                   mov        dword ptr [ebp - 0x6c], eax
004d64d9 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d64de f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004d64e6 51                       push       ecx
004d64e7 f30f110424               movss      dword ptr [esp], xmm0
004d64ec 8d9544ffffff             lea        edx, [ebp - 0xbc]
004d64f2 52                       push       edx
004d64f3 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004d64f6 e8f52df5ff               call       0x4292f0
004d64fb 894590                   mov        dword ptr [ebp - 0x70], eax
004d64fe 51                       push       ecx
004d64ff f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004d6507 f30f110424               movss      dword ptr [esp], xmm0
004d650c 8d8538ffffff             lea        eax, [ebp - 0xc8]
004d6512 50                       push       eax
004d6513 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004d6516 e8d5caf7ff               call       0x452ff0
004d651b 50                       push       eax
004d651c 8d8d2cffffff             lea        ecx, [ebp - 0xd4]
004d6522 51                       push       ecx
004d6523 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004d6526 e84530f5ff               call       0x429570
004d652b 8b5508                   mov        edx, dword ptr [ebp + 8]
004d652e 8b08                     mov        ecx, dword ptr [eax]
004d6530 890a                     mov        dword ptr [edx], ecx
004d6532 8b4804                   mov        ecx, dword ptr [eax + 4]
004d6535 894a04                   mov        dword ptr [edx + 4], ecx
004d6538 8b4008                   mov        eax, dword ptr [eax + 8]
004d653b 894208                   mov        dword ptr [edx + 8], eax
004d653e 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d6541 f30f104134               movss      xmm0, dword ptr [ecx + 0x34]
004d6546 f30f594514               mulss      xmm0, dword ptr [ebp + 0x14]
004d654b 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d654e f30f584230               addss      xmm0, dword ptr [edx + 0x30]
004d6553 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004d6556 f30f1100                 movss      dword ptr [eax], xmm0
004d655a 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d655d 83c12c                   add        ecx, 0x2c
004d6560 e87b2df5ff               call       0x4292e0
004d6565 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004d6568 d919                     fstp       dword ptr [ecx]
004d656a e903010000               jmp        0x4d6672
004d656f 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d6572 83c220                   add        edx, 0x20
004d6575 8b02                     mov        eax, dword ptr [edx]
004d6577 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004d657a 8b4a04                   mov        ecx, dword ptr [edx + 4]
004d657d 894de8                   mov        dword ptr [ebp - 0x18], ecx
004d6580 8b5208                   mov        edx, dword ptr [edx + 8]
004d6583 8955ec                   mov        dword ptr [ebp - 0x14], edx
004d6586 8d4dd8                   lea        ecx, [ebp - 0x28]
004d6589 e882c8f4ff               call       0x422e10
004d658e 8d4dc0                   lea        ecx, [ebp - 0x40]
004d6591 e87ac8f4ff               call       0x422e10
004d6596 0f57c0                   xorps      xmm0, xmm0
004d6599 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004d659e 0f57c0                   xorps      xmm0, xmm0
004d65a1 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
004d65a6 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d65a9 f30f104030               movss      xmm0, dword ptr [eax + 0x30]
004d65ae f30f114588               movss      dword ptr [ebp - 0x78], xmm0
004d65b3 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d65b6 83c12c                   add        ecx, 0x2c
004d65b9 e8222df5ff               call       0x4292e0
004d65be d95d84                   fstp       dword ptr [ebp - 0x7c]
004d65c1 51                       push       ecx
004d65c2 f30f104588               movss      xmm0, dword ptr [ebp - 0x78]
004d65c7 f30f110424               movss      dword ptr [esp], xmm0
004d65cc 51                       push       ecx
004d65cd f30f104584               movss      xmm0, dword ptr [ebp - 0x7c]
004d65d2 f30f110424               movss      dword ptr [esp], xmm0
004d65d7 8d4dd8                   lea        ecx, [ebp - 0x28]
004d65da 51                       push       ecx
004d65db e8502df6ff               call       0x439330
004d65e0 83c40c                   add        esp, 0xc
004d65e3 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d65e6 51                       push       ecx
004d65e7 f30f104234               movss      xmm0, dword ptr [edx + 0x34]
004d65ec f30f110424               movss      dword ptr [esp], xmm0
004d65f1 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d65f4 51                       push       ecx
004d65f5 f30f104038               movss      xmm0, dword ptr [eax + 0x38]
004d65fa f30f110424               movss      dword ptr [esp], xmm0
004d65ff 8d4dc0                   lea        ecx, [ebp - 0x40]
004d6602 51                       push       ecx
004d6603 e8282df6ff               call       0x439330
004d6608 83c40c                   add        esp, 0xc
004d660b 8d55c0                   lea        edx, [ebp - 0x40]
004d660e 52                       push       edx
004d660f 8d4dd8                   lea        ecx, [ebp - 0x28]
004d6612 e8c930f5ff               call       0x4296e0
004d6617 90                       nop        
004d6618 51                       push       ecx
004d6619 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d661e f30f110424               movss      dword ptr [esp], xmm0
004d6623 8d8520ffffff             lea        eax, [ebp - 0xe0]
004d6629 50                       push       eax
004d662a 8d4dd8                   lea        ecx, [ebp - 0x28]
004d662d e8be2cf5ff               call       0x4292f0
004d6632 50                       push       eax
004d6633 8d4de4                   lea        ecx, [ebp - 0x1c]
004d6636 e8a530f5ff               call       0x4296e0
004d663b 90                       nop        
004d663c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d663f 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004d6642 8911                     mov        dword ptr [ecx], edx
004d6644 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004d6647 894104                   mov        dword ptr [ecx + 4], eax
004d664a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d664d 895108                   mov        dword ptr [ecx + 8], edx
004d6650 8d45d8                   lea        eax, [ebp - 0x28]
004d6653 50                       push       eax
004d6654 e8473cfaff               call       0x47a2a0
004d6659 83c404                   add        esp, 4
004d665c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004d665f d919                     fstp       dword ptr [ecx]
004d6661 8d55d8                   lea        edx, [ebp - 0x28]
004d6664 52                       push       edx
004d6665 e8a6fbf7ff               call       0x456210
004d666a 83c404                   add        esp, 4
004d666d 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004d6670 d918                     fstp       dword ptr [eax]
004d6672 e94c010000               jmp        0x4d67c3
004d6677 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d667a 83c120                   add        ecx, 0x20
004d667d 8b11                     mov        edx, dword ptr [ecx]
004d667f 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d6682 8b4104                   mov        eax, dword ptr [ecx + 4]
004d6685 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d6688 8b4908                   mov        ecx, dword ptr [ecx + 8]
004d668b 894df8                   mov        dword ptr [ebp - 8], ecx
004d668e 8d4dcc                   lea        ecx, [ebp - 0x34]
004d6691 e87ac7f4ff               call       0x422e10
004d6696 0f57c0                   xorps      xmm0, xmm0
004d6699 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
004d669e 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d66a1 83c12c                   add        ecx, 0x2c
004d66a4 e8372cf5ff               call       0x4292e0
004d66a9 d95db4                   fstp       dword ptr [ebp - 0x4c]
004d66ac 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d66af f30f104230               movss      xmm0, dword ptr [edx + 0x30]
004d66b4 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004d66b9 c745ac00000000           mov        dword ptr [ebp - 0x54], 0
004d66c0 eb09                     jmp        0x4d66cb
004d66c2 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
004d66c5 83c001                   add        eax, 1
004d66c8 8945ac                   mov        dword ptr [ebp - 0x54], eax
004d66cb f30f2c4d14               cvttss2si  ecx, dword ptr [ebp + 0x14]
004d66d0 394dac                   cmp        dword ptr [ebp - 0x54], ecx
004d66d3 7d61                     jge        0x4d6736
004d66d5 51                       push       ecx
004d66d6 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004d66db f30f110424               movss      dword ptr [esp], xmm0
004d66e0 51                       push       ecx
004d66e1 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
004d66e6 f30f110424               movss      dword ptr [esp], xmm0
004d66eb 8d55cc                   lea        edx, [ebp - 0x34]
004d66ee 52                       push       edx
004d66ef e83c2cf6ff               call       0x439330
004d66f4 83c40c                   add        esp, 0xc
004d66f7 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d66fa f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
004d66ff f30f584038               addss      xmm0, dword ptr [eax + 0x38]
004d6704 51                       push       ecx
004d6705 f30f110424               movss      dword ptr [esp], xmm0
004d670a e8311ef6ff               call       0x438540
004d670f 83c404                   add        esp, 4
004d6712 d95db4                   fstp       dword ptr [ebp - 0x4c]
004d6715 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d6718 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004d671d f30f584134               addss      xmm0, dword ptr [ecx + 0x34]
004d6722 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004d6727 8d55cc                   lea        edx, [ebp - 0x34]
004d672a 52                       push       edx
004d672b 8d4df0                   lea        ecx, [ebp - 0x10]
004d672e e8ad2ff5ff               call       0x4296e0
004d6733 90                       nop        
004d6734 eb8c                     jmp        0x4d66c2
004d6736 51                       push       ecx
004d6737 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004d673c f30f110424               movss      dword ptr [esp], xmm0
004d6741 51                       push       ecx
004d6742 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
004d6747 f30f110424               movss      dword ptr [esp], xmm0
004d674c 8d45cc                   lea        eax, [ebp - 0x34]
004d674f 50                       push       eax
004d6750 e8db2bf6ff               call       0x439330
004d6755 83c40c                   add        esp, 0xc
004d6758 51                       push       ecx
004d6759 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d675e f30f110424               movss      dword ptr [esp], xmm0
004d6763 e8482bf8ff               call       0x4592b0
004d6768 83c404                   add        esp, 4
004d676b d95d80                   fstp       dword ptr [ebp - 0x80]
004d676e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004d6773 f30f5c4580               subss      xmm0, dword ptr [ebp - 0x80]
004d6778 51                       push       ecx
004d6779 f30f110424               movss      dword ptr [esp], xmm0
004d677e 8d8d14ffffff             lea        ecx, [ebp - 0xec]
004d6784 51                       push       ecx
004d6785 8d4dcc                   lea        ecx, [ebp - 0x34]
004d6788 e8632bf5ff               call       0x4292f0
004d678d 50                       push       eax
004d678e 8d4df0                   lea        ecx, [ebp - 0x10]
004d6791 e84a2ff5ff               call       0x4296e0
004d6796 90                       nop        
004d6797 8b5508                   mov        edx, dword ptr [ebp + 8]
004d679a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004d679d 8902                     mov        dword ptr [edx], eax
004d679f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d67a2 894a04                   mov        dword ptr [edx + 4], ecx
004d67a5 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d67a8 894208                   mov        dword ptr [edx + 8], eax
004d67ab 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004d67ae f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004d67b3 f30f1101                 movss      dword ptr [ecx], xmm0
004d67b7 8b5510                   mov        edx, dword ptr [ebp + 0x10]
004d67ba f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
004d67bf f30f1102                 movss      dword ptr [edx], xmm0
004d67c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d67c6 33cd                     xor        ecx, ebp
004d67c8 e821c20600               call       0x5429ee
004d67cd 8be5                     mov        esp, ebp
004d67cf 5d                       pop        ebp
004d67d0 c21000                   ret        0x10
004d67d3 cc                       int3       
004d67d4 cc                       int3       
004d67d5 cc                       int3       
004d67d6 cc                       int3       
004d67d7 cc                       int3       
004d67d8 cc                       int3       
004d67d9 cc                       int3       
004d67da cc                       int3       
004d67db cc                       int3       
004d67dc cc                       int3       
004d67dd cc                       int3       
004d67de cc                       int3       
004d67df cc                       int3       
