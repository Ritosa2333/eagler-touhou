005265a0 55                       push       ebp
005265a1 8bec                     mov        ebp, esp
005265a3 81ec80000000             sub        esp, 0x80
005265a9 894dfc                   mov        dword ptr [ebp - 4], ecx
005265ac c745f84e000000           mov        dword ptr [ebp - 8], 0x4e
005265b3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005265b6 8b4820                   mov        ecx, dword ptr [eax + 0x20]
005265b9 894df4                   mov        dword ptr [ebp - 0xc], ecx
005265bc 837df404                 cmp        dword ptr [ebp - 0xc], 4
005265c0 0f879e040000             ja         0x526a64
005265c6 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005265c9 ff2495706a5200           jmp        dword ptr [edx*4 + 0x526a70]
005265d0 8b45fc                   mov        eax, dword ptr [ebp - 4]
005265d3 0590030000               add        eax, 0x390
005265d8 8945d8                   mov        dword ptr [ebp - 0x28], eax
005265db 6a00                     push       0
005265dd 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
005265e0 e84b2ef2ff               call       0x449430
005265e5 0fb6c8                   movzx      ecx, al
005265e8 85c9                     test       ecx, ecx
005265ea 742d                     je         0x526619
005265ec 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005265f2 e85913f1ff               call       0x437950
005265f7 8945d4                   mov        dword ptr [ebp - 0x2c], eax
005265fa 6a00                     push       0
005265fc 6aff                     push       -1
005265fe 6a13                     push       0x13
00526600 6a00                     push       0
00526602 8d5588                   lea        edx, [ebp - 0x78]
00526605 52                       push       edx
00526606 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00526609 e862a6f2ff               call       0x450c70
0052660e 8b00                     mov        eax, dword ptr [eax]
00526610 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526613 898190030000             mov        dword ptr [ecx + 0x390], eax
00526619 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052661c 83c224                   add        edx, 0x24
0052661f 8955d0                   mov        dword ptr [ebp - 0x30], edx
00526622 6a03                     push       3
00526624 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00526627 e8b45ef7ff               call       0x49c4e0
0052662c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052662f 83c024                   add        eax, 0x24
00526632 8945cc                   mov        dword ptr [ebp - 0x34], eax
00526635 6a01                     push       1
00526637 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0052663a e8a195f9ff               call       0x4bfbe0
0052663f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00526642 51                       push       ecx
00526643 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526646 e8e5650000               call       0x52cc30
0052664b 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052664e 52                       push       edx
0052664f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526652 e829690000               call       0x52cf80
00526657 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052665a 50                       push       eax
0052665b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052665e e82d660000               call       0x52cc90
00526663 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526666 83c124                   add        ecx, 0x24
00526669 894dc8                   mov        dword ptr [ebp - 0x38], ecx
0052666c 6a07                     push       7
0052666e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00526671 e80a7affff               call       0x51e080
00526676 0fbfd0                   movsx      edx, ax
00526679 52                       push       edx
0052667a 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052667d 50                       push       eax
0052667e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526681 e84a660000               call       0x52ccd0
00526686 6a2e                     push       0x2e
00526688 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052668b e8f0680000               call       0x52cf80
00526690 6a01                     push       1
00526692 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526695 e8b6670000               call       0x52ce50
0052669a 90                       nop        
0052669b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052669e 81c154010000             add        ecx, 0x154
005266a4 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005266a7 6a06                     push       6
005266a9 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
005266ac e8bfa9f3ff               call       0x461070
005266b1 0fb6d0                   movzx      edx, al
005266b4 85d2                     test       edx, edx
005266b6 740b                     je         0x5266c3
005266b8 6a02                     push       2
005266ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005266bd e88e670000               call       0x52ce50
005266c2 90                       nop        
005266c3 e99c030000               jmp        0x526a64
005266c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005266cb 83c124                   add        ecx, 0x24
005266ce e82d95f9ff               call       0x4bfc00
005266d3 90                       nop        
005266d4 33c0                     xor        eax, eax
005266d6 7531                     jne        0x526709
005266d8 b904000000               mov        ecx, 4
005266dd 6bd100                   imul       edx, ecx, 0
005266e0 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005266e7 7420                     je         0x526709
005266e9 b804000000               mov        eax, 4
005266ee 6bc800                   imul       ecx, eax, 0
005266f1 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005266f7 8955c0                   mov        dword ptr [ebp - 0x40], edx
005266fa 6a10                     push       0x10
005266fc 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005266ff e87c3befff               call       0x41a280
00526704 8945f0                   mov        dword ptr [ebp - 0x10], eax
00526707 eb07                     jmp        0x526710
00526709 c745f000000000           mov        dword ptr [ebp - 0x10], 0
00526710 837df000                 cmp        dword ptr [ebp - 0x10], 0
00526714 7542                     jne        0x526758
00526716 33c0                     xor        eax, eax
00526718 7531                     jne        0x52674b
0052671a b904000000               mov        ecx, 4
0052671f 6bd100                   imul       edx, ecx, 0
00526722 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00526729 7420                     je         0x52674b
0052672b b804000000               mov        eax, 4
00526730 6bc800                   imul       ecx, eax, 0
00526733 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00526739 8955bc                   mov        dword ptr [ebp - 0x44], edx
0052673c 6a40                     push       0x40
0052673e 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00526741 e83a3befff               call       0x41a280
00526746 8945ec                   mov        dword ptr [ebp - 0x14], eax
00526749 eb07                     jmp        0x526752
0052674b c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00526752 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00526756 7414                     je         0x52676c
00526758 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052675b 83c024                   add        eax, 0x24
0052675e 8945b8                   mov        dword ptr [ebp - 0x48], eax
00526761 6a00                     push       0
00526763 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00526766 e8d588f9ff               call       0x4bf040
0052676b 90                       nop        
0052676c 33c9                     xor        ecx, ecx
0052676e 7531                     jne        0x5267a1
00526770 ba04000000               mov        edx, 4
00526775 6bc200                   imul       eax, edx, 0
00526778 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0052677f 7420                     je         0x5267a1
00526781 b904000000               mov        ecx, 4
00526786 6bd100                   imul       edx, ecx, 0
00526789 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0052678f 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00526792 6a20                     push       0x20
00526794 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00526797 e8e43aefff               call       0x41a280
0052679c 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052679f eb07                     jmp        0x5267a8
005267a1 c745e800000000           mov        dword ptr [ebp - 0x18], 0
005267a8 837de800                 cmp        dword ptr [ebp - 0x18], 0
005267ac 7545                     jne        0x5267f3
005267ae 33c9                     xor        ecx, ecx
005267b0 7534                     jne        0x5267e6
005267b2 ba04000000               mov        edx, 4
005267b7 6bc200                   imul       eax, edx, 0
005267ba 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005267c1 7423                     je         0x5267e6
005267c3 b904000000               mov        ecx, 4
005267c8 6bd100                   imul       edx, ecx, 0
005267cb 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005267d1 8945b0                   mov        dword ptr [ebp - 0x50], eax
005267d4 6880000000               push       0x80
005267d9 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005267dc e89f3aefff               call       0x41a280
005267e1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005267e4 eb07                     jmp        0x5267ed
005267e6 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
005267ed 837de400                 cmp        dword ptr [ebp - 0x1c], 0
005267f1 7414                     je         0x526807
005267f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005267f6 83c124                   add        ecx, 0x24
005267f9 894dac                   mov        dword ptr [ebp - 0x54], ecx
005267fc 6a00                     push       0
005267fe 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00526801 e81a88f9ff               call       0x4bf020
00526806 90                       nop        
00526807 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052680a 83c124                   add        ecx, 0x24
0052680d e81e68f3ff               call       0x45d030
00526812 85c0                     test       eax, eax
00526814 743e                     je         0x526854
00526816 6a00                     push       0
00526818 6a0a                     push       0xa
0052681a b930a85b00               mov        ecx, 0x5ba830
0052681f e84c05f0ff               call       0x426d70
00526824 8b55f8                   mov        edx, dword ptr [ebp - 8]
00526827 52                       push       edx
00526828 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052682b e860640000               call       0x52cc90
00526830 8b45fc                   mov        eax, dword ptr [ebp - 4]
00526833 83c024                   add        eax, 0x24
00526836 8945a8                   mov        dword ptr [ebp - 0x58], eax
00526839 6a07                     push       7
0052683b 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0052683e e83d78ffff               call       0x51e080
00526843 0fbfc8                   movsx      ecx, ax
00526846 51                       push       ecx
00526847 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052684a 52                       push       edx
0052684b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052684e e87d640000               call       0x52ccd0
00526853 90                       nop        
00526854 33c0                     xor        eax, eax
00526856 7534                     jne        0x52688c
00526858 b904000000               mov        ecx, 4
0052685d 6bd100                   imul       edx, ecx, 0
00526860 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00526867 7423                     je         0x52688c
00526869 b804000000               mov        eax, 4
0052686e 6bc800                   imul       ecx, eax, 0
00526871 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00526877 8955a4                   mov        dword ptr [ebp - 0x5c], edx
0052687a 6806010000               push       0x106
0052687f 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00526882 e87933efff               call       0x419c00
00526887 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052688a eb07                     jmp        0x526893
0052688c c745e000000000           mov        dword ptr [ebp - 0x20], 0
00526893 837de000                 cmp        dword ptr [ebp - 0x20], 0
00526897 742a                     je         0x5268c3
00526899 6a04                     push       4
0052689b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052689e e8ad650000               call       0x52ce50
005268a3 6a00                     push       0
005268a5 6a09                     push       9
005268a7 b930a85b00               mov        ecx, 0x5ba830
005268ac e8bf04f0ff               call       0x426d70
005268b1 8b45f8                   mov        eax, dword ptr [ebp - 8]
005268b4 50                       push       eax
005268b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005268b8 e873630000               call       0x52cc30
005268bd 90                       nop        
005268be e9a1010000               jmp        0x526a64
005268c3 33c9                     xor        ecx, ecx
005268c5 7534                     jne        0x5268fb
005268c7 ba04000000               mov        edx, 4
005268cc 6bc200                   imul       eax, edx, 0
005268cf 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005268d6 7423                     je         0x5268fb
005268d8 b904000000               mov        ecx, 4
005268dd 6bd100                   imul       edx, ecx, 0
005268e0 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005268e6 8945a0                   mov        dword ptr [ebp - 0x60], eax
005268e9 6801000800               push       0x80001
005268ee 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
005268f1 e80a33efff               call       0x419c00
005268f6 8945dc                   mov        dword ptr [ebp - 0x24], eax
005268f9 eb07                     jmp        0x526902
005268fb c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00526902 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00526906 0f848b000000             je         0x526997
0052690c 6a06                     push       6
0052690e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00526911 51                       push       ecx
00526912 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526915 e8b6630000               call       0x52ccd0
0052691a 90                       nop        
0052691b b968a55b00               mov        ecx, 0x5ba568
00526920 e87bd7f3ff               call       0x4640a0
00526925 83f804                   cmp        eax, 4
00526928 7d2f                     jge        0x526959
0052692a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052692d 83c124                   add        ecx, 0x24
00526930 e8cb59eeff               call       0x40c300
00526935 83c030                   add        eax, 0x30
00526938 50                       push       eax
00526939 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052693c 52                       push       edx
0052693d 8d4584                   lea        eax, [ebp - 0x7c]
00526940 50                       push       eax
00526941 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526944 e8f7580000               call       0x52c240
00526949 89459c                   mov        dword ptr [ebp - 0x64], eax
0052694c 6a02                     push       2
0052694e 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00526951 e83a86f2ff               call       0x44ef90
00526956 90                       nop        
00526957 eb20                     jmp        0x526979
00526959 6a34                     push       0x34
0052695b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052695e 51                       push       ecx
0052695f 8d5580                   lea        edx, [ebp - 0x80]
00526962 52                       push       edx
00526963 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526966 e8d5580000               call       0x52c240
0052696b 894598                   mov        dword ptr [ebp - 0x68], eax
0052696e 6a02                     push       2
00526970 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00526973 e81886f2ff               call       0x44ef90
00526978 90                       nop        
00526979 6a03                     push       3
0052697b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052697e e8cd640000               call       0x52ce50
00526983 6a00                     push       0
00526985 6a07                     push       7
00526987 b930a85b00               mov        ecx, 0x5ba830
0052698c e8df03f0ff               call       0x426d70
00526991 90                       nop        
00526992 e9cd000000               jmp        0x526a64
00526997 e9c8000000               jmp        0x526a64
0052699c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052699f 0554010000               add        eax, 0x154
005269a4 894594                   mov        dword ptr [ebp - 0x6c], eax
005269a7 6a0e                     push       0xe
005269a9 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005269ac e80fccefff               call       0x4235c0
005269b1 0fb6c8                   movzx      ecx, al
005269b4 85c9                     test       ecx, ecx
005269b6 7445                     je         0x5269fd
005269b8 6a2e                     push       0x2e
005269ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005269bd e86e620000               call       0x52cc30
005269c2 90                       nop        
005269c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005269c6 83c124                   add        ecx, 0x24
005269c9 e83259eeff               call       0x40c300
005269ce 894590                   mov        dword ptr [ebp - 0x70], eax
005269d1 837d9002                 cmp        dword ptr [ebp - 0x70], 2
005269d5 7402                     je         0x5269d9
005269d7 eb0d                     jmp        0x5269e6
005269d9 6a17                     push       0x17
005269db 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005269de e8ed640000               call       0x52ced0
005269e3 90                       nop        
005269e4 eb0b                     jmp        0x5269f1
005269e6 6a0b                     push       0xb
005269e8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005269eb e8e0640000               call       0x52ced0
005269f0 90                       nop        
005269f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005269f4 83c124                   add        ecx, 0x24
005269f7 e8b4f6f9ff               call       0x4c60b0
005269fc 90                       nop        
005269fd eb65                     jmp        0x526a64
005269ff 8b55fc                   mov        edx, dword ptr [ebp - 4]
00526a02 81c254010000             add        edx, 0x154
00526a08 89558c                   mov        dword ptr [ebp - 0x74], edx
00526a0b 6a06                     push       6
00526a0d 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00526a10 e8abcbefff               call       0x4235c0
00526a15 0fb6c0                   movzx      eax, al
00526a18 85c0                     test       eax, eax
00526a1a 7448                     je         0x526a64
00526a1c 6a2e                     push       0x2e
00526a1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a21 e80a620000               call       0x52cc30
00526a26 6a01                     push       1
00526a28 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a2b e8a0640000               call       0x52ced0
00526a30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a33 81c190030000             add        ecx, 0x390
00526a39 e80226f5ff               call       0x479040
00526a3e 6a00                     push       0
00526a40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a43 81c190030000             add        ecx, 0x390
00526a49 e852adeeff               call       0x4117a0
00526a4e 6a01                     push       1
00526a50 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a53 e878640000               call       0x52ced0
00526a58 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00526a5b 83c124                   add        ecx, 0x24
00526a5e e8adf5f9ff               call       0x4c6010
00526a63 90                       nop        
00526a64 b801000000               mov        eax, 1
00526a69 8be5                     mov        esp, ebp
00526a6b 5d                       pop        ebp
00526a6c c3                       ret        
00526a6d 0f1f00                   nop        dword ptr [eax]
00526a70 d06552                   shl        byte ptr [ebp + 0x52], 1
00526a73 009b665200c8             add        byte ptr [ebx - 0x37ffad9a], bl
00526a79 6652                     push       dx
00526a7b 009c695200ff69           add        byte ptr [ecx + ebp*2 + 0x69ff0052], bl
00526a82 52                       push       edx
00526a83 00cc                     add        ah, cl
00526a85 cc                       int3       
00526a86 cc                       int3       
00526a87 cc                       int3       
00526a88 cc                       int3       
00526a89 cc                       int3       
00526a8a cc                       int3       
00526a8b cc                       int3       
00526a8c cc                       int3       
00526a8d cc                       int3       
00526a8e cc                       int3       
00526a8f cc                       int3       
