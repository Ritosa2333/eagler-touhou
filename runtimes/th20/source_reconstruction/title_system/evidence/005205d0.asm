005205d0 55                       push       ebp
005205d1 8bec                     mov        ebp, esp
005205d3 81ec88000000             sub        esp, 0x88
005205d9 894dfc                   mov        dword ptr [ebp - 4], ecx
005205dc 8b45fc                   mov        eax, dword ptr [ebp - 4]
005205df 8b4820                   mov        ecx, dword ptr [eax + 0x20]
005205e2 894dec                   mov        dword ptr [ebp - 0x14], ecx
005205e5 837dec03                 cmp        dword ptr [ebp - 0x14], 3
005205e9 0f8776060000             ja         0x520c65
005205ef 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005205f2 ff24956c0c5200           jmp        dword ptr [edx*4 + 0x520c6c]
005205f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005205fc 0590030000               add        eax, 0x390
00520601 8945c8                   mov        dword ptr [ebp - 0x38], eax
00520604 6a00                     push       0
00520606 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00520609 e8228ef2ff               call       0x449430
0052060e 0fb6c8                   movzx      ecx, al
00520611 85c9                     test       ecx, ecx
00520613 7430                     je         0x520645
00520615 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052061b e83073f1ff               call       0x437950
00520620 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00520623 6a00                     push       0
00520625 6aff                     push       -1
00520627 6a13                     push       0x13
00520629 6a00                     push       0
0052062b 8d9578ffffff             lea        edx, [ebp - 0x88]
00520631 52                       push       edx
00520632 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00520635 e83606f3ff               call       0x450c70
0052063a 8b00                     mov        eax, dword ptr [eax]
0052063c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052063f 898190030000             mov        dword ptr [ecx + 0x390], eax
00520645 6a28                     push       0x28
00520647 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052064a e831c90000               call       0x52cf80
0052064f 6a00                     push       0
00520651 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520654 81c178040000             add        ecx, 0x478
0052065a e841b4eeff               call       0x40baa0
0052065f 90                       nop        
00520660 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520663 81c268590000             add        edx, 0x5968
00520669 8955bc                   mov        dword ptr [ebp - 0x44], edx
0052066c c745c000000000           mov        dword ptr [ebp - 0x40], 0
00520673 8d45c0                   lea        eax, [ebp - 0x40]
00520676 50                       push       eax
00520677 6890f85100               push       0x51f890
0052067c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052067f 51                       push       ecx
00520680 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00520683 e888a4feff               call       0x50ab10
00520688 90                       nop        
00520689 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052068c c7828004000000000000     mov        dword ptr [edx + 0x480], 0
00520696 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520699 c7808404000000000000     mov        dword ptr [eax + 0x484], 0
005206a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005206a6 c7818c04000000000000     mov        dword ptr [ecx + 0x48c], 0
005206b0 6a01                     push       1
005206b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005206b5 e896c70000               call       0x52ce50
005206ba 90                       nop        
005206bb 6aff                     push       -1
005206bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005206c0 81c154010000             add        ecx, 0x154
005206c6 e8552ef0ff               call       0x423520
005206cb 90                       nop        
005206cc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005206cf 81c178040000             add        ecx, 0x478
005206d5 e856abf4ff               call       0x46b230
005206da 0fb6d0                   movzx      edx, al
005206dd 85d2                     test       edx, edx
005206df 740c                     je         0x5206ed
005206e1 8b45fc                   mov        eax, dword ptr [ebp - 4]
005206e4 c7402002000000           mov        dword ptr [eax + 0x20], 2
005206eb eb0a                     jmp        0x5206f7
005206ed b801000000               mov        eax, 1
005206f2 e970050000               jmp        0x520c67
005206f7 e969050000               jmp        0x520c65
005206fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005206ff 83c124                   add        ecx, 0x24
00520702 e8f9f4f9ff               call       0x4bfc00
00520707 90                       nop        
00520708 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052070b 83b98c04000000           cmp        dword ptr [ecx + 0x48c], 0
00520712 0f8546010000             jne        0x52085e
00520718 33d2                     xor        edx, edx
0052071a 7531                     jne        0x52074d
0052071c b804000000               mov        eax, 4
00520721 6bc800                   imul       ecx, eax, 0
00520724 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
0052072b 7420                     je         0x52074d
0052072d ba04000000               mov        edx, 4
00520732 6bc200                   imul       eax, edx, 0
00520735 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
0052073b 894db8                   mov        dword ptr [ebp - 0x48], ecx
0052073e 6a10                     push       0x10
00520740 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00520743 e8389befff               call       0x41a280
00520748 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052074b eb07                     jmp        0x520754
0052074d c745e800000000           mov        dword ptr [ebp - 0x18], 0
00520754 837de800                 cmp        dword ptr [ebp - 0x18], 0
00520758 7414                     je         0x52076e
0052075a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052075d 83c224                   add        edx, 0x24
00520760 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00520763 6a00                     push       0
00520765 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00520768 e8d3e8f9ff               call       0x4bf040
0052076d 90                       nop        
0052076e 33c0                     xor        eax, eax
00520770 7531                     jne        0x5207a3
00520772 b904000000               mov        ecx, 4
00520777 6bd100                   imul       edx, ecx, 0
0052077a 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00520781 7420                     je         0x5207a3
00520783 b804000000               mov        eax, 4
00520788 6bc800                   imul       ecx, eax, 0
0052078b 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00520791 8955b0                   mov        dword ptr [ebp - 0x50], edx
00520794 6a20                     push       0x20
00520796 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00520799 e8e29aefff               call       0x41a280
0052079e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005207a1 eb07                     jmp        0x5207aa
005207a3 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
005207aa 837de400                 cmp        dword ptr [ebp - 0x1c], 0
005207ae 7414                     je         0x5207c4
005207b0 8b45fc                   mov        eax, dword ptr [ebp - 4]
005207b3 83c024                   add        eax, 0x24
005207b6 8945ac                   mov        dword ptr [ebp - 0x54], eax
005207b9 6a00                     push       0
005207bb 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005207be e85de8f9ff               call       0x4bf020
005207c3 90                       nop        
005207c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005207c7 83c124                   add        ecx, 0x24
005207ca e861c8f3ff               call       0x45d030
005207cf 85c0                     test       eax, eax
005207d1 0f8482000000             je         0x520859
005207d7 c745e000000000           mov        dword ptr [ebp - 0x20], 0
005207de 6a00                     push       0
005207e0 6a0a                     push       0xa
005207e2 b930a85b00               mov        ecx, 0x5ba830
005207e7 e88465f0ff               call       0x426d70
005207ec 90                       nop        
005207ed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005207f0 83c124                   add        ecx, 0x24
005207f3 e808bbeeff               call       0x40c300
005207f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005207fb 3b81d0560000             cmp        eax, dword ptr [ecx + 0x56d0]
00520801 7d1d                     jge        0x520820
00520803 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520806 83c124                   add        ecx, 0x24
00520809 e8f2baeeff               call       0x40c300
0052080e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520811 8982d0560000             mov        dword ptr [edx + 0x56d0], eax
00520817 c745e001000000           mov        dword ptr [ebp - 0x20], 1
0052081e eb39                     jmp        0x520859
00520820 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520823 83c124                   add        ecx, 0x24
00520826 e8d5baeeff               call       0x40c300
0052082b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052082e 8b91d0560000             mov        edx, dword ptr [ecx + 0x56d0]
00520834 83c20a                   add        edx, 0xa
00520837 3bc2                     cmp        eax, edx
00520839 7c1e                     jl         0x520859
0052083b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052083e 83c124                   add        ecx, 0x24
00520841 e8babaeeff               call       0x40c300
00520846 83e809                   sub        eax, 9
00520849 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052084c 8981d0560000             mov        dword ptr [ecx + 0x56d0], eax
00520852 c745e001000000           mov        dword ptr [ebp - 0x20], 1
00520859 e92a010000               jmp        0x520988
0052085e 33d2                     xor        edx, edx
00520860 7531                     jne        0x520893
00520862 b804000000               mov        eax, 4
00520867 6bc800                   imul       ecx, eax, 0
0052086a 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00520871 7420                     je         0x520893
00520873 ba04000000               mov        edx, 4
00520878 6bc200                   imul       eax, edx, 0
0052087b 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00520881 894da8                   mov        dword ptr [ebp - 0x58], ecx
00520884 6a10                     push       0x10
00520886 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00520889 e8f299efff               call       0x41a280
0052088e 8945dc                   mov        dword ptr [ebp - 0x24], eax
00520891 eb07                     jmp        0x52089a
00520893 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0052089a 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
0052089e 0f8587000000             jne        0x52092b
005208a4 33d2                     xor        edx, edx
005208a6 7531                     jne        0x5208d9
005208a8 b804000000               mov        eax, 4
005208ad 6bc800                   imul       ecx, eax, 0
005208b0 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005208b7 7420                     je         0x5208d9
005208b9 ba04000000               mov        edx, 4
005208be 6bc200                   imul       eax, edx, 0
005208c1 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
005208c7 894da4                   mov        dword ptr [ebp - 0x5c], ecx
005208ca 6a20                     push       0x20
005208cc 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
005208cf e8ac99efff               call       0x41a280
005208d4 8945d8                   mov        dword ptr [ebp - 0x28], eax
005208d7 eb07                     jmp        0x5208e0
005208d9 c745d800000000           mov        dword ptr [ebp - 0x28], 0
005208e0 837dd800                 cmp        dword ptr [ebp - 0x28], 0
005208e4 7545                     jne        0x52092b
005208e6 33d2                     xor        edx, edx
005208e8 7534                     jne        0x52091e
005208ea b804000000               mov        eax, 4
005208ef 6bc800                   imul       ecx, eax, 0
005208f2 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
005208f9 7423                     je         0x52091e
005208fb ba04000000               mov        edx, 4
00520900 6bc200                   imul       eax, edx, 0
00520903 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00520909 894da0                   mov        dword ptr [ebp - 0x60], ecx
0052090c 6806010000               push       0x106
00520911 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00520914 e86799efff               call       0x41a280
00520919 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052091c eb07                     jmp        0x520925
0052091e c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00520925 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00520929 745d                     je         0x520988
0052092b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052092e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520931 8b8888040000             mov        ecx, dword ptr [eax + 0x488]
00520937 898a84040000             mov        dword ptr [edx + 0x484], ecx
0052093d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520940 c7828004000000000000     mov        dword ptr [edx + 0x480], 0
0052094a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052094d 8b8884040000             mov        ecx, dword ptr [eax + 0x484]
00520953 c1e106                   shl        ecx, 6
00520956 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520959 8d840a90040000           lea        eax, [edx + ecx + 0x490]
00520960 50                       push       eax
00520961 6a00                     push       0
00520963 b9404d5c00               mov        ecx, 0x5c4d40
00520968 e80391fbff               call       0x4d9a70
0052096d 6a00                     push       0
0052096f 6a00                     push       0
00520971 b9404d5c00               mov        ecx, 0x5c4d40
00520976 e8d591fbff               call       0x4d9b50
0052097b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052097e c7818c04000000000000     mov        dword ptr [ecx + 0x48c], 0
00520988 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052098b 83ba8004000008           cmp        dword ptr [edx + 0x480], 8
00520992 7d15                     jge        0x5209a9
00520994 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520997 8b8880040000             mov        ecx, dword ptr [eax + 0x480]
0052099d 83c101                   add        ecx, 1
005209a0 8b55fc                   mov        edx, dword ptr [ebp - 4]
005209a3 898a80040000             mov        dword ptr [edx + 0x480], ecx
005209a9 33c0                     xor        eax, eax
005209ab 7534                     jne        0x5209e1
005209ad b904000000               mov        ecx, 4
005209b2 6bd100                   imul       edx, ecx, 0
005209b5 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005209bc 7423                     je         0x5209e1
005209be b804000000               mov        eax, 4
005209c3 6bc800                   imul       ecx, eax, 0
005209c6 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005209cc 89559c                   mov        dword ptr [ebp - 0x64], edx
005209cf 6801000800               push       0x80001
005209d4 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005209d7 e82492efff               call       0x419c00
005209dc 8945d0                   mov        dword ptr [ebp - 0x30], eax
005209df eb07                     jmp        0x5209e8
005209e1 c745d000000000           mov        dword ptr [ebp - 0x30], 0
005209e8 837dd000                 cmp        dword ptr [ebp - 0x30], 0
005209ec 0f840d010000             je         0x520aff
005209f2 c745f800000000           mov        dword ptr [ebp - 8], 0
005209f9 eb09                     jmp        0x520a04
005209fb 8b45f8                   mov        eax, dword ptr [ebp - 8]
005209fe 83c001                   add        eax, 1
00520a01 8945f8                   mov        dword ptr [ebp - 8], eax
00520a04 837df808                 cmp        dword ptr [ebp - 8], 8
00520a08 7d28                     jge        0x520a32
00520a0a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520a0d 81c154040000             add        ecx, 0x454
00520a13 894d98                   mov        dword ptr [ebp - 0x68], ecx
00520a16 8b55f8                   mov        edx, dword ptr [ebp - 8]
00520a19 52                       push       edx
00520a1a 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00520a1d e85e3befff               call       0x414580
00520a22 894594                   mov        dword ptr [ebp - 0x6c], eax
00520a25 6a03                     push       3
00520a27 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00520a2a e861e5f2ff               call       0x44ef90
00520a2f 90                       nop        
00520a30 ebc9                     jmp        0x5209fb
00520a32 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520a35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520a38 8b9184040000             mov        edx, dword ptr [ecx + 0x484]
00520a3e 899088040000             mov        dword ptr [eax + 0x488], edx
00520a44 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520a47 83c124                   add        ecx, 0x24
00520a4a e8b1b8eeff               call       0x40c300
00520a4f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520a52 898184040000             mov        dword ptr [ecx + 0x484], eax
00520a58 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520a5b c7828004000000000000     mov        dword ptr [edx + 0x480], 0
00520a65 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00520a6b e87036f4ff               call       0x4640e0
00520a70 83c036                   add        eax, 0x36
00520a73 89458c                   mov        dword ptr [ebp - 0x74], eax
00520a76 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520a79 8b8884040000             mov        ecx, dword ptr [eax + 0x484]
00520a7f 894d90                   mov        dword ptr [ebp - 0x70], ecx
00520a82 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00520a85 52                       push       edx
00520a86 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00520a89 e88211efff               call       0x411c10
00520a8e 0fb600                   movzx      eax, byte ptr [eax]
00520a91 85c0                     test       eax, eax
00520a93 750c                     jne        0x520aa1
00520a95 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520a98 83b98c04000000           cmp        dword ptr [ecx + 0x48c], 0
00520a9f 7442                     je         0x520ae3
00520aa1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520aa4 83c124                   add        ecx, 0x24
00520aa7 e854b8eeff               call       0x40c300
00520aac c1e006                   shl        eax, 6
00520aaf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520ab2 8d840290040000           lea        eax, [edx + eax + 0x490]
00520ab9 50                       push       eax
00520aba 6a00                     push       0
00520abc b9404d5c00               mov        ecx, 0x5c4d40
00520ac1 e8aa8ffbff               call       0x4d9a70
00520ac6 6a00                     push       0
00520ac8 6a00                     push       0
00520aca b9404d5c00               mov        ecx, 0x5c4d40
00520acf e87c90fbff               call       0x4d9b50
00520ad4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520ad7 c7818c04000000000000     mov        dword ptr [ecx + 0x48c], 0
00520ae1 eb17                     jmp        0x520afa
00520ae3 b9404d5c00               mov        ecx, 0x5c4d40
00520ae8 e8d390fbff               call       0x4d9bc0
00520aed 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520af0 c7828c04000001000000     mov        dword ptr [edx + 0x48c], 1
00520afa e966010000               jmp        0x520c65
00520aff 33c0                     xor        eax, eax
00520b01 7534                     jne        0x520b37
00520b03 b904000000               mov        ecx, 4
00520b08 6bd100                   imul       edx, ecx, 0
00520b0b 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00520b12 7423                     je         0x520b37
00520b14 b804000000               mov        eax, 4
00520b19 6bc800                   imul       ecx, eax, 0
00520b1c 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00520b22 895588                   mov        dword ptr [ebp - 0x78], edx
00520b25 6806010000               push       0x106
00520b2a 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00520b2d e8ce90efff               call       0x419c00
00520b32 8945cc                   mov        dword ptr [ebp - 0x34], eax
00520b35 eb07                     jmp        0x520b3e
00520b37 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
00520b3e 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
00520b42 0f8498000000             je         0x520be0
00520b48 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00520b4f eb09                     jmp        0x520b5a
00520b51 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00520b54 83c001                   add        eax, 1
00520b57 8945f4                   mov        dword ptr [ebp - 0xc], eax
00520b5a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520b5d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00520b60 3b917c040000             cmp        edx, dword ptr [ecx + 0x47c]
00520b66 7d21                     jge        0x520b89
00520b68 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520b6b 05d4030000               add        eax, 0x3d4
00520b70 894584                   mov        dword ptr [ebp - 0x7c], eax
00520b73 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00520b76 51                       push       ecx
00520b77 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00520b7a e8013aefff               call       0x414580
00520b7f 8bc8                     mov        ecx, eax
00520b81 e8ba84f5ff               call       0x479040
00520b86 90                       nop        
00520b87 ebc8                     jmp        0x520b51
00520b89 c745f000000000           mov        dword ptr [ebp - 0x10], 0
00520b90 eb09                     jmp        0x520b9b
00520b92 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00520b95 83c201                   add        edx, 1
00520b98 8955f0                   mov        dword ptr [ebp - 0x10], edx
00520b9b 837df008                 cmp        dword ptr [ebp - 0x10], 8
00520b9f 7d21                     jge        0x520bc2
00520ba1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00520ba4 0554040000               add        eax, 0x454
00520ba9 894580                   mov        dword ptr [ebp - 0x80], eax
00520bac 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00520baf 51                       push       ecx
00520bb0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00520bb3 e8c839efff               call       0x414580
00520bb8 8bc8                     mov        ecx, eax
00520bba e88184f5ff               call       0x479040
00520bbf 90                       nop        
00520bc0 ebd0                     jmp        0x520b92
00520bc2 6a00                     push       0
00520bc4 6a09                     push       9
00520bc6 b930a85b00               mov        ecx, 0x5ba830
00520bcb e8a061f0ff               call       0x426d70
00520bd0 6a03                     push       3
00520bd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520bd5 e876c20000               call       0x52ce50
00520bda 90                       nop        
00520bdb e985000000               jmp        0x520c65
00520be0 e980000000               jmp        0x520c65
00520be5 8b55fc                   mov        edx, dword ptr [ebp - 4]
00520be8 81c254010000             add        edx, 0x154
00520bee 89957cffffff             mov        dword ptr [ebp - 0x84], edx
00520bf4 6a0a                     push       0xa
00520bf6 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00520bfc e8bf29f0ff               call       0x4235c0
00520c01 0fb6c0                   movzx      eax, al
00520c04 85c0                     test       eax, eax
00520c06 745d                     je         0x520c65
00520c08 6a28                     push       0x28
00520c0a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520c0d e81ec00000               call       0x52cc30
00520c12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520c15 81c190030000             add        ecx, 0x390
00520c1b e82084f5ff               call       0x479040
00520c20 6a00                     push       0
00520c22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520c25 81c190030000             add        ecx, 0x390
00520c2b e8700befff               call       0x4117a0
00520c30 6a01                     push       1
00520c32 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520c35 e896c20000               call       0x52ced0
00520c3a 6898505700               push       0x575098
00520c3f 6a00                     push       0
00520c41 b9404d5c00               mov        ecx, 0x5c4d40
00520c46 e8258efbff               call       0x4d9a70
00520c4b 6a00                     push       0
00520c4d 6a00                     push       0
00520c4f b9404d5c00               mov        ecx, 0x5c4d40
00520c54 e8f78efbff               call       0x4d9b50
00520c59 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520c5c 83c124                   add        ecx, 0x24
00520c5f e8ac53faff               call       0x4c6010
00520c64 90                       nop        
00520c65 33c0                     xor        eax, eax
00520c67 8be5                     mov        esp, ebp
00520c69 5d                       pop        ebp
00520c6a c3                       ret        
00520c6b 90                       nop        
00520c6c f9                       stc        
00520c6d 055200bb06               add        eax, 0x6bb0052
00520c72 52                       push       edx
00520c73 00fc                     add        ah, bh
00520c75 06                       push       es
00520c76 52                       push       edx
00520c77 00e5                     add        ch, ah
00520c79 0b5200                   or         edx, dword ptr [edx]
00520c7c cc                       int3       
00520c7d cc                       int3       
00520c7e cc                       int3       
00520c7f cc                       int3       
