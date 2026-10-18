00453500 55                       push       ebp
00453501 8bec                     mov        ebp, esp
00453503 83ec28                   sub        esp, 0x28
00453506 56                       push       esi
00453507 57                       push       edi
00453508 837d1400                 cmp        dword ptr [ebp + 0x14], 0
0045350c 7f40                     jg         0x45354e
0045350e 7c06                     jl         0x453516
00453510 837d1000                 cmp        dword ptr [ebp + 0x10], 0
00453514 7338                     jae        0x45354e
00453516 6aff                     push       -1
00453518 6aff                     push       -1
0045351a 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0045351d 50                       push       eax
0045351e 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453521 51                       push       ecx
00453522 e8d9fb0e00               call       0x543100
00453527 894510                   mov        dword ptr [ebp + 0x10], eax
0045352a 895514                   mov        dword ptr [ebp + 0x14], edx
0045352d ba01000000               mov        edx, 1
00453532 6bc200                   imul       eax, edx, 0
00453535 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00453538 c604012d                 mov        byte ptr [ecx + eax], 0x2d
0045353c 8b5508                   mov        edx, dword ptr [ebp + 8]
0045353f 83c201                   add        edx, 1
00453542 895508                   mov        dword ptr [ebp + 8], edx
00453545 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00453548 83e801                   sub        eax, 1
0045354b 89450c                   mov        dword ptr [ebp + 0xc], eax
0045354e 837d1400                 cmp        dword ptr [ebp + 0x14], 0
00453552 7f4a                     jg         0x45359e
00453554 7c09                     jl         0x45355f
00453556 817d10e8030000           cmp        dword ptr [ebp + 0x10], 0x3e8
0045355d 733f                     jae        0x45359e
0045355f 6a0a                     push       0xa
00453561 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00453564 51                       push       ecx
00453565 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00453568 52                       push       edx
00453569 8b4508                   mov        eax, dword ptr [ebp + 8]
0045356c 03450c                   add        eax, dword ptr [ebp + 0xc]
0045356f 50                       push       eax
00453570 b901000000               mov        ecx, 1
00453575 6bd100                   imul       edx, ecx, 0
00453578 035508                   add        edx, dword ptr [ebp + 8]
0045357b 52                       push       edx
0045357c e8bf5c0000               call       0x459240
00453581 83c414                   add        esp, 0x14
00453584 8945d8                   mov        dword ptr [ebp - 0x28], eax
00453587 8955dc                   mov        dword ptr [ebp - 0x24], edx
0045358a b801000000               mov        eax, 1
0045358f 6bc800                   imul       ecx, eax, 0
00453592 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00453595 c6040a00                 mov        byte ptr [edx + ecx], 0
00453599 e912050000               jmp        0x453ab0
0045359e 837d1400                 cmp        dword ptr [ebp + 0x14], 0
004535a2 0f8fb1000000             jg         0x453659
004535a8 7c0d                     jl         0x4535b7
004535aa 817d1040420f00           cmp        dword ptr [ebp + 0x10], 0xf4240
004535b1 0f83a2000000             jae        0x453659
004535b7 6a00                     push       0
004535b9 68e8030000               push       0x3e8
004535be 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004535c1 50                       push       eax
004535c2 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004535c5 51                       push       ecx
004535c6 e885fa0e00               call       0x543050
004535cb 6a00                     push       0
004535cd 6a0a                     push       0xa
004535cf 52                       push       edx
004535d0 50                       push       eax
004535d1 e82afb0e00               call       0x543100
004535d6 6a00                     push       0
004535d8 68e8030000               push       0x3e8
004535dd 52                       push       edx
004535de 50                       push       eax
004535df e81cfb0e00               call       0x543100
004535e4 8bf0                     mov        esi, eax
004535e6 8bfa                     mov        edi, edx
004535e8 6a00                     push       0
004535ea 68e8030000               push       0x3e8
004535ef 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004535f2 52                       push       edx
004535f3 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004535f6 50                       push       eax
004535f7 e844fb0e00               call       0x543140
004535fc 03f0                     add        esi, eax
004535fe 13fa                     adc        edi, edx
00453600 897510                   mov        dword ptr [ebp + 0x10], esi
00453603 897d14                   mov        dword ptr [ebp + 0x14], edi
00453606 6a0a                     push       0xa
00453608 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0045360b 51                       push       ecx
0045360c 8b5508                   mov        edx, dword ptr [ebp + 8]
0045360f 03550c                   add        edx, dword ptr [ebp + 0xc]
00453612 52                       push       edx
00453613 b801000000               mov        eax, 1
00453618 6bc800                   imul       ecx, eax, 0
0045361b 034d08                   add        ecx, dword ptr [ebp + 8]
0045361e 51                       push       ecx
0045361f e8ec5b0000               call       0x459210
00453624 83c410                   add        esp, 0x10
00453627 8945e0                   mov        dword ptr [ebp - 0x20], eax
0045362a 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0045362d ba01000000               mov        edx, 1
00453632 6bc200                   imul       eax, edx, 0
00453635 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00453638 c6040100                 mov        byte ptr [ecx + eax], 0
0045363c ba01000000               mov        edx, 1
00453641 6bc200                   imul       eax, edx, 0
00453644 034508                   add        eax, dword ptr [ebp + 8]
00453647 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0045364a 2bc8                     sub        ecx, eax
0045364c 8b5508                   mov        edx, dword ptr [ebp + 8]
0045364f c6440afc2c               mov        byte ptr [edx + ecx - 4], 0x2c
00453654 e957040000               jmp        0x453ab0
00453659 837d1400                 cmp        dword ptr [ebp + 0x14], 0
0045365d 0f8f1a010000             jg         0x45377d
00453663 7c0d                     jl         0x453672
00453665 817d1000ca9a3b           cmp        dword ptr [ebp + 0x10], 0x3b9aca00
0045366c 0f830b010000             jae        0x45377d
00453672 6a00                     push       0
00453674 6840420f00               push       0xf4240
00453679 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0045367c 50                       push       eax
0045367d 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453680 51                       push       ecx
00453681 e8caf90e00               call       0x543050
00453686 6a00                     push       0
00453688 6a64                     push       0x64
0045368a 52                       push       edx
0045368b 50                       push       eax
0045368c e86ffa0e00               call       0x543100
00453691 6a00                     push       0
00453693 68e8030000               push       0x3e8
00453698 52                       push       edx
00453699 50                       push       eax
0045369a e861fa0e00               call       0x543100
0045369f 6a00                     push       0
004536a1 68e8030000               push       0x3e8
004536a6 52                       push       edx
004536a7 50                       push       eax
004536a8 e853fa0e00               call       0x543100
004536ad 8bf0                     mov        esi, eax
004536af 8bfa                     mov        edi, edx
004536b1 6a00                     push       0
004536b3 68e8030000               push       0x3e8
004536b8 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004536bb 52                       push       edx
004536bc 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004536bf 50                       push       eax
004536c0 e88bf90e00               call       0x543050
004536c5 6a00                     push       0
004536c7 68e8030000               push       0x3e8
004536cc 52                       push       edx
004536cd 50                       push       eax
004536ce e86dfa0e00               call       0x543140
004536d3 6a00                     push       0
004536d5 6a0a                     push       0xa
004536d7 52                       push       edx
004536d8 50                       push       eax
004536d9 e822fa0e00               call       0x543100
004536de 6a00                     push       0
004536e0 68e8030000               push       0x3e8
004536e5 52                       push       edx
004536e6 50                       push       eax
004536e7 e814fa0e00               call       0x543100
004536ec 03f0                     add        esi, eax
004536ee 13fa                     adc        edi, edx
004536f0 6a00                     push       0
004536f2 68e8030000               push       0x3e8
004536f7 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
004536fa 51                       push       ecx
004536fb 8b5510                   mov        edx, dword ptr [ebp + 0x10]
004536fe 52                       push       edx
004536ff e83cfa0e00               call       0x543140
00453704 03f0                     add        esi, eax
00453706 13fa                     adc        edi, edx
00453708 897510                   mov        dword ptr [ebp + 0x10], esi
0045370b 897d14                   mov        dword ptr [ebp + 0x14], edi
0045370e 6a0a                     push       0xa
00453710 8b4514                   mov        eax, dword ptr [ebp + 0x14]
00453713 50                       push       eax
00453714 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453717 51                       push       ecx
00453718 8b5508                   mov        edx, dword ptr [ebp + 8]
0045371b 03550c                   add        edx, dword ptr [ebp + 0xc]
0045371e 52                       push       edx
0045371f b801000000               mov        eax, 1
00453724 6bc800                   imul       ecx, eax, 0
00453727 034d08                   add        ecx, dword ptr [ebp + 8]
0045372a 51                       push       ecx
0045372b e8105b0000               call       0x459240
00453730 83c414                   add        esp, 0x14
00453733 8945e8                   mov        dword ptr [ebp - 0x18], eax
00453736 8955ec                   mov        dword ptr [ebp - 0x14], edx
00453739 ba01000000               mov        edx, 1
0045373e 6bc200                   imul       eax, edx, 0
00453741 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00453744 c6040100                 mov        byte ptr [ecx + eax], 0
00453748 ba01000000               mov        edx, 1
0045374d 6bc200                   imul       eax, edx, 0
00453750 034508                   add        eax, dword ptr [ebp + 8]
00453753 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00453756 2bc8                     sub        ecx, eax
00453758 8b5508                   mov        edx, dword ptr [ebp + 8]
0045375b c6440afc2c               mov        byte ptr [edx + ecx - 4], 0x2c
00453760 b801000000               mov        eax, 1
00453765 6bc800                   imul       ecx, eax, 0
00453768 034d08                   add        ecx, dword ptr [ebp + 8]
0045376b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0045376e 2bd1                     sub        edx, ecx
00453770 8b4508                   mov        eax, dword ptr [ebp + 8]
00453773 c64410f82c               mov        byte ptr [eax + edx - 8], 0x2c
00453778 e933030000               jmp        0x453ab0
0045377d 817d14e8000000           cmp        dword ptr [ebp + 0x14], 0xe8
00453784 0f8f66010000             jg         0x4538f0
0045378a 7c0d                     jl         0x453799
0045378c 817d100010a5d4           cmp        dword ptr [ebp + 0x10], 0xd4a51000
00453793 0f8357010000             jae        0x4538f0
00453799 6a00                     push       0
0045379b 6800ca9a3b               push       0x3b9aca00
004537a0 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
004537a3 51                       push       ecx
004537a4 8b5510                   mov        edx, dword ptr [ebp + 0x10]
004537a7 52                       push       edx
004537a8 e8a3f80e00               call       0x543050
004537ad 6a00                     push       0
004537af 68e8030000               push       0x3e8
004537b4 52                       push       edx
004537b5 50                       push       eax
004537b6 e845f90e00               call       0x543100
004537bb 6a00                     push       0
004537bd 6800ca9a3b               push       0x3b9aca00
004537c2 52                       push       edx
004537c3 50                       push       eax
004537c4 e837f90e00               call       0x543100
004537c9 8bf0                     mov        esi, eax
004537cb 8bfa                     mov        edi, edx
004537cd 6a00                     push       0
004537cf 6840420f00               push       0xf4240
004537d4 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004537d7 50                       push       eax
004537d8 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004537db 51                       push       ecx
004537dc e86ff80e00               call       0x543050
004537e1 6a00                     push       0
004537e3 68e8030000               push       0x3e8
004537e8 52                       push       edx
004537e9 50                       push       eax
004537ea e851f90e00               call       0x543140
004537ef 6a00                     push       0
004537f1 6a64                     push       0x64
004537f3 52                       push       edx
004537f4 50                       push       eax
004537f5 e806f90e00               call       0x543100
004537fa 6a00                     push       0
004537fc 6840420f00               push       0xf4240
00453801 52                       push       edx
00453802 50                       push       eax
00453803 e8f8f80e00               call       0x543100
00453808 03f0                     add        esi, eax
0045380a 13fa                     adc        edi, edx
0045380c 6a00                     push       0
0045380e 68e8030000               push       0x3e8
00453813 8b5514                   mov        edx, dword ptr [ebp + 0x14]
00453816 52                       push       edx
00453817 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0045381a 50                       push       eax
0045381b e830f80e00               call       0x543050
00453820 6a00                     push       0
00453822 68e8030000               push       0x3e8
00453827 52                       push       edx
00453828 50                       push       eax
00453829 e812f90e00               call       0x543140
0045382e 6a00                     push       0
00453830 6a0a                     push       0xa
00453832 52                       push       edx
00453833 50                       push       eax
00453834 e8c7f80e00               call       0x543100
00453839 6a00                     push       0
0045383b 68e8030000               push       0x3e8
00453840 52                       push       edx
00453841 50                       push       eax
00453842 e8b9f80e00               call       0x543100
00453847 03f0                     add        esi, eax
00453849 13fa                     adc        edi, edx
0045384b 6a00                     push       0
0045384d 68e8030000               push       0x3e8
00453852 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00453855 51                       push       ecx
00453856 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00453859 52                       push       edx
0045385a e8e1f80e00               call       0x543140
0045385f 03f0                     add        esi, eax
00453861 13fa                     adc        edi, edx
00453863 897510                   mov        dword ptr [ebp + 0x10], esi
00453866 897d14                   mov        dword ptr [ebp + 0x14], edi
00453869 6a0a                     push       0xa
0045386b 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0045386e 50                       push       eax
0045386f 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453872 51                       push       ecx
00453873 8b5508                   mov        edx, dword ptr [ebp + 8]
00453876 03550c                   add        edx, dword ptr [ebp + 0xc]
00453879 52                       push       edx
0045387a b801000000               mov        eax, 1
0045387f 6bc800                   imul       ecx, eax, 0
00453882 034d08                   add        ecx, dword ptr [ebp + 8]
00453885 51                       push       ecx
00453886 e8b5590000               call       0x459240
0045388b 83c414                   add        esp, 0x14
0045388e 8945f0                   mov        dword ptr [ebp - 0x10], eax
00453891 8955f4                   mov        dword ptr [ebp - 0xc], edx
00453894 ba01000000               mov        edx, 1
00453899 6bc200                   imul       eax, edx, 0
0045389c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045389f c6040100                 mov        byte ptr [ecx + eax], 0
004538a3 ba01000000               mov        edx, 1
004538a8 6bc200                   imul       eax, edx, 0
004538ab 034508                   add        eax, dword ptr [ebp + 8]
004538ae 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004538b1 2bc8                     sub        ecx, eax
004538b3 8b5508                   mov        edx, dword ptr [ebp + 8]
004538b6 c6440afc2c               mov        byte ptr [edx + ecx - 4], 0x2c
004538bb b801000000               mov        eax, 1
004538c0 6bc800                   imul       ecx, eax, 0
004538c3 034d08                   add        ecx, dword ptr [ebp + 8]
004538c6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004538c9 2bd1                     sub        edx, ecx
004538cb 8b4508                   mov        eax, dword ptr [ebp + 8]
004538ce c64410f82c               mov        byte ptr [eax + edx - 8], 0x2c
004538d3 b901000000               mov        ecx, 1
004538d8 6bd100                   imul       edx, ecx, 0
004538db 035508                   add        edx, dword ptr [ebp + 8]
004538de 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004538e1 2bc2                     sub        eax, edx
004538e3 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004538e6 c64401f42c               mov        byte ptr [ecx + eax - 0xc], 0x2c
004538eb e9c0010000               jmp        0x453ab0
004538f0 68e8000000               push       0xe8
004538f5 680010a5d4               push       0xd4a51000
004538fa 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004538fd 52                       push       edx
004538fe 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00453901 50                       push       eax
00453902 e849f70e00               call       0x543050
00453907 6a00                     push       0
00453909 68e8030000               push       0x3e8
0045390e 52                       push       edx
0045390f 50                       push       eax
00453910 e82bf80e00               call       0x543140
00453915 6a00                     push       0
00453917 6810270000               push       0x2710
0045391c 52                       push       edx
0045391d 50                       push       eax
0045391e e8ddf70e00               call       0x543100
00453923 68e8000000               push       0xe8
00453928 680010a5d4               push       0xd4a51000
0045392d 52                       push       edx
0045392e 50                       push       eax
0045392f e8ccf70e00               call       0x543100
00453934 8bf0                     mov        esi, eax
00453936 8bfa                     mov        edi, edx
00453938 6a00                     push       0
0045393a 6800ca9a3b               push       0x3b9aca00
0045393f 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00453942 51                       push       ecx
00453943 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00453946 52                       push       edx
00453947 e804f70e00               call       0x543050
0045394c 6a00                     push       0
0045394e 68e8030000               push       0x3e8
00453953 52                       push       edx
00453954 50                       push       eax
00453955 e8e6f70e00               call       0x543140
0045395a 6a00                     push       0
0045395c 68e8030000               push       0x3e8
00453961 52                       push       edx
00453962 50                       push       eax
00453963 e898f70e00               call       0x543100
00453968 6a00                     push       0
0045396a 6800ca9a3b               push       0x3b9aca00
0045396f 52                       push       edx
00453970 50                       push       eax
00453971 e88af70e00               call       0x543100
00453976 03f0                     add        esi, eax
00453978 13fa                     adc        edi, edx
0045397a 6a00                     push       0
0045397c 6840420f00               push       0xf4240
00453981 8b4514                   mov        eax, dword ptr [ebp + 0x14]
00453984 50                       push       eax
00453985 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453988 51                       push       ecx
00453989 e8c2f60e00               call       0x543050
0045398e 6a00                     push       0
00453990 68e8030000               push       0x3e8
00453995 52                       push       edx
00453996 50                       push       eax
00453997 e8a4f70e00               call       0x543140
0045399c 6a00                     push       0
0045399e 6a64                     push       0x64
004539a0 52                       push       edx
004539a1 50                       push       eax
004539a2 e859f70e00               call       0x543100
004539a7 6a00                     push       0
004539a9 6840420f00               push       0xf4240
004539ae 52                       push       edx
004539af 50                       push       eax
004539b0 e84bf70e00               call       0x543100
004539b5 03f0                     add        esi, eax
004539b7 13fa                     adc        edi, edx
004539b9 6a00                     push       0
004539bb 68e8030000               push       0x3e8
004539c0 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004539c3 52                       push       edx
004539c4 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004539c7 50                       push       eax
004539c8 e883f60e00               call       0x543050
004539cd 6a00                     push       0
004539cf 68e8030000               push       0x3e8
004539d4 52                       push       edx
004539d5 50                       push       eax
004539d6 e865f70e00               call       0x543140
004539db 6a00                     push       0
004539dd 6a0a                     push       0xa
004539df 52                       push       edx
004539e0 50                       push       eax
004539e1 e81af70e00               call       0x543100
004539e6 6a00                     push       0
004539e8 68e8030000               push       0x3e8
004539ed 52                       push       edx
004539ee 50                       push       eax
004539ef e80cf70e00               call       0x543100
004539f4 03f0                     add        esi, eax
004539f6 13fa                     adc        edi, edx
004539f8 6a00                     push       0
004539fa 68e8030000               push       0x3e8
004539ff 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00453a02 51                       push       ecx
00453a03 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00453a06 52                       push       edx
00453a07 e834f70e00               call       0x543140
00453a0c 03f0                     add        esi, eax
00453a0e 13fa                     adc        edi, edx
00453a10 897510                   mov        dword ptr [ebp + 0x10], esi
00453a13 897d14                   mov        dword ptr [ebp + 0x14], edi
00453a16 6a0a                     push       0xa
00453a18 8b4514                   mov        eax, dword ptr [ebp + 0x14]
00453a1b 50                       push       eax
00453a1c 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00453a1f 51                       push       ecx
00453a20 8b5508                   mov        edx, dword ptr [ebp + 8]
00453a23 03550c                   add        edx, dword ptr [ebp + 0xc]
00453a26 52                       push       edx
00453a27 b801000000               mov        eax, 1
00453a2c 6bc800                   imul       ecx, eax, 0
00453a2f 034d08                   add        ecx, dword ptr [ebp + 8]
00453a32 51                       push       ecx
00453a33 e808580000               call       0x459240
00453a38 83c414                   add        esp, 0x14
00453a3b 8945f8                   mov        dword ptr [ebp - 8], eax
00453a3e 8955fc                   mov        dword ptr [ebp - 4], edx
00453a41 ba01000000               mov        edx, 1
00453a46 6bc200                   imul       eax, edx, 0
00453a49 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00453a4c c6040100                 mov        byte ptr [ecx + eax], 0
00453a50 ba01000000               mov        edx, 1
00453a55 6bc200                   imul       eax, edx, 0
00453a58 034508                   add        eax, dword ptr [ebp + 8]
00453a5b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00453a5e 2bc8                     sub        ecx, eax
00453a60 8b5508                   mov        edx, dword ptr [ebp + 8]
00453a63 c6440afc2c               mov        byte ptr [edx + ecx - 4], 0x2c
00453a68 b801000000               mov        eax, 1
00453a6d 6bc800                   imul       ecx, eax, 0
00453a70 034d08                   add        ecx, dword ptr [ebp + 8]
00453a73 8b55f8                   mov        edx, dword ptr [ebp - 8]
00453a76 2bd1                     sub        edx, ecx
00453a78 8b4508                   mov        eax, dword ptr [ebp + 8]
00453a7b c64410f82c               mov        byte ptr [eax + edx - 8], 0x2c
00453a80 b901000000               mov        ecx, 1
00453a85 6bd100                   imul       edx, ecx, 0
00453a88 035508                   add        edx, dword ptr [ebp + 8]
00453a8b 8b45f8                   mov        eax, dword ptr [ebp - 8]
00453a8e 2bc2                     sub        eax, edx
00453a90 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00453a93 c64401f42c               mov        byte ptr [ecx + eax - 0xc], 0x2c
00453a98 ba01000000               mov        edx, 1
00453a9d 6bc200                   imul       eax, edx, 0
00453aa0 034508                   add        eax, dword ptr [ebp + 8]
00453aa3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00453aa6 2bc8                     sub        ecx, eax
00453aa8 8b5508                   mov        edx, dword ptr [ebp + 8]
00453aab c6440af02c               mov        byte ptr [edx + ecx - 0x10], 0x2c
00453ab0 5f                       pop        edi
00453ab1 5e                       pop        esi
00453ab2 8be5                     mov        esp, ebp
00453ab4 5d                       pop        ebp
00453ab5 c3                       ret        
00453ab6 cc                       int3       
00453ab7 cc                       int3       
00453ab8 cc                       int3       
00453ab9 cc                       int3       
00453aba cc                       int3       
00453abb cc                       int3       
00453abc cc                       int3       
00453abd cc                       int3       
00453abe cc                       int3       
00453abf cc                       int3       
