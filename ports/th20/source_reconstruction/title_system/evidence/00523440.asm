00523440 55                       push       ebp
00523441 8bec                     mov        ebp, esp
00523443 81ec20010000             sub        esp, 0x120
00523449 56                       push       esi
0052344a 894dfc                   mov        dword ptr [ebp - 4], ecx
0052344d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523450 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00523453 894dec                   mov        dword ptr [ebp - 0x14], ecx
00523456 837dec05                 cmp        dword ptr [ebp - 0x14], 5
0052345a 0f874b0c0000             ja         0x5240ab
00523460 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00523463 ff2495b8405200           jmp        dword ptr [edx*4 + 0x5240b8]
0052346a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052346d 83c024                   add        eax, 0x24
00523470 8945c0                   mov        dword ptr [ebp - 0x40], eax
00523473 6a19                     push       0x19
00523475 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00523478 e86390f7ff               call       0x49c4e0
0052347d a12c615c00               mov        eax, dword ptr [0x5c612c]
00523482 99                       cdq        
00523483 b919000000               mov        ecx, 0x19
00523488 f7f9                     idiv       ecx
0052348a 52                       push       edx
0052348b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052348e 83c124                   add        ecx, 0x24
00523491 e8bab8f9ff               call       0x4bed50
00523496 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523499 81c208010000             add        edx, 0x108
0052349f 8955bc                   mov        dword ptr [ebp - 0x44], edx
005234a2 6a03                     push       3
005234a4 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005234a7 e83490f7ff               call       0x49c4e0
005234ac a12c615c00               mov        eax, dword ptr [0x5c612c]
005234b1 99                       cdq        
005234b2 b919000000               mov        ecx, 0x19
005234b7 f7f9                     idiv       ecx
005234b9 50                       push       eax
005234ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005234bd 81c108010000             add        ecx, 0x108
005234c3 e888b8f9ff               call       0x4bed50
005234c8 8b55fc                   mov        edx, dword ptr [ebp - 4]
005234cb 81c208010000             add        edx, 0x108
005234d1 8955b8                   mov        dword ptr [ebp - 0x48], edx
005234d4 6a01                     push       1
005234d6 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005234d9 e802c7f9ff               call       0x4bfbe0
005234de c7052c615c0000000000     mov        dword ptr [0x5c612c], 0
005234e8 8b45fc                   mov        eax, dword ptr [ebp - 4]
005234eb 0590030000               add        eax, 0x390
005234f0 8945b4                   mov        dword ptr [ebp - 0x4c], eax
005234f3 6a00                     push       0
005234f5 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005234f8 e8335ff2ff               call       0x449430
005234fd 0fb6c8                   movzx      ecx, al
00523500 85c9                     test       ecx, ecx
00523502 7430                     je         0x523534
00523504 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052350a e84144f1ff               call       0x437950
0052350f 8945b0                   mov        dword ptr [ebp - 0x50], eax
00523512 6a00                     push       0
00523514 6aff                     push       -1
00523516 6a13                     push       0x13
00523518 6a00                     push       0
0052351a 8d95e4feffff             lea        edx, [ebp - 0x11c]
00523520 52                       push       edx
00523521 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00523524 e847d7f2ff               call       0x450c70
00523529 8b00                     mov        eax, dword ptr [eax]
0052352b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052352e 898190030000             mov        dword ptr [ecx + 0x390], eax
00523534 6a26                     push       0x26
00523536 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523539 e8429a0000               call       0x52cf80
0052353e 6a01                     push       1
00523540 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523543 e808990000               call       0x52ce50
00523548 90                       nop        
00523549 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052354c 81c240570000             add        edx, 0x5740
00523552 8955a8                   mov        dword ptr [ebp - 0x58], edx
00523555 c745ac00000000           mov        dword ptr [ebp - 0x54], 0
0052355c 8d45ac                   lea        eax, [ebp - 0x54]
0052355f 50                       push       eax
00523560 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00523563 e8b88c0000               call       0x52c220
00523568 90                       nop        
00523569 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052356c c7813457000000000000     mov        dword ptr [ecx + 0x5734], 0
00523576 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523579 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
0052357f 83e0f7                   and        eax, 0xfffffff7
00523582 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523585 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
0052358b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052358e 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
00523594 83e0fb                   and        eax, 0xfffffffb
00523597 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052359a 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
005235a0 8b55fc                   mov        edx, dword ptr [ebp - 4]
005235a3 81c268590000             add        edx, 0x5968
005235a9 8955a0                   mov        dword ptr [ebp - 0x60], edx
005235ac 8b45fc                   mov        eax, dword ptr [ebp - 4]
005235af 8945a4                   mov        dword ptr [ebp - 0x5c], eax
005235b2 8d4da4                   lea        ecx, [ebp - 0x5c]
005235b5 51                       push       ecx
005235b6 6860fd5100               push       0x51fd60
005235bb 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
005235be e80d7ceeff               call       0x40b1d0
005235c3 90                       nop        
005235c4 6a00                     push       0
005235c6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005235c9 e882910000               call       0x52c750
005235ce 85c0                     test       eax, eax
005235d0 7515                     jne        0x5235e7
005235d2 6a00                     push       0
005235d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005235d7 e8a4990000               call       0x52cf80
005235dc 6a00                     push       0
005235de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005235e1 e8aa960000               call       0x52cc90
005235e6 90                       nop        
005235e7 8b55fc                   mov        edx, dword ptr [ebp - 4]
005235ea 81c254010000             add        edx, 0x154
005235f0 89559c                   mov        dword ptr [ebp - 0x64], edx
005235f3 6a06                     push       6
005235f5 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005235f8 e873daf3ff               call       0x461070
005235fd 0fb6c0                   movzx      eax, al
00523600 85c0                     test       eax, eax
00523602 740b                     je         0x52360f
00523604 6a02                     push       2
00523606 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523609 e842980000               call       0x52ce50
0052360e 90                       nop        
0052360f e9970a0000               jmp        0x5240ab
00523614 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523617 83c124                   add        ecx, 0x24
0052361a e8e1c5f9ff               call       0x4bfc00
0052361f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523622 81c108010000             add        ecx, 0x108
00523628 e8d3c5f9ff               call       0x4bfc00
0052362d 90                       nop        
0052362e 33c9                     xor        ecx, ecx
00523630 7531                     jne        0x523663
00523632 ba04000000               mov        edx, 4
00523637 6bc200                   imul       eax, edx, 0
0052363a 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00523641 7420                     je         0x523663
00523643 b904000000               mov        ecx, 4
00523648 6bd100                   imul       edx, ecx, 0
0052364b 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00523651 894598                   mov        dword ptr [ebp - 0x68], eax
00523654 6a10                     push       0x10
00523656 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00523659 e8226cefff               call       0x41a280
0052365e 8945e8                   mov        dword ptr [ebp - 0x18], eax
00523661 eb07                     jmp        0x52366a
00523663 c745e800000000           mov        dword ptr [ebp - 0x18], 0
0052366a 837de800                 cmp        dword ptr [ebp - 0x18], 0
0052366e 7414                     je         0x523684
00523670 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523673 83c124                   add        ecx, 0x24
00523676 894d94                   mov        dword ptr [ebp - 0x6c], ecx
00523679 6a00                     push       0
0052367b 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0052367e e8bdb9f9ff               call       0x4bf040
00523683 90                       nop        
00523684 33d2                     xor        edx, edx
00523686 7531                     jne        0x5236b9
00523688 b804000000               mov        eax, 4
0052368d 6bc800                   imul       ecx, eax, 0
00523690 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00523697 7420                     je         0x5236b9
00523699 ba04000000               mov        edx, 4
0052369e 6bc200                   imul       eax, edx, 0
005236a1 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005236a7 894d90                   mov        dword ptr [ebp - 0x70], ecx
005236aa 6a20                     push       0x20
005236ac 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005236af e8cc6befff               call       0x41a280
005236b4 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005236b7 eb07                     jmp        0x5236c0
005236b9 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
005236c0 837de400                 cmp        dword ptr [ebp - 0x1c], 0
005236c4 7414                     je         0x5236da
005236c6 8b55fc                   mov        edx, dword ptr [ebp - 4]
005236c9 83c224                   add        edx, 0x24
005236cc 89558c                   mov        dword ptr [ebp - 0x74], edx
005236cf 6a00                     push       0
005236d1 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
005236d4 e847b9f9ff               call       0x4bf020
005236d9 90                       nop        
005236da 33c0                     xor        eax, eax
005236dc 7531                     jne        0x52370f
005236de b904000000               mov        ecx, 4
005236e3 6bd100                   imul       edx, ecx, 0
005236e6 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005236ed 7420                     je         0x52370f
005236ef b804000000               mov        eax, 4
005236f4 6bc800                   imul       ecx, eax, 0
005236f7 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005236fd 895588                   mov        dword ptr [ebp - 0x78], edx
00523700 6a40                     push       0x40
00523702 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00523705 e8766befff               call       0x41a280
0052370a 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052370d eb07                     jmp        0x523716
0052370f c745e000000000           mov        dword ptr [ebp - 0x20], 0
00523716 837de000                 cmp        dword ptr [ebp - 0x20], 0
0052371a 7416                     je         0x523732
0052371c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052371f 0508010000               add        eax, 0x108
00523724 894584                   mov        dword ptr [ebp - 0x7c], eax
00523727 6a00                     push       0
00523729 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0052372c e80fb9f9ff               call       0x4bf040
00523731 90                       nop        
00523732 33c9                     xor        ecx, ecx
00523734 7534                     jne        0x52376a
00523736 ba04000000               mov        edx, 4
0052373b 6bc200                   imul       eax, edx, 0
0052373e 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00523745 7423                     je         0x52376a
00523747 b904000000               mov        ecx, 4
0052374c 6bd100                   imul       edx, ecx, 0
0052374f 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00523755 894580                   mov        dword ptr [ebp - 0x80], eax
00523758 6880000000               push       0x80
0052375d 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00523760 e81b6befff               call       0x41a280
00523765 8945dc                   mov        dword ptr [ebp - 0x24], eax
00523768 eb07                     jmp        0x523771
0052376a c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00523771 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00523775 741d                     je         0x523794
00523777 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052377a 81c108010000             add        ecx, 0x108
00523780 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
00523786 6a00                     push       0
00523788 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0052378e e88db8f9ff               call       0x4bf020
00523793 90                       nop        
00523794 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523797 81c108010000             add        ecx, 0x108
0052379d e88e98f3ff               call       0x45d030
005237a2 85c0                     test       eax, eax
005237a4 740f                     je         0x5237b5
005237a6 6a00                     push       0
005237a8 6a0a                     push       0xa
005237aa b930a85b00               mov        ecx, 0x5ba830
005237af e8bc35f0ff               call       0x426d70
005237b4 90                       nop        
005237b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005237b8 83c124                   add        ecx, 0x24
005237bb e87098f3ff               call       0x45d030
005237c0 85c0                     test       eax, eax
005237c2 740f                     je         0x5237d3
005237c4 6a00                     push       0
005237c6 6a0a                     push       0xa
005237c8 b930a85b00               mov        ecx, 0x5ba830
005237cd e89e35f0ff               call       0x426d70
005237d2 90                       nop        
005237d3 33d2                     xor        edx, edx
005237d5 753a                     jne        0x523811
005237d7 b804000000               mov        eax, 4
005237dc 6bc800                   imul       ecx, eax, 0
005237df 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005237e6 7429                     je         0x523811
005237e8 ba04000000               mov        edx, 4
005237ed 6bc200                   imul       eax, edx, 0
005237f0 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005237f6 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
005237fc 6806010000               push       0x106
00523801 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00523807 e8f463efff               call       0x419c00
0052380c 8945d8                   mov        dword ptr [ebp - 0x28], eax
0052380f eb07                     jmp        0x523818
00523811 c745d800000000           mov        dword ptr [ebp - 0x28], 0
00523818 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0052381c 7432                     je         0x523850
0052381e 6a05                     push       5
00523820 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523823 e828960000               call       0x52ce50
00523828 6a00                     push       0
0052382a 6a09                     push       9
0052382c b930a85b00               mov        ecx, 0x5ba830
00523831 e83a35f0ff               call       0x426d70
00523836 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523839 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
0052383f 83c804                   or         eax, 4
00523842 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523845 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
0052384b e95b080000               jmp        0x5240ab
00523850 33d2                     xor        edx, edx
00523852 753a                     jne        0x52388e
00523854 b804000000               mov        eax, 4
00523859 6bc800                   imul       ecx, eax, 0
0052385c 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00523863 7429                     je         0x52388e
00523865 ba04000000               mov        edx, 4
0052386a 6bc200                   imul       eax, edx, 0
0052386d 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00523873 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
00523879 6801000800               push       0x80001
0052387e 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00523884 e87763efff               call       0x419c00
00523889 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052388c eb07                     jmp        0x523895
0052388e c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00523895 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00523899 0f84b6010000             je         0x523a55
0052389f 8b55fc                   mov        edx, dword ptr [ebp - 4]
005238a2 81c240570000             add        edx, 0x5740
005238a8 899570ffffff             mov        dword ptr [ebp - 0x90], edx
005238ae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005238b1 83c124                   add        ecx, 0x24
005238b4 e8478aeeff               call       0x40c300
005238b9 8bf0                     mov        esi, eax
005238bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005238be 81c108010000             add        ecx, 0x108
005238c4 e8378aeeff               call       0x40c300
005238c9 6bc019                   imul       eax, eax, 0x19
005238cc 03f0                     add        esi, eax
005238ce 56                       push       esi
005238cf 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
005238d5 e8a60cefff               call       0x414580
005238da 833800                   cmp        dword ptr [eax], 0
005238dd 0f846d010000             je         0x523a50
005238e3 6a04                     push       4
005238e5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005238e8 e863950000               call       0x52ce50
005238ed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005238f0 83c124                   add        ecx, 0x24
005238f3 e8088aeeff               call       0x40c300
005238f8 8bf0                     mov        esi, eax
005238fa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005238fd 81c108010000             add        ecx, 0x108
00523903 e8f889eeff               call       0x40c300
00523908 6bc819                   imul       ecx, eax, 0x19
0052390b 03f1                     add        esi, ecx
0052390d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523910 89b238570000             mov        dword ptr [edx + 0x5738], esi
00523916 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523919 83c124                   add        ecx, 0x24
0052391c e88f27faff               call       0x4c60b0
00523921 6a00                     push       0
00523923 6a07                     push       7
00523925 b930a85b00               mov        ecx, 0x5ba830
0052392a e84134f0ff               call       0x426d70
0052392f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523932 83c024                   add        eax, 0x24
00523935 89856cffffff             mov        dword ptr [ebp - 0x94], eax
0052393b 6a07                     push       7
0052393d 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00523943 e8988bf7ff               call       0x49c4e0
00523948 6a00                     push       0
0052394a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052394d 83c124                   add        ecx, 0x24
00523950 e8fbb3f9ff               call       0x4bed50
00523955 90                       nop        
00523956 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0052395d eb09                     jmp        0x523968
0052395f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00523962 83c101                   add        ecx, 1
00523965 894df4                   mov        dword ptr [ebp - 0xc], ecx
00523968 837df407                 cmp        dword ptr [ebp - 0xc], 7
0052396c 7d60                     jge        0x5239ce
0052396e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523971 81c240570000             add        edx, 0x5740
00523977 899568ffffff             mov        dword ptr [ebp - 0x98], edx
0052397d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523980 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
00523986 51                       push       ecx
00523987 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
0052398d e8ee0befff               call       0x414580
00523992 8b10                     mov        edx, dword ptr [eax]
00523994 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
0052399a 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052399d 83c001                   add        eax, 1
005239a0 50                       push       eax
005239a1 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
005239a7 e8c44df6ff               call       0x488770
005239ac 85c0                     test       eax, eax
005239ae 751c                     jne        0x5239cc
005239b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005239b3 83c124                   add        ecx, 0x24
005239b6 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
005239bc 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005239bf 52                       push       edx
005239c0 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
005239c6 e8c59dffff               call       0x51d790
005239cb 90                       nop        
005239cc eb91                     jmp        0x52395f
005239ce 8b45fc                   mov        eax, dword ptr [ebp - 4]
005239d1 83c024                   add        eax, 0x24
005239d4 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
005239da 6a00                     push       0
005239dc 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
005239e2 e859b6f9ff               call       0x4bf040
005239e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005239ea 83c124                   add        ecx, 0x24
005239ed 898d58ffffff             mov        dword ptr [ebp - 0xa8], ecx
005239f3 6a00                     push       0
005239f5 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
005239fb e820b6f9ff               call       0x4bf020
00523a00 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523a03 81c240570000             add        edx, 0x5740
00523a09 899554ffffff             mov        dword ptr [ebp - 0xac], edx
00523a0f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523a12 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
00523a18 51                       push       ecx
00523a19 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
00523a1f e85c0befff               call       0x414580
00523a24 8b08                     mov        ecx, dword ptr [eax]
00523a26 e8e5ddefff               call       0x421810
00523a2b 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00523a31 8b9550ffffff             mov        edx, dword ptr [ebp - 0xb0]
00523a37 8a420a                   mov        al, byte ptr [edx + 0xa]
00523a3a d0e8                     shr        al, 1
00523a3c 2401                     and        al, 1
00523a3e 0fb6c8                   movzx      ecx, al
00523a41 83f901                   cmp        ecx, 1
00523a44 750a                     jne        0x523a50
00523a46 e908020000               jmp        0x523c53
00523a4b e903020000               jmp        0x523c53
00523a50 e956060000               jmp        0x5240ab
00523a55 e951060000               jmp        0x5240ab
00523a5a 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523a5d 81c254010000             add        edx, 0x154
00523a63 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
00523a69 6a0f                     push       0xf
00523a6b 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00523a71 e8eafaefff               call       0x423560
00523a76 0fb6c0                   movzx      eax, al
00523a79 85c0                     test       eax, eax
00523a7b 7405                     je         0x523a82
00523a7d e929060000               jmp        0x5240ab
00523a82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523a85 83c124                   add        ecx, 0x24
00523a88 e873c1f9ff               call       0x4bfc00
00523a8d 90                       nop        
00523a8e 33c9                     xor        ecx, ecx
00523a90 7537                     jne        0x523ac9
00523a92 ba04000000               mov        edx, 4
00523a97 6bc200                   imul       eax, edx, 0
00523a9a 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00523aa1 7426                     je         0x523ac9
00523aa3 b904000000               mov        ecx, 4
00523aa8 6bd100                   imul       edx, ecx, 0
00523aab 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00523ab1 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
00523ab7 6a10                     push       0x10
00523ab9 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00523abf e8bc67efff               call       0x41a280
00523ac4 8945d0                   mov        dword ptr [ebp - 0x30], eax
00523ac7 eb07                     jmp        0x523ad0
00523ac9 c745d000000000           mov        dword ptr [ebp - 0x30], 0
00523ad0 837dd000                 cmp        dword ptr [ebp - 0x30], 0
00523ad4 741a                     je         0x523af0
00523ad6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523ad9 83c124                   add        ecx, 0x24
00523adc 898d44ffffff             mov        dword ptr [ebp - 0xbc], ecx
00523ae2 6a00                     push       0
00523ae4 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
00523aea e851b5f9ff               call       0x4bf040
00523aef 90                       nop        
00523af0 33d2                     xor        edx, edx
00523af2 7537                     jne        0x523b2b
00523af4 b804000000               mov        eax, 4
00523af9 6bc800                   imul       ecx, eax, 0
00523afc 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00523b03 7426                     je         0x523b2b
00523b05 ba04000000               mov        edx, 4
00523b0a 6bc200                   imul       eax, edx, 0
00523b0d 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00523b13 898d40ffffff             mov        dword ptr [ebp - 0xc0], ecx
00523b19 6a20                     push       0x20
00523b1b 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
00523b21 e85a67efff               call       0x41a280
00523b26 8945cc                   mov        dword ptr [ebp - 0x34], eax
00523b29 eb07                     jmp        0x523b32
00523b2b c745cc00000000           mov        dword ptr [ebp - 0x34], 0
00523b32 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
00523b36 741a                     je         0x523b52
00523b38 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523b3b 83c224                   add        edx, 0x24
00523b3e 89953cffffff             mov        dword ptr [ebp - 0xc4], edx
00523b44 6a00                     push       0
00523b46 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
00523b4c e8cfb4f9ff               call       0x4bf020
00523b51 90                       nop        
00523b52 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523b55 83c124                   add        ecx, 0x24
00523b58 e8d394f3ff               call       0x45d030
00523b5d 85c0                     test       eax, eax
00523b5f 740f                     je         0x523b70
00523b61 6a00                     push       0
00523b63 6a0a                     push       0xa
00523b65 b930a85b00               mov        ecx, 0x5ba830
00523b6a e80132f0ff               call       0x426d70
00523b6f 90                       nop        
00523b70 33c0                     xor        eax, eax
00523b72 753a                     jne        0x523bae
00523b74 b904000000               mov        ecx, 4
00523b79 6bd100                   imul       edx, ecx, 0
00523b7c 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00523b83 7429                     je         0x523bae
00523b85 b804000000               mov        eax, 4
00523b8a 6bc800                   imul       ecx, eax, 0
00523b8d 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00523b93 899538ffffff             mov        dword ptr [ebp - 0xc8], edx
00523b99 6806010000               push       0x106
00523b9e 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
00523ba4 e85760efff               call       0x419c00
00523ba9 8945c8                   mov        dword ptr [ebp - 0x38], eax
00523bac eb07                     jmp        0x523bb5
00523bae c745c800000000           mov        dword ptr [ebp - 0x38], 0
00523bb5 837dc800                 cmp        dword ptr [ebp - 0x38], 0
00523bb9 744d                     je         0x523c08
00523bbb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523bbe 83c124                   add        ecx, 0x24
00523bc1 e84a24faff               call       0x4c6010
00523bc6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523bc9 83c024                   add        eax, 0x24
00523bcc 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
00523bd2 6a19                     push       0x19
00523bd4 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
00523bda e80189f7ff               call       0x49c4e0
00523bdf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523be2 83c124                   add        ecx, 0x24
00523be5 e8b62cfcff               call       0x4e68a0
00523bea 6a02                     push       2
00523bec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523bef e85c920000               call       0x52ce50
00523bf4 6a00                     push       0
00523bf6 6a09                     push       9
00523bf8 b930a85b00               mov        ecx, 0x5ba830
00523bfd e86e31f0ff               call       0x426d70
00523c02 90                       nop        
00523c03 e9a3040000               jmp        0x5240ab
00523c08 33c9                     xor        ecx, ecx
00523c0a 753a                     jne        0x523c46
00523c0c ba04000000               mov        edx, 4
00523c11 6bc200                   imul       eax, edx, 0
00523c14 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00523c1b 7429                     je         0x523c46
00523c1d b904000000               mov        ecx, 4
00523c22 6bd100                   imul       edx, ecx, 0
00523c25 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00523c2b 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
00523c31 6801000800               push       0x80001
00523c36 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
00523c3c e8bf5fefff               call       0x419c00
00523c41 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00523c44 eb07                     jmp        0x523c4d
00523c46 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00523c4d 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
00523c51 745a                     je         0x523cad
00523c53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523c56 83c124                   add        ecx, 0x24
00523c59 e8a286eeff               call       0x40c300
00523c5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523c61 89813c570000             mov        dword ptr [ecx + 0x573c], eax
00523c67 6a03                     push       3
00523c69 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523c6c e8df910000               call       0x52ce50
00523c71 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523c74 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
00523c7a 83c804                   or         eax, 4
00523c7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523c80 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
00523c86 6a00                     push       0
00523c88 6a32                     push       0x32
00523c8a b930a85b00               mov        ecx, 0x5ba830
00523c8f e8dc30f0ff               call       0x426d70
00523c94 51                       push       ecx
00523c95 f30f10055cfe5600         movss      xmm0, dword ptr [0x56fe5c]
00523c9d f30f110424               movss      dword ptr [esp], xmm0
00523ca2 b9404d5c00               mov        ecx, 0x5c4d40
00523ca7 e8245dfbff               call       0x4d99d0
00523cac 90                       nop        
00523cad e9f9030000               jmp        0x5240ab
00523cb2 6a00                     push       0
00523cb4 e86738f1ff               call       0x437520
00523cb9 83c404                   add        esp, 4
00523cbc 8bc8                     mov        ecx, eax
00523cbe e85d48f1ff               call       0x438520
00523cc3 85c0                     test       eax, eax
00523cc5 7526                     jne        0x523ced
00523cc7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523cca 81c254010000             add        edx, 0x154
00523cd0 89952cffffff             mov        dword ptr [ebp - 0xd4], edx
00523cd6 6a00                     push       0
00523cd8 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00523cde e83d57f0ff               call       0x429420
00523ce3 b801000000               mov        eax, 1
00523ce8 e9c3030000               jmp        0x5240b0
00523ced 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523cf0 0554010000               add        eax, 0x154
00523cf5 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
00523cfb 6a02                     push       2
00523cfd 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00523d03 e8d848f5ff               call       0x4785e0
00523d08 0fb6c8                   movzx      ecx, al
00523d0b 85c9                     test       ecx, ecx
00523d0d 7463                     je         0x523d72
00523d0f 6a00                     push       0
00523d11 e80a38f1ff               call       0x437520
00523d16 83c404                   add        esp, 4
00523d19 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
00523d1f 6a00                     push       0
00523d21 6a00                     push       0
00523d23 6a00                     push       0
00523d25 8d95e0feffff             lea        edx, [ebp - 0x120]
00523d2b 52                       push       edx
00523d2c 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
00523d32 e8b99ff7ff               call       0x49dcf0
00523d37 8b00                     mov        eax, dword ptr [eax]
00523d39 a3044f5c00               mov        dword ptr [0x5c4f04], eax
00523d3e 6a00                     push       0
00523d40 b9044f5c00               mov        ecx, 0x5c4f04
00523d45 e86689ffff               call       0x51c6b0
00523d4a 51                       push       ecx
00523d4b f30f100588035700         movss      xmm0, dword ptr [0x570388]
00523d53 f30f110424               movss      dword ptr [esp], xmm0
00523d58 51                       push       ecx
00523d59 f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
00523d61 f30f110424               movss      dword ptr [esp], xmm0
00523d66 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523d6c e82fcdf7ff               call       0x4a0aa0
00523d71 90                       nop        
00523d72 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523d75 81c154010000             add        ecx, 0x154
00523d7b 898d20ffffff             mov        dword ptr [ebp - 0xe0], ecx
00523d81 6a20                     push       0x20
00523d83 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
00523d89 e832f8efff               call       0x4235c0
00523d8e 0fb6d0                   movzx      edx, al
00523d91 85d2                     test       edx, edx
00523d93 0f8425020000             je         0x523fbe
00523d99 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523d9c 8b88d4580000             mov        ecx, dword ptr [eax + 0x58d4]
00523da2 c1e903                   shr        ecx, 3
00523da5 83e101                   and        ecx, 1
00523da8 0f8410020000             je         0x523fbe
00523dae 6a02                     push       2
00523db0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523db3 e818910000               call       0x52ced0
00523db8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523dbb 8b823c570000             mov        eax, dword ptr [edx + 0x573c]
00523dc1 83c001                   add        eax, 1
00523dc4 50                       push       eax
00523dc5 b968a55b00               mov        ecx, 0x5ba568
00523dca e891a5f9ff               call       0x4be360
00523dcf 6a0d                     push       0xd
00523dd1 b9404d5c00               mov        ecx, 0x5c4d40
00523dd6 e875a4f9ff               call       0x4be250
00523ddb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523dde 81c140570000             add        ecx, 0x5740
00523de4 898d1cffffff             mov        dword ptr [ebp - 0xe4], ecx
00523dea 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523ded 8b8238570000             mov        eax, dword ptr [edx + 0x5738]
00523df3 50                       push       eax
00523df4 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
00523dfa e88107efff               call       0x414580
00523dff 8b08                     mov        ecx, dword ptr [eax]
00523e01 e8fa840000               call       0x52c300
00523e06 50                       push       eax
00523e07 6800010000               push       0x100
00523e0c 68204b5c00               push       0x5c4b20
00523e11 e8fa470200               call       0x548610
00523e16 83c40c                   add        esp, 0xc
00523e19 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523e1c 81c140570000             add        ecx, 0x5740
00523e22 898d18ffffff             mov        dword ptr [ebp - 0xe8], ecx
00523e28 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523e2b 8b8238570000             mov        eax, dword ptr [edx + 0x5738]
00523e31 50                       push       eax
00523e32 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
00523e38 e84307efff               call       0x414580
00523e3d 8b08                     mov        ecx, dword ptr [eax]
00523e3f e8ccd9efff               call       0x421810
00523e44 8945f8                   mov        dword ptr [ebp - 8], eax
00523e47 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00523e4a 8b91d8000000             mov        edx, dword ptr [ecx + 0xd8]
00523e50 52                       push       edx
00523e51 b968a55b00               mov        ecx, 0x5ba568
00523e56 e88565feff               call       0x50a3e0
00523e5b 6a00                     push       0
00523e5d e81e02f4ff               call       0x464080
00523e62 83c404                   add        esp, 4
00523e65 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
00523e6b b804000000               mov        eax, 4
00523e70 6bc800                   imul       ecx, eax, 0
00523e73 8b55f8                   mov        edx, dword ptr [ebp - 8]
00523e76 8b840adc000000           mov        eax, dword ptr [edx + ecx + 0xdc]
00523e7d 898514ffffff             mov        dword ptr [ebp - 0xec], eax
00523e83 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
00523e89 51                       push       ecx
00523e8a 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
00523e90 e82befeeff               call       0x412dc0
00523e95 6a00                     push       0
00523e97 e8e401f4ff               call       0x464080
00523e9c 83c404                   add        esp, 4
00523e9f 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
00523ea5 ba04000000               mov        edx, 4
00523eaa c1e200                   shl        edx, 0
00523ead 8b45f8                   mov        eax, dword ptr [ebp - 8]
00523eb0 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
00523eb7 898d0cffffff             mov        dword ptr [ebp - 0xf4], ecx
00523ebd 8b950cffffff             mov        edx, dword ptr [ebp - 0xf4]
00523ec3 52                       push       edx
00523ec4 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00523eca e8819fefff               call       0x41de50
00523ecf 6a00                     push       0
00523ed1 e8aa01f4ff               call       0x464080
00523ed6 83c404                   add        esp, 4
00523ed9 898500ffffff             mov        dword ptr [ebp - 0x100], eax
00523edf b804000000               mov        eax, 4
00523ee4 d1e0                     shl        eax, 1
00523ee6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00523ee9 8b9401dc000000           mov        edx, dword ptr [ecx + eax + 0xdc]
00523ef0 899504ffffff             mov        dword ptr [ebp - 0xfc], edx
00523ef6 8b8504ffffff             mov        eax, dword ptr [ebp - 0xfc]
00523efc 50                       push       eax
00523efd 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00523f03 e898eeeeff               call       0x412da0
00523f08 6a00                     push       0
00523f0a e87101f4ff               call       0x464080
00523f0f 83c404                   add        esp, 4
00523f12 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
00523f18 b904000000               mov        ecx, 4
00523f1d 6bd103                   imul       edx, ecx, 3
00523f20 8b45f8                   mov        eax, dword ptr [ebp - 8]
00523f23 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
00523f2a 898dfcfeffff             mov        dword ptr [ebp - 0x104], ecx
00523f30 8b95fcfeffff             mov        edx, dword ptr [ebp - 0x104]
00523f36 52                       push       edx
00523f37 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
00523f3d e84e9fefff               call       0x41de90
00523f42 8b45f8                   mov        eax, dword ptr [ebp - 8]
00523f45 8b88f0000000             mov        ecx, dword ptr [eax + 0xf0]
00523f4b 51                       push       ecx
00523f4c b968a55b00               mov        ecx, 0x5ba568
00523f51 e85aa2f9ff               call       0x4be1b0
00523f56 90                       nop        
00523f57 8b55f8                   mov        edx, dword ptr [ebp - 8]
00523f5a 8a420a                   mov        al, byte ptr [edx + 0xa]
00523f5d d0e8                     shr        al, 1
00523f5f 2401                     and        al, 1
00523f61 0fb6c8                   movzx      ecx, al
00523f64 83f901                   cmp        ecx, 1
00523f67 7523                     jne        0x523f8c
00523f69 6a02                     push       2
00523f6b b968a55b00               mov        ecx, 0x5ba568
00523f70 e82b65feff               call       0x50a4a0
00523f75 8b55f8                   mov        edx, dword ptr [ebp - 8]
00523f78 8b82fc000000             mov        eax, dword ptr [edx + 0xfc]
00523f7e 50                       push       eax
00523f7f b968a55b00               mov        ecx, 0x5ba568
00523f84 e86768feff               call       0x50a7f0
00523f89 90                       nop        
00523f8a eb19                     jmp        0x523fa5
00523f8c 6a00                     push       0
00523f8e b968a55b00               mov        ecx, 0x5ba568
00523f93 e80865feff               call       0x50a4a0
00523f98 6aff                     push       -1
00523f9a b968a55b00               mov        ecx, 0x5ba568
00523f9f e84c68feff               call       0x50a7f0
00523fa4 90                       nop        
00523fa5 c70528615c0002000000     mov        dword ptr [0x5c6128], 2
00523faf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00523fb2 8b9138570000             mov        edx, dword ptr [ecx + 0x5738]
00523fb8 89152c615c00             mov        dword ptr [0x5c612c], edx
00523fbe e9e8000000               jmp        0x5240ab
00523fc3 8b45fc                   mov        eax, dword ptr [ebp - 4]
00523fc6 0554010000               add        eax, 0x154
00523fcb 8985f4feffff             mov        dword ptr [ebp - 0x10c], eax
00523fd1 6a06                     push       6
00523fd3 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
00523fd9 e8e2f5efff               call       0x4235c0
00523fde 0fb6c8                   movzx      ecx, al
00523fe1 85c9                     test       ecx, ecx
00523fe3 0f84c2000000             je         0x5240ab
00523fe9 8b55fc                   mov        edx, dword ptr [ebp - 4]
00523fec 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
00523ff2 c1e803                   shr        eax, 3
00523ff5 83e001                   and        eax, 1
00523ff8 0f84ad000000             je         0x5240ab
00523ffe c745f000000000           mov        dword ptr [ebp - 0x10], 0
00524005 eb09                     jmp        0x524010
00524007 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052400a 83c101                   add        ecx, 1
0052400d 894df0                   mov        dword ptr [ebp - 0x10], ecx
00524010 837df064                 cmp        dword ptr [ebp - 0x10], 0x64
00524014 7d2b                     jge        0x524041
00524016 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524019 81c240570000             add        edx, 0x5740
0052401f 8995f0feffff             mov        dword ptr [ebp - 0x110], edx
00524025 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00524028 50                       push       eax
00524029 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0052402f e84c05efff               call       0x414580
00524034 8b08                     mov        ecx, dword ptr [eax]
00524036 51                       push       ecx
00524037 e884d7efff               call       0x4217c0
0052403c 83c404                   add        esp, 4
0052403f ebc6                     jmp        0x524007
00524041 8b55fc                   mov        edx, dword ptr [ebp - 4]
00524044 81c240570000             add        edx, 0x5740
0052404a 8995e8feffff             mov        dword ptr [ebp - 0x118], edx
00524050 c785ecfeffff00000000     mov        dword ptr [ebp - 0x114], 0
0052405a 8d85ecfeffff             lea        eax, [ebp - 0x114]
00524060 50                       push       eax
00524061 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
00524067 e8b4810000               call       0x52c220
0052406c 90                       nop        
0052406d 6a26                     push       0x26
0052406f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00524072 e8b98b0000               call       0x52cc30
00524077 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052407a 81c190030000             add        ecx, 0x390
00524080 e8bb4ff5ff               call       0x479040
00524085 6a00                     push       0
00524087 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052408a 81c190030000             add        ecx, 0x390
00524090 e80bd7eeff               call       0x4117a0
00524095 6a01                     push       1
00524097 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052409a e8318e0000               call       0x52ced0
0052409f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005240a2 83c124                   add        ecx, 0x24
005240a5 e8661ffaff               call       0x4c6010
005240aa 90                       nop        
005240ab b801000000               mov        eax, 1
005240b0 5e                       pop        esi
005240b1 8be5                     mov        esp, ebp
005240b3 5d                       pop        ebp
005240b4 c3                       ret        
005240b5 0f1f00                   nop        dword ptr [eax]
005240b8 6a34                     push       0x34
005240ba 52                       push       edx
005240bb 00e7                     add        bh, ah
005240bd 3552001436               xor        eax, 0x36140052
005240c2 52                       push       edx
005240c3 00b23c52005a             add        byte ptr [edx + 0x5a00523c], dh
005240c9 3a5200                   cmp        dl, byte ptr [edx]
005240cc c3                       ret        
005240cd 3f                       aas        
005240ce 52                       push       edx
