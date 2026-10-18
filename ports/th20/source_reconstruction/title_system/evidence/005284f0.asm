005284f0 55                       push       ebp
005284f1 8bec                     mov        ebp, esp
005284f3 81ec88000000             sub        esp, 0x88
005284f9 894dfc                   mov        dword ptr [ebp - 4], ecx
005284fc c78578ffffff03000000     mov        dword ptr [ebp - 0x88], 3
00528506 c7857cffffff03000000     mov        dword ptr [ebp - 0x84], 3
00528510 c7458004000000           mov        dword ptr [ebp - 0x80], 4
00528517 c7458404000000           mov        dword ptr [ebp - 0x7c], 4
0052851e c7458804000000           mov        dword ptr [ebp - 0x78], 4
00528525 c7458c07000000           mov        dword ptr [ebp - 0x74], 7
0052852c c745900d000000           mov        dword ptr [ebp - 0x70], 0xd
00528533 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528536 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00528539 894dec                   mov        dword ptr [ebp - 0x14], ecx
0052853c 837dec04                 cmp        dword ptr [ebp - 0x14], 4
00528540 0f87b6040000             ja         0x5289fc
00528546 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00528549 ff24953c8a5200           jmp        dword ptr [edx*4 + 0x528a3c]
00528550 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528553 83c024                   add        eax, 0x24
00528556 8945d8                   mov        dword ptr [ebp - 0x28], eax
00528559 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052855c 8b91d8580000             mov        edx, dword ptr [ecx + 0x58d8]
00528562 8b849578ffffff           mov        eax, dword ptr [ebp + edx*4 - 0x88]
00528569 50                       push       eax
0052856a 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0052856d e86e3ff7ff               call       0x49c4e0
00528572 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528575 83c124                   add        ecx, 0x24
00528578 e823e3fbff               call       0x4e68a0
0052857d 90                       nop        
0052857e 6a25                     push       0x25
00528580 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528583 e8c8410000               call       0x52c750
00528588 85c0                     test       eax, eax
0052858a 750b                     jne        0x528597
0052858c 6a25                     push       0x25
0052858e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528591 e8ea490000               call       0x52cf80
00528596 90                       nop        
00528597 6a56                     push       0x56
00528599 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052859c e8af410000               call       0x52c750
005285a1 85c0                     test       eax, eax
005285a3 750b                     jne        0x5285b0
005285a5 6a56                     push       0x56
005285a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005285aa e8d1490000               call       0x52cf80
005285af 90                       nop        
005285b0 6a01                     push       1
005285b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005285b5 e896480000               call       0x52ce50
005285ba 90                       nop        
005285bb c745f800000000           mov        dword ptr [ebp - 8], 0
005285c2 eb09                     jmp        0x5285cd
005285c4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005285c7 83c101                   add        ecx, 1
005285ca 894df8                   mov        dword ptr [ebp - 8], ecx
005285cd 8b55fc                   mov        edx, dword ptr [ebp - 4]
005285d0 8b82d8580000             mov        eax, dword ptr [edx + 0x58d8]
005285d6 b90d000000               mov        ecx, 0xd
005285db 2b8c8578ffffff           sub        ecx, dword ptr [ebp + eax*4 - 0x88]
005285e2 394df8                   cmp        dword ptr [ebp - 8], ecx
005285e5 7d1c                     jge        0x528603
005285e7 ba20000000               mov        edx, 0x20
005285ec 2b55f8                   sub        edx, dword ptr [ebp - 8]
005285ef 83c207                   add        edx, 7
005285f2 0fbfc2                   movsx      eax, dx
005285f5 50                       push       eax
005285f6 6a56                     push       0x56
005285f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005285fb e810470000               call       0x52cd10
00528600 90                       nop        
00528601 ebc1                     jmp        0x5285c4
00528603 833d28615c0005           cmp        dword ptr [0x5c6128], 5
0052860a 755c                     jne        0x528668
0052860c 8b0d5c0a5b00             mov        ecx, dword ptr [0x5b0a5c]
00528612 51                       push       ecx
00528613 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528616 83c124                   add        ecx, 0x24
00528619 e83267f9ff               call       0x4bed50
0052861e c7055c0a5b00ffffffff     mov        dword ptr [0x5b0a5c], 0xffffffff
00528628 6a56                     push       0x56
0052862a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052862d e85e460000               call       0x52cc90
00528632 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528635 83c224                   add        edx, 0x24
00528638 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0052863b 6a07                     push       7
0052863d 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00528640 e83b5affff               call       0x51e080
00528645 98                       cwde       
00528646 50                       push       eax
00528647 6a56                     push       0x56
00528649 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052864c e8bf460000               call       0x52cd10
00528651 6a06                     push       6
00528653 6a56                     push       0x56
00528655 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528658 e8b3460000               call       0x52cd10
0052865d 90                       nop        
0052865e e994020000               jmp        0x5288f7
00528663 e98f020000               jmp        0x5288f7
00528668 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052866b 81c154010000             add        ecx, 0x154
00528671 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00528674 6a0a                     push       0xa
00528676 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00528679 e8f289f3ff               call       0x461070
0052867e 0fb6d0                   movzx      edx, al
00528681 85d2                     test       edx, edx
00528683 7436                     je         0x5286bb
00528685 6a02                     push       2
00528687 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052868a e8c1470000               call       0x52ce50
0052868f 6a56                     push       0x56
00528691 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528694 e8f7450000               call       0x52cc90
00528699 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052869c 83c024                   add        eax, 0x24
0052869f 8945cc                   mov        dword ptr [ebp - 0x34], eax
005286a2 6a07                     push       7
005286a4 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
005286a7 e8d459ffff               call       0x51e080
005286ac 0fbfc8                   movsx      ecx, ax
005286af 51                       push       ecx
005286b0 6a56                     push       0x56
005286b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005286b5 e816460000               call       0x52ccd0
005286ba 90                       nop        
005286bb e93c030000               jmp        0x5289fc
005286c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005286c3 83c124                   add        ecx, 0x24
005286c6 e83575f9ff               call       0x4bfc00
005286cb 90                       nop        
005286cc 33d2                     xor        edx, edx
005286ce 7531                     jne        0x528701
005286d0 b804000000               mov        eax, 4
005286d5 6bc800                   imul       ecx, eax, 0
005286d8 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005286df 7420                     je         0x528701
005286e1 ba04000000               mov        edx, 4
005286e6 6bc200                   imul       eax, edx, 0
005286e9 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005286ef 894dc8                   mov        dword ptr [ebp - 0x38], ecx
005286f2 6a10                     push       0x10
005286f4 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005286f7 e8841befff               call       0x41a280
005286fc 8945e8                   mov        dword ptr [ebp - 0x18], eax
005286ff eb07                     jmp        0x528708
00528701 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00528708 837de800                 cmp        dword ptr [ebp - 0x18], 0
0052870c 7414                     je         0x528722
0052870e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528711 83c224                   add        edx, 0x24
00528714 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00528717 6a00                     push       0
00528719 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0052871c e81f69f9ff               call       0x4bf040
00528721 90                       nop        
00528722 33c0                     xor        eax, eax
00528724 7531                     jne        0x528757
00528726 b904000000               mov        ecx, 4
0052872b 6bd100                   imul       edx, ecx, 0
0052872e 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00528735 7420                     je         0x528757
00528737 b804000000               mov        eax, 4
0052873c 6bc800                   imul       ecx, eax, 0
0052873f 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00528745 8955c0                   mov        dword ptr [ebp - 0x40], edx
00528748 6a20                     push       0x20
0052874a 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0052874d e82e1befff               call       0x41a280
00528752 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00528755 eb07                     jmp        0x52875e
00528757 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0052875e 837de400                 cmp        dword ptr [ebp - 0x1c], 0
00528762 7414                     je         0x528778
00528764 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528767 83c024                   add        eax, 0x24
0052876a 8945bc                   mov        dword ptr [ebp - 0x44], eax
0052876d 6a00                     push       0
0052876f 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00528772 e8a968f9ff               call       0x4bf020
00528777 90                       nop        
00528778 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052877b 83c124                   add        ecx, 0x24
0052877e e8ad48f3ff               call       0x45d030
00528783 85c0                     test       eax, eax
00528785 743a                     je         0x5287c1
00528787 6a00                     push       0
00528789 6a0a                     push       0xa
0052878b b930a85b00               mov        ecx, 0x5ba830
00528790 e8dbe5efff               call       0x426d70
00528795 6a56                     push       0x56
00528797 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052879a e8f1440000               call       0x52cc90
0052879f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005287a2 83c124                   add        ecx, 0x24
005287a5 894db8                   mov        dword ptr [ebp - 0x48], ecx
005287a8 6a07                     push       7
005287aa 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005287ad e8ce58ffff               call       0x51e080
005287b2 0fbfd0                   movsx      edx, ax
005287b5 52                       push       edx
005287b6 6a56                     push       0x56
005287b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005287bb e810450000               call       0x52ccd0
005287c0 90                       nop        
005287c1 33c0                     xor        eax, eax
005287c3 7534                     jne        0x5287f9
005287c5 b904000000               mov        ecx, 4
005287ca 6bd100                   imul       edx, ecx, 0
005287cd 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005287d4 7423                     je         0x5287f9
005287d6 b804000000               mov        eax, 4
005287db 6bc800                   imul       ecx, eax, 0
005287de 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005287e4 8955b4                   mov        dword ptr [ebp - 0x4c], edx
005287e7 6806010000               push       0x106
005287ec 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005287ef e80c14efff               call       0x419c00
005287f4 8945e0                   mov        dword ptr [ebp - 0x20], eax
005287f7 eb07                     jmp        0x528800
005287f9 c745e000000000           mov        dword ptr [ebp - 0x20], 0
00528800 837de000                 cmp        dword ptr [ebp - 0x20], 0
00528804 741e                     je         0x528824
00528806 6a04                     push       4
00528808 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052880b e840460000               call       0x52ce50
00528810 6a00                     push       0
00528812 6a09                     push       9
00528814 b930a85b00               mov        ecx, 0x5ba830
00528819 e852e5efff               call       0x426d70
0052881e 90                       nop        
0052881f e9d8010000               jmp        0x5289fc
00528824 33c0                     xor        eax, eax
00528826 7534                     jne        0x52885c
00528828 b904000000               mov        ecx, 4
0052882d 6bd100                   imul       edx, ecx, 0
00528830 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00528837 7423                     je         0x52885c
00528839 b804000000               mov        eax, 4
0052883e 6bc800                   imul       ecx, eax, 0
00528841 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00528847 8955b0                   mov        dword ptr [ebp - 0x50], edx
0052884a 6801000800               push       0x80001
0052884f 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00528852 e8a913efff               call       0x419c00
00528857 8945dc                   mov        dword ptr [ebp - 0x24], eax
0052885a eb07                     jmp        0x528863
0052885c c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00528863 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00528867 746c                     je         0x5288d5
00528869 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052886c 83c124                   add        ecx, 0x24
0052886f e88c3aeeff               call       0x40c300
00528874 8945ac                   mov        dword ptr [ebp - 0x54], eax
00528877 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052887a 8b88d8580000             mov        ecx, dword ptr [eax + 0x58d8]
00528880 894da8                   mov        dword ptr [ebp - 0x58], ecx
00528883 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00528886 52                       push       edx
00528887 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0052888a 50                       push       eax
0052888b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052888e e89d420000               call       0x52cb30
00528893 85c0                     test       eax, eax
00528895 7514                     jne        0x5288ab
00528897 6a00                     push       0
00528899 6a10                     push       0x10
0052889b b930a85b00               mov        ecx, 0x5ba830
005288a0 e8cbe4efff               call       0x426d70
005288a5 90                       nop        
005288a6 e951010000               jmp        0x5289fc
005288ab 6a06                     push       6
005288ad 6a56                     push       0x56
005288af 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005288b2 e819440000               call       0x52ccd0
005288b7 6a03                     push       3
005288b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005288bc e88f450000               call       0x52ce50
005288c1 6a00                     push       0
005288c3 6a07                     push       7
005288c5 b930a85b00               mov        ecx, 0x5ba830
005288ca e8a1e4efff               call       0x426d70
005288cf 90                       nop        
005288d0 e927010000               jmp        0x5289fc
005288d5 e922010000               jmp        0x5289fc
005288da 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005288dd 81c154010000             add        ecx, 0x154
005288e3 894da4                   mov        dword ptr [ebp - 0x5c], ecx
005288e6 6a0e                     push       0xe
005288e8 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
005288eb e8d0acefff               call       0x4235c0
005288f0 0fb6d0                   movzx      edx, al
005288f3 85d2                     test       edx, edx
005288f5 7441                     je         0x528938
005288f7 6a14                     push       0x14
005288f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005288fc e8cf450000               call       0x52ced0
00528901 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528904 83c124                   add        ecx, 0x24
00528907 e8f439eeff               call       0x40c300
0052890c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052890f 8981dc580000             mov        dword ptr [ecx + 0x58dc], eax
00528915 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528918 83c124                   add        ecx, 0x24
0052891b e890d7f9ff               call       0x4c60b0
00528920 6a25                     push       0x25
00528922 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528925 e806430000               call       0x52cc30
0052892a 6a00                     push       0
0052892c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052892f 83c124                   add        ecx, 0x24
00528932 e81964f9ff               call       0x4bed50
00528937 90                       nop        
00528938 e9bf000000               jmp        0x5289fc
0052893d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528940 81c254010000             add        edx, 0x154
00528946 8955a0                   mov        dword ptr [ebp - 0x60], edx
00528949 6a06                     push       6
0052894b 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0052894e e86dacefff               call       0x4235c0
00528953 0fb6c0                   movzx      eax, al
00528956 85c0                     test       eax, eax
00528958 0f849e000000             je         0x5289fc
0052895e c745f400000000           mov        dword ptr [ebp - 0xc], 0
00528965 eb09                     jmp        0x528970
00528967 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052896a 83c101                   add        ecx, 1
0052896d 894df4                   mov        dword ptr [ebp - 0xc], ecx
00528970 837df405                 cmp        dword ptr [ebp - 0xc], 5
00528974 7d22                     jge        0x528998
00528976 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528979 81c294030000             add        edx, 0x394
0052897f 89559c                   mov        dword ptr [ebp - 0x64], edx
00528982 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00528985 50                       push       eax
00528986 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00528989 e8f2bbeeff               call       0x414580
0052898e 8bc8                     mov        ecx, eax
00528990 e8ab06f5ff               call       0x479040
00528995 90                       nop        
00528996 ebcf                     jmp        0x528967
00528998 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0052899f eb09                     jmp        0x5289aa
005289a1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005289a4 83c101                   add        ecx, 1
005289a7 894df0                   mov        dword ptr [ebp - 0x10], ecx
005289aa 837df007                 cmp        dword ptr [ebp - 0x10], 7
005289ae 7d12                     jge        0x5289c2
005289b0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005289b3 83c27f                   add        edx, 0x7f
005289b6 52                       push       edx
005289b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289ba e831440000               call       0x52cdf0
005289bf 90                       nop        
005289c0 ebdf                     jmp        0x5289a1
005289c2 6a25                     push       0x25
005289c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289c7 e864420000               call       0x52cc30
005289cc 6a56                     push       0x56
005289ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289d1 e85a420000               call       0x52cc30
005289d6 6a55                     push       0x55
005289d8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289db e850420000               call       0x52cc30
005289e0 6a12                     push       0x12
005289e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289e5 e8e6440000               call       0x52ced0
005289ea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289ed 83c124                   add        ecx, 0x24
005289f0 e81bd6f9ff               call       0x4c6010
005289f5 b801000000               mov        eax, 1
005289fa eb3b                     jmp        0x528a37
005289fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005289ff 83c124                   add        ecx, 0x24
00528a02 e8f938eeff               call       0x40c300
00528a07 894598                   mov        dword ptr [ebp - 0x68], eax
00528a0a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528a0d 8b88d8580000             mov        ecx, dword ptr [eax + 0x58d8]
00528a13 894d94                   mov        dword ptr [ebp - 0x6c], ecx
00528a16 6aff                     push       -1
00528a18 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528a1b 81c2e4580000             add        edx, 0x58e4
00528a21 52                       push       edx
00528a22 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00528a25 50                       push       eax
00528a26 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00528a29 51                       push       ecx
00528a2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528a2d e87e74ffff               call       0x51feb0
00528a32 b801000000               mov        eax, 1
00528a37 8be5                     mov        esp, ebp
00528a39 5d                       pop        ebp
00528a3a c3                       ret        
00528a3b 90                       nop        
00528a3c 50                       push       eax
00528a3d 855200                   test       dword ptr [edx], edx
00528a40 68865200c0               push       0xc0005286
00528a45 865200                   xchg       byte ptr [edx], dl
00528a48 da8852003d89             fimul      dword ptr [eax - 0x76c2ffae]
00528a4e 52                       push       edx
