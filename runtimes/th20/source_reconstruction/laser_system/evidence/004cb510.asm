004cb510 55                       push       ebp
004cb511 8bec                     mov        ebp, esp
004cb513 81ec98000000             sub        esp, 0x98
004cb519 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cb51e 33c5                     xor        eax, ebp
004cb520 8945fc                   mov        dword ptr [ebp - 4], eax
004cb523 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004cb526 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004cb52a 7413                     je         0x4cb53f
004cb52c 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004cb52f 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cb536 7407                     je         0x4cb53f
004cb538 33c0                     xor        eax, eax
004cb53a e91d040000               jmp        0x4cb95c
004cb53f c745b800000000           mov        dword ptr [ebp - 0x48], 0
004cb546 c745c000000000           mov        dword ptr [ebp - 0x40], 0
004cb54d 8d4dd8                   lea        ecx, [ebp - 0x28]
004cb550 e8bb78f5ff               call       0x422e10
004cb555 8d4df0                   lea        ecx, [ebp - 0x10]
004cb558 e8b378f5ff               call       0x422e10
004cb55d 8d4de4                   lea        ecx, [ebp - 0x1c]
004cb560 e8ab78f5ff               call       0x422e10
004cb565 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb568 83c158                   add        ecx, 0x58
004cb56b 8b11                     mov        edx, dword ptr [ecx]
004cb56d 8955cc                   mov        dword ptr [ebp - 0x34], edx
004cb570 8b4104                   mov        eax, dword ptr [ecx + 4]
004cb573 8945d0                   mov        dword ptr [ebp - 0x30], eax
004cb576 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cb579 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004cb57c 51                       push       ecx
004cb57d f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004cb585 f30f110424               movss      dword ptr [esp], xmm0
004cb58a 8d5580                   lea        edx, [ebp - 0x80]
004cb58d 52                       push       edx
004cb58e 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004cb591 e85a7af8ff               call       0x452ff0
004cb596 8b08                     mov        ecx, dword ptr [eax]
004cb598 894df0                   mov        dword ptr [ebp - 0x10], ecx
004cb59b 8b5004                   mov        edx, dword ptr [eax + 4]
004cb59e 8955f4                   mov        dword ptr [ebp - 0xc], edx
004cb5a1 8b4008                   mov        eax, dword ptr [eax + 8]
004cb5a4 8945f8                   mov        dword ptr [ebp - 8], eax
004cb5a7 8d4df0                   lea        ecx, [ebp - 0x10]
004cb5aa 51                       push       ecx
004cb5ab 8d9574ffffff             lea        edx, [ebp - 0x8c]
004cb5b1 52                       push       edx
004cb5b2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cb5b5 e886def5ff               call       0x429440
004cb5ba 8b08                     mov        ecx, dword ptr [eax]
004cb5bc 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004cb5bf 8b5004                   mov        edx, dword ptr [eax + 4]
004cb5c2 8955dc                   mov        dword ptr [ebp - 0x24], edx
004cb5c5 8b4008                   mov        eax, dword ptr [eax + 8]
004cb5c8 8945e0                   mov        dword ptr [ebp - 0x20], eax
004cb5cb 8d4df0                   lea        ecx, [ebp - 0x10]
004cb5ce 51                       push       ecx
004cb5cf 8d9568ffffff             lea        edx, [ebp - 0x98]
004cb5d5 52                       push       edx
004cb5d6 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cb5d9 e892dff5ff               call       0x429570
004cb5de 8b08                     mov        ecx, dword ptr [eax]
004cb5e0 894df0                   mov        dword ptr [ebp - 0x10], ecx
004cb5e3 8b5004                   mov        edx, dword ptr [eax + 4]
004cb5e6 8955f4                   mov        dword ptr [ebp - 0xc], edx
004cb5e9 8b4008                   mov        eax, dword ptr [eax + 8]
004cb5ec 8945f8                   mov        dword ptr [ebp - 8], eax
004cb5ef 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb5f2 8b9128130000             mov        edx, dword ptr [ecx + 0x1328]
004cb5f8 8955bc                   mov        dword ptr [ebp - 0x44], edx
004cb5fb c745c000000000           mov        dword ptr [ebp - 0x40], 0
004cb602 eb09                     jmp        0x4cb60d
004cb604 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004cb607 83c001                   add        eax, 1
004cb60a 8945c0                   mov        dword ptr [ebp - 0x40], eax
004cb60d 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb610 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004cb613 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004cb619 0f8d3a030000             jge        0x4cb959
004cb61f 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004cb622 8b08                     mov        ecx, dword ptr [eax]
004cb624 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004cb627 8b5004                   mov        edx, dword ptr [eax + 4]
004cb62a 8955e8                   mov        dword ptr [ebp - 0x18], edx
004cb62d 8b4008                   mov        eax, dword ptr [eax + 8]
004cb630 8945ec                   mov        dword ptr [ebp - 0x14], eax
004cb633 8b4d1c                   mov        ecx, dword ptr [ebp + 0x1c]
004cb636 833900                   cmp        dword ptr [ecx], 0
004cb639 0f8552010000             jne        0x4cb791
004cb63f 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb642 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cb648 e8e370f4ff               call       0x412730
004cb64d 8945b0                   mov        dword ptr [ebp - 0x50], eax
004cb650 6a00                     push       0
004cb652 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004cb655 e806a0fbff               call       0x485660
004cb65a 8945ac                   mov        dword ptr [ebp - 0x54], eax
004cb65d 6a00                     push       0
004cb65f 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004cb662 e819b50000               call       0x4d6b80
004cb667 8bc8                     mov        ecx, eax
004cb669 e862c6f6ff               call       0x437cd0
004cb66e f30f104048               movss      xmm0, dword ptr [eax + 0x48]
004cb673 f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cb67b f30f114598               movss      dword ptr [ebp - 0x68], xmm0
004cb680 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004cb683 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cb689 e8a270f4ff               call       0x412730
004cb68e 8945a8                   mov        dword ptr [ebp - 0x58], eax
004cb691 6a00                     push       0
004cb693 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004cb696 e8c59ffbff               call       0x485660
004cb69b 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004cb69e 6a00                     push       0
004cb6a0 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004cb6a3 e8d8b40000               call       0x4d6b80
004cb6a8 8bc8                     mov        ecx, eax
004cb6aa e821c6f6ff               call       0x437cd0
004cb6af f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
004cb6b4 f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cb6bc f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
004cb6c1 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb6c4 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cb6ca e86170f4ff               call       0x412730
004cb6cf 8945a0                   mov        dword ptr [ebp - 0x60], eax
004cb6d2 6a00                     push       0
004cb6d4 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004cb6d7 e8849ffbff               call       0x485660
004cb6dc 8bc8                     mov        ecx, eax
004cb6de e8ddebfaff               call       0x47a2c0
004cb6e3 f30f104004               movss      xmm0, dword ptr [eax + 4]
004cb6e8 f30f114590               movss      dword ptr [ebp - 0x70], xmm0
004cb6ed 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb6f0 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cb6f6 e83570f4ff               call       0x412730
004cb6fb 89459c                   mov        dword ptr [ebp - 0x64], eax
004cb6fe 6a00                     push       0
004cb700 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004cb703 e8589ffbff               call       0x485660
004cb708 8bc8                     mov        ecx, eax
004cb70a e8b1ebfaff               call       0x47a2c0
004cb70f f30f1000                 movss      xmm0, dword ptr [eax]
004cb713 f30f11458c               movss      dword ptr [ebp - 0x74], xmm0
004cb718 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cb720 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cb728 51                       push       ecx
004cb729 f30f110424               movss      dword ptr [esp], xmm0
004cb72e 51                       push       ecx
004cb72f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004cb734 f30f110424               movss      dword ptr [esp], xmm0
004cb739 51                       push       ecx
004cb73a f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004cb73f f30f110424               movss      dword ptr [esp], xmm0
004cb744 51                       push       ecx
004cb745 0f57c0                   xorps      xmm0, xmm0
004cb748 f30f110424               movss      dword ptr [esp], xmm0
004cb74d 51                       push       ecx
004cb74e f30f104598               movss      xmm0, dword ptr [ebp - 0x68]
004cb753 f30f110424               movss      dword ptr [esp], xmm0
004cb758 51                       push       ecx
004cb759 f30f104594               movss      xmm0, dword ptr [ebp - 0x6c]
004cb75e f30f110424               movss      dword ptr [esp], xmm0
004cb763 51                       push       ecx
004cb764 f30f104590               movss      xmm0, dword ptr [ebp - 0x70]
004cb769 f30f110424               movss      dword ptr [esp], xmm0
004cb76e 51                       push       ecx
004cb76f f30f10458c               movss      xmm0, dword ptr [ebp - 0x74]
004cb774 f30f110424               movss      dword ptr [esp], xmm0
004cb779 e892bef8ff               call       0x457610
004cb77e 83c420                   add        esp, 0x20
004cb781 0fb6c0                   movzx      eax, al
004cb784 85c0                     test       eax, eax
004cb786 7409                     je         0x4cb791
004cb788 8b4d1c                   mov        ecx, dword ptr [ebp + 0x1c]
004cb78b c70101000000             mov        dword ptr [ecx], 1
004cb791 51                       push       ecx
004cb792 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004cb797 f30f110424               movss      dword ptr [esp], xmm0
004cb79c 51                       push       ecx
004cb79d f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004cb7a2 f30f110424               movss      dword ptr [esp], xmm0
004cb7a7 51                       push       ecx
004cb7a8 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cb7ad f30f110424               movss      dword ptr [esp], xmm0
004cb7b2 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004cb7b5 51                       push       ecx
004cb7b6 f30f104204               movss      xmm0, dword ptr [edx + 4]
004cb7bb f30f110424               movss      dword ptr [esp], xmm0
004cb7c0 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004cb7c3 51                       push       ecx
004cb7c4 f30f1000                 movss      xmm0, dword ptr [eax]
004cb7c8 f30f110424               movss      dword ptr [esp], xmm0
004cb7cd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cb7d0 51                       push       ecx
004cb7d1 f30f104104               movss      xmm0, dword ptr [ecx + 4]
004cb7d6 f30f110424               movss      dword ptr [esp], xmm0
004cb7db 8b5508                   mov        edx, dword ptr [ebp + 8]
004cb7de 51                       push       ecx
004cb7df f30f1002                 movss      xmm0, dword ptr [edx]
004cb7e3 f30f110424               movss      dword ptr [esp], xmm0
004cb7e8 e8a3d6f8ff               call       0x458e90
004cb7ed 83c41c                   add        esp, 0x1c
004cb7f0 0fb6c0                   movzx      eax, al
004cb7f3 85c0                     test       eax, eax
004cb7f5 750a                     jne        0x4cb801
004cb7f7 e94f010000               jmp        0x4cb94b
004cb7fc e94a010000               jmp        0x4cb94b
004cb801 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004cb804 83c101                   add        ecx, 1
004cb807 894db8                   mov        dword ptr [ebp - 0x48], ecx
004cb80a c745c40f000000           mov        dword ptr [ebp - 0x3c], 0xf
004cb811 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb814 f30f10427c               movss      xmm0, dword ptr [edx + 0x7c]
004cb819 0f2f05a8f75600           comiss     xmm0, dword ptr [0x56f7a8]
004cb820 7209                     jb         0x4cb82b
004cb822 6b45c403                 imul       eax, dword ptr [ebp - 0x3c], 3
004cb826 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004cb829 eb5a                     jmp        0x4cb885
004cb82b 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb82e f30f10417c               movss      xmm0, dword ptr [ecx + 0x7c]
004cb833 0f2f05e0c85600           comiss     xmm0, dword ptr [0x56c8e0]
004cb83a 7249                     jb         0x4cb885
004cb83c 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb83f f30f1005a8f75600         movss      xmm0, dword ptr [0x56f7a8]
004cb847 0f2f427c                 comiss     xmm0, dword ptr [edx + 0x7c]
004cb84b 7638                     jbe        0x4cb885
004cb84d f30f2a45c4               cvtsi2ss   xmm0, dword ptr [ebp - 0x3c]
004cb852 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004cb855 f30f10487c               movss      xmm1, dword ptr [eax + 0x7c]
004cb85a f30f5c0de0c85600         subss      xmm1, dword ptr [0x56c8e0]
004cb862 f30f5e0dbcd75600         divss      xmm1, dword ptr [0x56d7bc]
004cb86a f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
004cb872 f30f580dccc85600         addss      xmm1, dword ptr [0x56c8cc]
004cb87a f30f59c1                 mulss      xmm0, xmm1
004cb87e f30f2cc8                 cvttss2si  ecx, xmm0
004cb882 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004cb885 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb888 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004cb88d 0f2f057cfe5600           comiss     xmm0, dword ptr [0x56fe7c]
004cb894 723a                     jb         0x4cb8d0
004cb896 f30f2a45c4               cvtsi2ss   xmm0, dword ptr [ebp - 0x3c]
004cb89b f30f100d7cfe5600         movss      xmm1, dword ptr [0x56fe7c]
004cb8a3 f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cb8ab f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cb8b3 f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cb8bb f30f58c1                 addss      xmm0, xmm1
004cb8bf f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cb8c7 f30f2cc0                 cvttss2si  eax, xmm0
004cb8cb 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004cb8ce eb5a                     jmp        0x4cb92a
004cb8d0 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cb8d3 f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004cb8d8 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004cb8df 7249                     jb         0x4cb92a
004cb8e1 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb8e4 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
004cb8ec 0f2f4278                 comiss     xmm0, dword ptr [edx + 0x78]
004cb8f0 7638                     jbe        0x4cb92a
004cb8f2 f30f2a45c4               cvtsi2ss   xmm0, dword ptr [ebp - 0x3c]
004cb8f7 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004cb8fa f30f104878               movss      xmm1, dword ptr [eax + 0x78]
004cb8ff f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cb907 f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cb90f f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cb917 f30f58c1                 addss      xmm0, xmm1
004cb91b f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cb923 f30f2cc8                 cvttss2si  ecx, xmm0
004cb927 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004cb92a 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004cb92d 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cb933 e85811f5ff               call       0x41ca90
004cb938 83c04c                   add        eax, 0x4c
004cb93b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004cb93e 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
004cb941 8b08                     mov        ecx, dword ptr [eax]
004cb943 034dc4                   add        ecx, dword ptr [ebp - 0x3c]
004cb946 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
004cb949 890a                     mov        dword ptr [edx], ecx
004cb94b 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004cb94e 83c020                   add        eax, 0x20
004cb951 8945bc                   mov        dword ptr [ebp - 0x44], eax
004cb954 e9abfcffff               jmp        0x4cb604
004cb959 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004cb95c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cb95f 33cd                     xor        ecx, ebp
004cb961 e888700700               call       0x5429ee
004cb966 8be5                     mov        esp, ebp
004cb968 5d                       pop        ebp
004cb969 c21800                   ret        0x18
004cb96c cc                       int3       
004cb96d cc                       int3       
004cb96e cc                       int3       
004cb96f cc                       int3       
