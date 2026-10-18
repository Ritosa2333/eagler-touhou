004545d0 55                       push       ebp
004545d1 8bec                     mov        ebp, esp
004545d3 83ec20                   sub        esp, 0x20
004545d6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004545db 33c5                     xor        eax, ebp
004545dd 8945fc                   mov        dword ptr [ebp - 4], eax
004545e0 51                       push       ecx
004545e1 f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004545e6 f30f110424               movss      dword ptr [esp], xmm0
004545eb 51                       push       ecx
004545ec f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004545f1 f30f110424               movss      dword ptr [esp], xmm0
004545f6 51                       push       ecx
004545f7 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004545fc f30f110424               movss      dword ptr [esp], xmm0
00454601 51                       push       ecx
00454602 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00454607 f30f110424               movss      dword ptr [esp], xmm0
0045460c 51                       push       ecx
0045460d f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00454612 f30f110424               movss      dword ptr [esp], xmm0
00454617 51                       push       ecx
00454618 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045461d f30f110424               movss      dword ptr [esp], xmm0
00454622 51                       push       ecx
00454623 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00454628 f30f110424               movss      dword ptr [esp], xmm0
0045462d 51                       push       ecx
0045462e f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454633 f30f110424               movss      dword ptr [esp], xmm0
00454638 e8e3220000               call       0x456920
0045463d 83c420                   add        esp, 0x20
00454640 0fb6c0                   movzx      eax, al
00454643 85c0                     test       eax, eax
00454645 7507                     jne        0x45464e
00454647 33c0                     xor        eax, eax
00454649 e982010000               jmp        0x4547d0
0045464e 51                       push       ecx
0045464f f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00454654 f30f110424               movss      dword ptr [esp], xmm0
00454659 51                       push       ecx
0045465a f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045465f f30f110424               movss      dword ptr [esp], xmm0
00454664 51                       push       ecx
00454665 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045466a f30f110424               movss      dword ptr [esp], xmm0
0045466f 51                       push       ecx
00454670 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454675 f30f110424               movss      dword ptr [esp], xmm0
0045467a 8d4df4                   lea        ecx, [ebp - 0xc]
0045467d 51                       push       ecx
0045467e 8d55f8                   lea        edx, [ebp - 8]
00454681 52                       push       edx
00454682 e8e91a0000               call       0x456170
00454687 83c418                   add        esp, 0x18
0045468a 8945e8                   mov        dword ptr [ebp - 0x18], eax
0045468d 51                       push       ecx
0045468e f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
00454693 f30f110424               movss      dword ptr [esp], xmm0
00454698 51                       push       ecx
00454699 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
0045469e f30f110424               movss      dword ptr [esp], xmm0
004546a3 51                       push       ecx
004546a4 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004546a9 f30f110424               movss      dword ptr [esp], xmm0
004546ae 51                       push       ecx
004546af f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004546b4 f30f110424               movss      dword ptr [esp], xmm0
004546b9 8d45ec                   lea        eax, [ebp - 0x14]
004546bc 50                       push       eax
004546bd 8d4df0                   lea        ecx, [ebp - 0x10]
004546c0 51                       push       ecx
004546c1 e8aa1a0000               call       0x456170
004546c6 83c418                   add        esp, 0x18
004546c9 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004546cc 837de800                 cmp        dword ptr [ebp - 0x18], 0
004546d0 7553                     jne        0x454725
004546d2 837de400                 cmp        dword ptr [ebp - 0x1c], 0
004546d6 754d                     jne        0x454725
004546d8 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004546dd f30f5c45f4               subss      xmm0, dword ptr [ebp - 0xc]
004546e2 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
004546e7 f30f5c4df0               subss      xmm1, dword ptr [ebp - 0x10]
004546ec f30f5ec1                 divss      xmm0, xmm1
004546f0 8b5508                   mov        edx, dword ptr [ebp + 8]
004546f3 f30f1102                 movss      dword ptr [edx], xmm0
004546f7 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004546fc f30f5c45f4               subss      xmm0, dword ptr [ebp - 0xc]
00454701 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00454706 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
0045470b f30f5c4df0               subss      xmm1, dword ptr [ebp - 0x10]
00454710 f30f5ec1                 divss      xmm0, xmm1
00454714 f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
00454719 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0045471c f30f1100                 movss      dword ptr [eax], xmm0
00454720 e9a6000000               jmp        0x4547cb
00454725 837de800                 cmp        dword ptr [ebp - 0x18], 0
00454729 7454                     je         0x45477f
0045472b 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0045472f 744e                     je         0x45477f
00454731 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454736 f30f5c4520               subss      xmm0, dword ptr [ebp + 0x20]
0045473b 51                       push       ecx
0045473c f30f110424               movss      dword ptr [esp], xmm0
00454741 e83a0fffff               call       0x445680
00454746 83c404                   add        esp, 4
00454749 d95de0                   fstp       dword ptr [ebp - 0x20]
0045474c f30f100514e75600         movss      xmm0, dword ptr [0x56e714]
00454754 0f2f45e0                 comiss     xmm0, dword ptr [ebp - 0x20]
00454758 761f                     jbe        0x454779
0045475a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045475d f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454762 f30f1101                 movss      dword ptr [ecx], xmm0
00454766 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00454769 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045476e f30f1102                 movss      dword ptr [edx], xmm0
00454772 b801000000               mov        eax, 1
00454777 eb57                     jmp        0x4547d0
00454779 33c0                     xor        eax, eax
0045477b eb53                     jmp        0x4547d0
0045477d eb4c                     jmp        0x4547cb
0045477f 837de800                 cmp        dword ptr [ebp - 0x18], 0
00454783 7424                     je         0x4547a9
00454785 8b4508                   mov        eax, dword ptr [ebp + 8]
00454788 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045478d f30f1100                 movss      dword ptr [eax], xmm0
00454791 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00454796 f30f594510               mulss      xmm0, dword ptr [ebp + 0x10]
0045479b f30f5845ec               addss      xmm0, dword ptr [ebp - 0x14]
004547a0 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004547a3 f30f1101                 movss      dword ptr [ecx], xmm0
004547a7 eb22                     jmp        0x4547cb
004547a9 8b5508                   mov        edx, dword ptr [ebp + 8]
004547ac f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004547b1 f30f1102                 movss      dword ptr [edx], xmm0
004547b5 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004547ba f30f594520               mulss      xmm0, dword ptr [ebp + 0x20]
004547bf f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
004547c4 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004547c7 f30f1100                 movss      dword ptr [eax], xmm0
004547cb b801000000               mov        eax, 1
004547d0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004547d3 33cd                     xor        ecx, ebp
004547d5 e814e20e00               call       0x5429ee
004547da 8be5                     mov        esp, ebp
004547dc 5d                       pop        ebp
004547dd c3                       ret        
004547de cc                       int3       
004547df cc                       int3       
