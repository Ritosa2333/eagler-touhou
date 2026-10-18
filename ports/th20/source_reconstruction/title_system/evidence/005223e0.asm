005223e0 55                       push       ebp
005223e1 8bec                     mov        ebp, esp
005223e3 81ecbc000000             sub        esp, 0xbc
005223e9 56                       push       esi
005223ea 894dfc                   mov        dword ptr [ebp - 4], ecx
005223ed 8b45fc                   mov        eax, dword ptr [ebp - 4]
005223f0 8b4820                   mov        ecx, dword ptr [eax + 0x20]
005223f3 894dec                   mov        dword ptr [ebp - 0x14], ecx
005223f6 837dec03                 cmp        dword ptr [ebp - 0x14], 3
005223fa 0f87020a0000             ja         0x522e02
00522400 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00522403 ff24950c2e5200           jmp        dword ptr [edx*4 + 0x522e0c]
0052240a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052240d 83c024                   add        eax, 0x24
00522410 8945cc                   mov        dword ptr [ebp - 0x34], eax
00522413 6a1e                     push       0x1e
00522415 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00522418 e8c3a0f7ff               call       0x49c4e0
0052241d 6828265700               push       0x572628
00522422 6a00                     push       0
00522424 b9404d5c00               mov        ecx, 0x5c4d40
00522429 e84276fbff               call       0x4d9a70
0052242e 6a11                     push       0x11
00522430 6a00                     push       0
00522432 b9404d5c00               mov        ecx, 0x5c4d40
00522437 e81477fbff               call       0x4d9b50
0052243c 90                       nop        
0052243d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522440 81c190030000             add        ecx, 0x390
00522446 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00522449 6a00                     push       0
0052244b 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052244e e8dd6ff2ff               call       0x449430
00522453 0fb6d0                   movzx      edx, al
00522456 85d2                     test       edx, edx
00522458 7430                     je         0x52248a
0052245a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522460 e8eb54f1ff               call       0x437950
00522465 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00522468 6a00                     push       0
0052246a 6aff                     push       -1
0052246c 6a13                     push       0x13
0052246e 6a00                     push       0
00522470 8d8544ffffff             lea        eax, [ebp - 0xbc]
00522476 50                       push       eax
00522477 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0052247a e8f1e7f2ff               call       0x450c70
0052247f 8b08                     mov        ecx, dword ptr [eax]
00522481 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522484 898a90030000             mov        dword ptr [edx + 0x390], ecx
0052248a 6a29                     push       0x29
0052248c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052248f e8ecaa0000               call       0x52cf80
00522494 6a01                     push       1
00522496 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522499 e8b2a90000               call       0x52ce50
0052249e e8bd94ffff               call       0x51b960
005224a3 8945c0                   mov        dword ptr [ebp - 0x40], eax
005224a6 6a02                     push       2
005224a8 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005224ab e8007cffff               call       0x51a0b0
005224b0 e8ab94ffff               call       0x51b960
005224b5 8945b8                   mov        dword ptr [ebp - 0x48], eax
005224b8 6a00                     push       0
005224ba e8c11bf4ff               call       0x464080
005224bf 83c404                   add        esp, 4
005224c2 8bc8                     mov        ecx, eax
005224c4 e8e7a5efff               call       0x41cab0
005224c9 8945bc                   mov        dword ptr [ebp - 0x44], eax
005224cc 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
005224cf 50                       push       eax
005224d0 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005224d3 e8a8a90000               call       0x52ce80
005224d8 6a48                     push       0x48
005224da 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005224dd e89eaa0000               call       0x52cf80
005224e2 6a00                     push       0
005224e4 e8971bf4ff               call       0x464080
005224e9 83c404                   add        esp, 4
005224ec 8bc8                     mov        ecx, eax
005224ee e83d02efff               call       0x412730
005224f3 8bf0                     mov        esi, eax
005224f5 6a00                     push       0
005224f7 e8841bf4ff               call       0x464080
005224fc 83c404                   add        esp, 4
005224ff 8bc8                     mov        ecx, eax
00522501 e8aaa5efff               call       0x41cab0
00522506 99                       cdq        
00522507 2bc2                     sub        eax, edx
00522509 d1f8                     sar        eax, 1
0052250b 8d4cb025                 lea        ecx, [eax + esi*4 + 0x25]
0052250f 51                       push       ecx
00522510 6a48                     push       0x48
00522512 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522515 e8f6a70000               call       0x52cd10
0052251a b968a55b00               mov        ecx, 0x5ba568
0052251f e87c1bf4ff               call       0x4640a0
00522524 83c040                   add        eax, 0x40
00522527 50                       push       eax
00522528 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052252b e850aa0000               call       0x52cf80
00522530 90                       nop        
00522531 6a00                     push       0
00522533 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522536 e815a20000               call       0x52c750
0052253b 85c0                     test       eax, eax
0052253d 7515                     jne        0x522554
0052253f 6a00                     push       0
00522541 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522544 e837aa0000               call       0x52cf80
00522549 6a00                     push       0
0052254b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052254e e83da70000               call       0x52cc90
00522553 90                       nop        
00522554 b968a55b00               mov        ecx, 0x5ba568
00522559 e8e25df9ff               call       0x4b8340
0052255e 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00522561 b968a55b00               mov        ecx, 0x5ba568
00522566 e8055af9ff               call       0x4b7f70
0052256b 8945b0                   mov        dword ptr [ebp - 0x50], eax
0052256e 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00522571 52                       push       edx
00522572 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00522575 50                       push       eax
00522576 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052257c e8dfaef9ff               call       0x4bd460
00522581 8bc8                     mov        ecx, eax
00522583 e8e8cbfeff               call       0x50f170
00522588 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052258b 6a00                     push       0
0052258d b968a55b00               mov        ecx, 0x5ba568
00522592 e8c9bdf9ff               call       0x4be360
00522597 90                       nop        
00522598 837de800                 cmp        dword ptr [ebp - 0x18], 0
0052259c 0f8c18010000             jl         0x5226ba
005225a2 6a00                     push       0
005225a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005225a7 81c154010000             add        ecx, 0x154
005225ad e86e0ff0ff               call       0x423520
005225b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005225b5 83c124                   add        ecx, 0x24
005225b8 894dac                   mov        dword ptr [ebp - 0x54], ecx
005225bb 6a01                     push       1
005225bd 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005225c0 e81bd6f9ff               call       0x4bfbe0
005225c5 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
005225c8 52                       push       edx
005225c9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005225cc 83c124                   add        ecx, 0x24
005225cf e87cc7f9ff               call       0x4bed50
005225d4 6a00                     push       0
005225d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005225d9 81c1e8560000             add        ecx, 0x56e8
005225df e86cc7f9ff               call       0x4bed50
005225e4 8b45fc                   mov        eax, dword ptr [ebp - 4]
005225e7 05e8560000               add        eax, 0x56e8
005225ec 8945a8                   mov        dword ptr [ebp - 0x58], eax
005225ef 8b0d7cff5a00             mov        ecx, dword ptr [0x5aff7c]
005225f5 51                       push       ecx
005225f6 e825d80200               call       0x54fe20
005225fb 83c404                   add        esp, 4
005225fe 50                       push       eax
005225ff 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00522602 e8d99ef7ff               call       0x49c4e0
00522607 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052260a 81c2e8560000             add        edx, 0x56e8
00522610 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00522613 6a01                     push       1
00522615 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00522618 e8c3d5f9ff               call       0x4bfbe0
0052261d 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00522623 e8b81af4ff               call       0x4640e0
00522628 83c00c                   add        eax, 0xc
0052262b 50                       push       eax
0052262c 6a0a                     push       0xa
0052262e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522631 05d4560000               add        eax, 0x56d4
00522636 50                       push       eax
00522637 e8d45f0200               call       0x548610
0052263c 83c40c                   add        esp, 0xc
0052263f 68ac245700               push       0x5724ac
00522644 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522647 81c1d4560000             add        ecx, 0x56d4
0052264d 51                       push       ecx
0052264e e8bdd60200               call       0x54fd10
00522653 83c408                   add        esp, 8
00522656 85c0                     test       eax, eax
00522658 7417                     je         0x522671
0052265a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052265d 81c2e8560000             add        edx, 0x56e8
00522663 8955a0                   mov        dword ptr [ebp - 0x60], edx
00522666 6a00                     push       0
00522668 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0052266b e8d0c9f9ff               call       0x4bf040
00522670 90                       nop        
00522671 c745f008000000           mov        dword ptr [ebp - 0x10], 8
00522678 eb09                     jmp        0x522683
0052267a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052267d 83e801                   sub        eax, 1
00522680 8945f0                   mov        dword ptr [ebp - 0x10], eax
00522683 837df000                 cmp        dword ptr [ebp - 0x10], 0
00522687 7e16                     jle        0x52269f
00522689 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052268c 034df0                   add        ecx, dword ptr [ebp - 0x10]
0052268f 0fbe91d3560000           movsx      edx, byte ptr [ecx + 0x56d3]
00522696 83fa20                   cmp        edx, 0x20
00522699 7402                     je         0x52269d
0052269b eb02                     jmp        0x52269f
0052269d ebdb                     jmp        0x52267a
0052269f 8b45fc                   mov        eax, dword ptr [ebp - 4]
005226a2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005226a5 8988e0560000             mov        dword ptr [eax + 0x56e0], ecx
005226ab 8b55fc                   mov        edx, dword ptr [ebp - 4]
005226ae c782e456000000000000     mov        dword ptr [edx + 0x56e4], 0
005226b8 eb1a                     jmp        0x5226d4
005226ba 6aff                     push       -1
005226bc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005226bf 83c124                   add        ecx, 0x24
005226c2 e889c6f9ff               call       0x4bed50
005226c7 8b45fc                   mov        eax, dword ptr [ebp - 4]
005226ca c780e456000001000000     mov        dword ptr [eax + 0x56e4], 1
005226d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005226d7 81c154010000             add        ecx, 0x154
005226dd 894d9c                   mov        dword ptr [ebp - 0x64], ecx
005226e0 6a06                     push       6
005226e2 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005226e5 e886e9f3ff               call       0x461070
005226ea 0fb6d0                   movzx      edx, al
005226ed 85d2                     test       edx, edx
005226ef 740b                     je         0x5226fc
005226f1 6a02                     push       2
005226f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005226f6 e855a70000               call       0x52ce50
005226fb 90                       nop        
005226fc e901070000               jmp        0x522e02
00522701 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522704 83b8e456000000           cmp        dword ptr [eax + 0x56e4], 0
0052270b 0f85e6010000             jne        0x5228f7
00522711 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522714 81c1e8560000             add        ecx, 0x56e8
0052271a e8e1d4f9ff               call       0x4bfc00
0052271f 90                       nop        
00522720 33c9                     xor        ecx, ecx
00522722 7531                     jne        0x522755
00522724 ba04000000               mov        edx, 4
00522729 6bc200                   imul       eax, edx, 0
0052272c 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00522733 7420                     je         0x522755
00522735 b904000000               mov        ecx, 4
0052273a 6bd100                   imul       edx, ecx, 0
0052273d 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00522743 894598                   mov        dword ptr [ebp - 0x68], eax
00522746 6a10                     push       0x10
00522748 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0052274b e8307befff               call       0x41a280
00522750 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00522753 eb07                     jmp        0x52275c
00522755 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0052275c 837de400                 cmp        dword ptr [ebp - 0x1c], 0
00522760 7411                     je         0x522773
00522762 6a0d                     push       0xd
00522764 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522767 81c1e8560000             add        ecx, 0x56e8
0052276d e8aef8fbff               call       0x4e2020
00522772 90                       nop        
00522773 33c9                     xor        ecx, ecx
00522775 7531                     jne        0x5227a8
00522777 ba04000000               mov        edx, 4
0052277c 6bc200                   imul       eax, edx, 0
0052277f 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00522786 7420                     je         0x5227a8
00522788 b904000000               mov        ecx, 4
0052278d 6bd100                   imul       edx, ecx, 0
00522790 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00522796 894594                   mov        dword ptr [ebp - 0x6c], eax
00522799 6a20                     push       0x20
0052279b 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0052279e e8dd7aefff               call       0x41a280
005227a3 8945e0                   mov        dword ptr [ebp - 0x20], eax
005227a6 eb07                     jmp        0x5227af
005227a8 c745e000000000           mov        dword ptr [ebp - 0x20], 0
005227af 837de000                 cmp        dword ptr [ebp - 0x20], 0
005227b3 7411                     je         0x5227c6
005227b5 6a0d                     push       0xd
005227b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005227ba 81c1e8560000             add        ecx, 0x56e8
005227c0 e83bf8fbff               call       0x4e2000
005227c5 90                       nop        
005227c6 33c9                     xor        ecx, ecx
005227c8 7531                     jne        0x5227fb
005227ca ba04000000               mov        edx, 4
005227cf 6bc200                   imul       eax, edx, 0
005227d2 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005227d9 7420                     je         0x5227fb
005227db b904000000               mov        ecx, 4
005227e0 6bd100                   imul       edx, ecx, 0
005227e3 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005227e9 894590                   mov        dword ptr [ebp - 0x70], eax
005227ec 6a40                     push       0x40
005227ee 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005227f1 e88a7aefff               call       0x41a280
005227f6 8945dc                   mov        dword ptr [ebp - 0x24], eax
005227f9 eb07                     jmp        0x522802
005227fb c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00522802 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00522806 7444                     je         0x52284c
00522808 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052280b 81c1e8560000             add        ecx, 0x56e8
00522811 e8ea9aeeff               call       0x40c300
00522816 99                       cdq        
00522817 b90d000000               mov        ecx, 0xd
0052281c f7f9                     idiv       ecx
0052281e 85d2                     test       edx, edx
00522820 7419                     je         0x52283b
00522822 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522825 81c2e8560000             add        edx, 0x56e8
0052282b 89558c                   mov        dword ptr [ebp - 0x74], edx
0052282e 6a00                     push       0
00522830 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00522833 e808c8f9ff               call       0x4bf040
00522838 90                       nop        
00522839 eb11                     jmp        0x52284c
0052283b 6a0c                     push       0xc
0052283d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522840 81c1e8560000             add        ecx, 0x56e8
00522846 e8b5f7fbff               call       0x4e2000
0052284b 90                       nop        
0052284c 33c0                     xor        eax, eax
0052284e 7534                     jne        0x522884
00522850 b904000000               mov        ecx, 4
00522855 6bd100                   imul       edx, ecx, 0
00522858 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052285f 7423                     je         0x522884
00522861 b804000000               mov        eax, 4
00522866 6bc800                   imul       ecx, eax, 0
00522869 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052286f 895588                   mov        dword ptr [ebp - 0x78], edx
00522872 6880000000               push       0x80
00522877 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0052287a e8017aefff               call       0x41a280
0052287f 8945d8                   mov        dword ptr [ebp - 0x28], eax
00522882 eb07                     jmp        0x52288b
00522884 c745d800000000           mov        dword ptr [ebp - 0x28], 0
0052288b 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0052288f 7445                     je         0x5228d6
00522891 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522894 81c1e8560000             add        ecx, 0x56e8
0052289a e8619aeeff               call       0x40c300
0052289f 99                       cdq        
005228a0 b90d000000               mov        ecx, 0xd
005228a5 f7f9                     idiv       ecx
005228a7 83fa0c                   cmp        edx, 0xc
005228aa 7419                     je         0x5228c5
005228ac 8b55fc                   mov        edx, dword ptr [ebp - 4]
005228af 81c2e8560000             add        edx, 0x56e8
005228b5 895584                   mov        dword ptr [ebp - 0x7c], edx
005228b8 6a00                     push       0
005228ba 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005228bd e85ec7f9ff               call       0x4bf020
005228c2 90                       nop        
005228c3 eb11                     jmp        0x5228d6
005228c5 6a0c                     push       0xc
005228c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005228ca 81c1e8560000             add        ecx, 0x56e8
005228d0 e84bf7fbff               call       0x4e2020
005228d5 90                       nop        
005228d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005228d9 81c1e8560000             add        ecx, 0x56e8
005228df e84ca7f3ff               call       0x45d030
005228e4 85c0                     test       eax, eax
005228e6 740f                     je         0x5228f7
005228e8 6a00                     push       0
005228ea 6a0a                     push       0xa
005228ec b930a85b00               mov        ecx, 0x5ba830
005228f1 e87a44f0ff               call       0x426d70
005228f6 90                       nop        
005228f7 33c0                     xor        eax, eax
005228f9 7534                     jne        0x52292f
005228fb b904000000               mov        ecx, 4
00522900 6bd100                   imul       edx, ecx, 0
00522903 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052290a 7423                     je         0x52292f
0052290c b804000000               mov        eax, 4
00522911 6bc800                   imul       ecx, eax, 0
00522914 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052291a 895580                   mov        dword ptr [ebp - 0x80], edx
0052291d 6801000800               push       0x80001
00522922 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00522925 e8d672efff               call       0x419c00
0052292a 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052292d eb07                     jmp        0x522936
0052292f c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00522936 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
0052293a 0f8417030000             je         0x522c57
00522940 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522943 83b8e456000000           cmp        dword ptr [eax + 0x56e4], 0
0052294a 0f85ee020000             jne        0x522c3e
00522950 8b0d7cff5a00             mov        ecx, dword ptr [0x5aff7c]
00522956 51                       push       ecx
00522957 e8c4d40200               call       0x54fe20
0052295c 83c404                   add        esp, 4
0052295f 8945f4                   mov        dword ptr [ebp - 0xc], eax
00522962 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522965 81c2e8560000             add        edx, 0x56e8
0052296b 89957cffffff             mov        dword ptr [ebp - 0x84], edx
00522971 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00522974 83e803                   sub        eax, 3
00522977 50                       push       eax
00522978 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0052297e e84df6fbff               call       0x4e1fd0
00522983 85c0                     test       eax, eax
00522985 0f84dc000000             je         0x522a67
0052298b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052298e 83b9e056000008           cmp        dword ptr [ecx + 0x56e0], 8
00522995 0f8d80000000             jge        0x522a1b
0052299b 8b157cff5a00             mov        edx, dword ptr [0x5aff7c]
005229a1 899578ffffff             mov        dword ptr [ebp - 0x88], edx
005229a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005229aa 81c1e8560000             add        ecx, 0x56e8
005229b0 e84b99eeff               call       0x40c300
005229b5 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
005229bb 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
005229c1 038574ffffff             add        eax, dword ptr [ebp - 0x8c]
005229c7 8a08                     mov        cl, byte ptr [eax]
005229c9 884dfb                   mov        byte ptr [ebp - 5], cl
005229cc 8b55fc                   mov        edx, dword ptr [ebp - 4]
005229cf 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
005229d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005229d8 8a55fb                   mov        dl, byte ptr [ebp - 5]
005229db 889401d4560000           mov        byte ptr [ecx + eax + 0x56d4], dl
005229e2 8b45fc                   mov        eax, dword ptr [ebp - 4]
005229e5 8b88e0560000             mov        ecx, dword ptr [eax + 0x56e0]
005229eb 83c101                   add        ecx, 1
005229ee 8b55fc                   mov        edx, dword ptr [ebp - 4]
005229f1 898ae0560000             mov        dword ptr [edx + 0x56e0], ecx
005229f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
005229fa 83b8e056000008           cmp        dword ptr [eax + 0x56e0], 8
00522a01 7c16                     jl         0x522a19
00522a03 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00522a06 83e901                   sub        ecx, 1
00522a09 51                       push       ecx
00522a0a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522a0d 81c1e8560000             add        ecx, 0x56e8
00522a13 e838c3f9ff               call       0x4bed50
00522a18 90                       nop        
00522a19 eb47                     jmp        0x522a62
00522a1b 8b157cff5a00             mov        edx, dword ptr [0x5aff7c]
00522a21 899570ffffff             mov        dword ptr [ebp - 0x90], edx
00522a27 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522a2a 81c1e8560000             add        ecx, 0x56e8
00522a30 e8cb98eeff               call       0x40c300
00522a35 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00522a3b 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
00522a41 03856cffffff             add        eax, dword ptr [ebp - 0x94]
00522a47 8a08                     mov        cl, byte ptr [eax]
00522a49 884dfa                   mov        byte ptr [ebp - 6], cl
00522a4c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522a4f 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522a55 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522a58 8a55fa                   mov        dl, byte ptr [ebp - 6]
00522a5b 889401d3560000           mov        byte ptr [ecx + eax + 0x56d3], dl
00522a62 e9c6010000               jmp        0x522c2d
00522a67 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522a6a 05e8560000               add        eax, 0x56e8
00522a6f 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00522a75 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00522a78 83e903                   sub        ecx, 3
00522a7b 51                       push       ecx
00522a7c 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00522a82 e82987f4ff               call       0x46b1b0
00522a87 85c0                     test       eax, eax
00522a89 7472                     je         0x522afd
00522a8b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522a8e 83bae056000008           cmp        dword ptr [edx + 0x56e0], 8
00522a95 7d4d                     jge        0x522ae4
00522a97 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522a9a 8b88e0560000             mov        ecx, dword ptr [eax + 0x56e0]
00522aa0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522aa3 c6840ad456000020         mov        byte ptr [edx + ecx + 0x56d4], 0x20
00522aab 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522aae 8b88e0560000             mov        ecx, dword ptr [eax + 0x56e0]
00522ab4 83c101                   add        ecx, 1
00522ab7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522aba 898ae0560000             mov        dword ptr [edx + 0x56e0], ecx
00522ac0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522ac3 83b8e056000008           cmp        dword ptr [eax + 0x56e0], 8
00522aca 7c16                     jl         0x522ae2
00522acc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00522acf 83e901                   sub        ecx, 1
00522ad2 51                       push       ecx
00522ad3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522ad6 81c1e8560000             add        ecx, 0x56e8
00522adc e86fc2f9ff               call       0x4bed50
00522ae1 90                       nop        
00522ae2 eb14                     jmp        0x522af8
00522ae4 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522ae7 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522aed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522af0 c68401d356000020         mov        byte ptr [ecx + eax + 0x56d3], 0x20
00522af8 e930010000               jmp        0x522c2d
00522afd 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522b00 81c2e8560000             add        edx, 0x56e8
00522b06 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
00522b0c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00522b0f 83e802                   sub        eax, 2
00522b12 50                       push       eax
00522b13 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00522b19 e89286f4ff               call       0x46b1b0
00522b1e 85c0                     test       eax, eax
00522b20 743f                     je         0x522b61
00522b22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522b25 83b9e056000000           cmp        dword ptr [ecx + 0x56e0], 0
00522b2c 7505                     jne        0x522b33
00522b2e e9cf020000               jmp        0x522e02
00522b33 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522b36 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522b3c 83e801                   sub        eax, 1
00522b3f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522b42 8981e0560000             mov        dword ptr [ecx + 0x56e0], eax
00522b48 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522b4b 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522b51 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522b54 c68401d456000020         mov        byte ptr [ecx + eax + 0x56d4], 0x20
00522b5c e9cc000000               jmp        0x522c2d
00522b61 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522b64 81c2e8560000             add        edx, 0x56e8
00522b6a 899560ffffff             mov        dword ptr [ebp - 0xa0], edx
00522b70 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00522b73 83e801                   sub        eax, 1
00522b76 50                       push       eax
00522b77 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00522b7d e82e86f4ff               call       0x46b1b0
00522b82 85c0                     test       eax, eax
00522b84 0f84a3000000             je         0x522c2d
00522b8a 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00522b90 e8bbd0feff               call       0x50fc50
00522b95 83c018                   add        eax, 0x18
00522b98 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
00522b9e b968a55b00               mov        ecx, 0x5ba568
00522ba3 e8f814f4ff               call       0x4640a0
00522ba8 69c890010000             imul       ecx, eax, 0x190
00522bae 898d58ffffff             mov        dword ptr [ebp - 0xa8], ecx
00522bb4 8b955cffffff             mov        edx, dword ptr [ebp - 0xa4]
00522bba 039558ffffff             add        edx, dword ptr [ebp - 0xa8]
00522bc0 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
00522bc6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522bc9 83c124                   add        ecx, 0x24
00522bcc e82f97eeff               call       0x40c300
00522bd1 6bc028                   imul       eax, eax, 0x28
00522bd4 898554ffffff             mov        dword ptr [ebp - 0xac], eax
00522bda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522bdd 81c1d4560000             add        ecx, 0x56d4
00522be3 51                       push       ecx
00522be4 6a0a                     push       0xa
00522be6 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
00522bec 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
00522bf2 8d4c100a                 lea        ecx, [eax + edx + 0xa]
00522bf6 51                       push       ecx
00522bf7 e8145a0200               call       0x548610
00522bfc 83c40c                   add        esp, 0xc
00522bff 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522c02 81c2d4560000             add        edx, 0x56d4
00522c08 52                       push       edx
00522c09 6a0a                     push       0xa
00522c0b 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00522c11 e8ca14f4ff               call       0x4640e0
00522c16 83c00c                   add        eax, 0xc
00522c19 50                       push       eax
00522c1a e8f1590200               call       0x548610
00522c1f 83c40c                   add        esp, 0xc
00522c22 6a03                     push       3
00522c24 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522c27 e824a20000               call       0x52ce50
00522c2c 90                       nop        
00522c2d 6a00                     push       0
00522c2f 6a07                     push       7
00522c31 b930a85b00               mov        ecx, 0x5ba830
00522c36 e83541f0ff               call       0x426d70
00522c3b 90                       nop        
00522c3c eb19                     jmp        0x522c57
00522c3e 6a03                     push       3
00522c40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522c43 e808a20000               call       0x52ce50
00522c48 6a00                     push       0
00522c4a 6a07                     push       7
00522c4c b930a85b00               mov        ecx, 0x5ba830
00522c51 e81a41f0ff               call       0x426d70
00522c56 90                       nop        
00522c57 33c0                     xor        eax, eax
00522c59 753a                     jne        0x522c95
00522c5b b904000000               mov        ecx, 4
00522c60 6bd100                   imul       edx, ecx, 0
00522c63 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00522c6a 7429                     je         0x522c95
00522c6c b804000000               mov        eax, 4
00522c71 6bc800                   imul       ecx, eax, 0
00522c74 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00522c7a 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
00522c80 6806010000               push       0x106
00522c85 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00522c8b e8706fefff               call       0x419c00
00522c90 8945d0                   mov        dword ptr [ebp - 0x30], eax
00522c93 eb07                     jmp        0x522c9c
00522c95 c745d000000000           mov        dword ptr [ebp - 0x30], 0
00522c9c 837dd000                 cmp        dword ptr [ebp - 0x30], 0
00522ca0 746a                     je         0x522d0c
00522ca2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522ca5 83b8e456000000           cmp        dword ptr [eax + 0x56e4], 0
00522cac 7545                     jne        0x522cf3
00522cae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522cb1 83b9e056000000           cmp        dword ptr [ecx + 0x56e0], 0
00522cb8 7437                     je         0x522cf1
00522cba 6a00                     push       0
00522cbc 6a09                     push       9
00522cbe b930a85b00               mov        ecx, 0x5ba830
00522cc3 e8a840f0ff               call       0x426d70
00522cc8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522ccb 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522cd1 83e801                   sub        eax, 1
00522cd4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522cd7 8981e0560000             mov        dword ptr [ecx + 0x56e0], eax
00522cdd 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522ce0 8b82e0560000             mov        eax, dword ptr [edx + 0x56e0]
00522ce6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522ce9 c68401d456000020         mov        byte ptr [ecx + eax + 0x56d4], 0x20
00522cf1 eb19                     jmp        0x522d0c
00522cf3 6a03                     push       3
00522cf5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522cf8 e853a10000               call       0x52ce50
00522cfd 6a00                     push       0
00522cff 6a07                     push       7
00522d01 b930a85b00               mov        ecx, 0x5ba830
00522d06 e86540f0ff               call       0x426d70
00522d0b 90                       nop        
00522d0c e9f1000000               jmp        0x522e02
00522d11 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522d14 81c254010000             add        edx, 0x154
00522d1a 899548ffffff             mov        dword ptr [ebp - 0xb8], edx
00522d20 6a06                     push       6
00522d22 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00522d28 e89308f0ff               call       0x4235c0
00522d2d 0fb6c0                   movzx      eax, al
00522d30 85c0                     test       eax, eax
00522d32 0f84ca000000             je         0x522e02
00522d38 e8238cffff               call       0x51b960
00522d3d 8bc8                     mov        ecx, eax
00522d3f e8fc6affff               call       0x519840
00522d44 6a29                     push       0x29
00522d46 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522d49 e8e29e0000               call       0x52cc30
00522d4e b968a55b00               mov        ecx, 0x5ba568
00522d53 e8e851f9ff               call       0x4b7f40
00522d58 83c03c                   add        eax, 0x3c
00522d5b 50                       push       eax
00522d5c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522d5f e8cc9e0000               call       0x52cc30
00522d64 b968a55b00               mov        ecx, 0x5ba568
00522d69 e83213f4ff               call       0x4640a0
00522d6e 83c040                   add        eax, 0x40
00522d71 50                       push       eax
00522d72 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522d75 e8b69e0000               call       0x52cc30
00522d7a 6a48                     push       0x48
00522d7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522d7f e8ac9e0000               call       0x52cc30
00522d84 90                       nop        
00522d85 b968a55b00               mov        ecx, 0x5ba568
00522d8a e81152f9ff               call       0x4b7fa0
00522d8f 85c0                     test       eax, eax
00522d91 750d                     jne        0x522da0
00522d93 6a10                     push       0x10
00522d95 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522d98 e833a10000               call       0x52ced0
00522d9d 90                       nop        
00522d9e eb62                     jmp        0x522e02
00522da0 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
00522da6 51                       push       ecx
00522da7 e814eaefff               call       0x4217c0
00522dac 83c404                   add        esp, 4
00522daf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522db2 81c190030000             add        ecx, 0x390
00522db8 e88362f5ff               call       0x479040
00522dbd 6a00                     push       0
00522dbf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522dc2 81c190030000             add        ecx, 0x390
00522dc8 e8d3e9eeff               call       0x4117a0
00522dcd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522dd0 83c124                   add        ecx, 0x24
00522dd3 e83832faff               call       0x4c6010
00522dd8 6a01                     push       1
00522dda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522ddd e8eea00000               call       0x52ced0
00522de2 6898505700               push       0x575098
00522de7 6a00                     push       0
00522de9 b9404d5c00               mov        ecx, 0x5c4d40
00522dee e87d6cfbff               call       0x4d9a70
00522df3 6a00                     push       0
00522df5 6a00                     push       0
00522df7 b9404d5c00               mov        ecx, 0x5c4d40
00522dfc e84f6dfbff               call       0x4d9b50
00522e01 90                       nop        
00522e02 b801000000               mov        eax, 1
00522e07 5e                       pop        esi
00522e08 8be5                     mov        esp, ebp
00522e0a 5d                       pop        ebp
00522e0b c3                       ret        
00522e0c 0a2452                   or         ah, byte ptr [edx + edx*2]
00522e0f 00d4                     add        ah, dl
00522e11 2652                     push       edx
00522e13 0001                     add        byte ptr [ecx], al
00522e15 27                       daa        
00522e16 52                       push       edx
00522e17 0011                     add        byte ptr [ecx], dl
00522e19 2d5200cccc               sub        eax, 0xcccc0052
00522e1e cc                       int3       
00522e1f cc                       int3       
