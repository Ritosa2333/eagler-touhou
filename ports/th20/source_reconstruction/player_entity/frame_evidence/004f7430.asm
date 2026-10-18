004f7430 55                       push       ebp
004f7431 8bec                     mov        ebp, esp
004f7433 81ec88010000             sub        esp, 0x188
004f7439 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004f743e 33c5                     xor        eax, ebp
004f7440 8945fc                   mov        dword ptr [ebp - 4], eax
004f7443 894de0                   mov        dword ptr [ebp - 0x20], ecx
004f7446 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7449 8b8854480100             mov        ecx, dword ptr [eax + 0x14854]
004f744f 51                       push       ecx
004f7450 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004f7456 e84502f8ff               call       0x4776a0
004f745b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f745e 8b8254480100             mov        eax, dword ptr [edx + 0x14854]
004f7464 50                       push       eax
004f7465 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004f746b e8401bfaff               call       0x498fb0
004f7470 8945dc                   mov        dword ptr [ebp - 0x24], eax
004f7473 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7476 e885a2f1ff               call       0x411700
004f747b 8945c0                   mov        dword ptr [ebp - 0x40], eax
004f747e 837dc006                 cmp        dword ptr [ebp - 0x40], 6
004f7482 0f870b0c0000             ja         0x4f8093
004f7488 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004f748b ff248dfc834f00           jmp        dword ptr [ecx*4 + 0x4f83fc]
004f7492 6a00                     push       0
004f7494 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7497 81c144060000             add        ecx, 0x644
004f749d e87ec0f2ff               call       0x423520
004f74a2 90                       nop        
004f74a3 51                       push       ecx
004f74a4 0f57c0                   xorps      xmm0, xmm0
004f74a7 f30f110424               movss      dword ptr [esp], xmm0
004f74ac 51                       push       ecx
004f74ad f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
004f74b5 f30f110424               movss      dword ptr [esp], xmm0
004f74ba 51                       push       ecx
004f74bb 0f57c0                   xorps      xmm0, xmm0
004f74be f30f110424               movss      dword ptr [esp], xmm0
004f74c3 8d8da0feffff             lea        ecx, [ebp - 0x160]
004f74c9 e802b9f2ff               call       0x422dd0
004f74ce 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f74d1 81c214060000             add        edx, 0x614
004f74d7 8b85a0feffff             mov        eax, dword ptr [ebp - 0x160]
004f74dd 8902                     mov        dword ptr [edx], eax
004f74df 8b8da4feffff             mov        ecx, dword ptr [ebp - 0x15c]
004f74e5 894a04                   mov        dword ptr [edx + 4], ecx
004f74e8 8b85a8feffff             mov        eax, dword ptr [ebp - 0x158]
004f74ee 894208                   mov        dword ptr [edx + 8], eax
004f74f1 68e0010000               push       0x1e0
004f74f6 6a00                     push       0
004f74f8 8d8dacfeffff             lea        ecx, [ebp - 0x154]
004f74fe e8fd68f1ff               call       0x40de00
004f7503 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7506 8b95acfeffff             mov        edx, dword ptr [ebp - 0x154]
004f750c 8b85b0feffff             mov        eax, dword ptr [ebp - 0x150]
004f7512 899120060000             mov        dword ptr [ecx + 0x620], edx
004f7518 898124060000             mov        dword ptr [ecx + 0x624], eax
004f751e e9700b0000               jmp        0x4f8093
004f7523 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7526 81c144060000             add        ecx, 0x644
004f752c 898db8feffff             mov        dword ptr [ebp - 0x148], ecx
004f7532 6a01                     push       1
004f7534 8b8db8feffff             mov        ecx, dword ptr [ebp - 0x148]
004f753a e851c0f2ff               call       0x423590
004f753f 0fb6d0                   movzx      edx, al
004f7542 85d2                     test       edx, edx
004f7544 7432                     je         0x4f7578
004f7546 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7549 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004f754c 898db4feffff             mov        dword ptr [ebp - 0x14c], ecx
004f7552 ba04000000               mov        edx, 4
004f7557 6bc200                   imul       eax, edx, 0
004f755a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f755d 8b9401a0220000           mov        edx, dword ptr [ecx + eax + 0x22a0]
004f7564 52                       push       edx
004f7565 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7568 83c028                   add        eax, 0x28
004f756b 50                       push       eax
004f756c 8b8db4feffff             mov        ecx, dword ptr [ebp - 0x14c]
004f7572 e8090ef4ff               call       0x438380
004f7577 90                       nop        
004f7578 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f757b 81c144060000             add        ecx, 0x644
004f7581 e80a8af1ff               call       0x40ff90
004f7586 69c000d8ffff             imul       eax, eax, 0xffffd800
004f758c 99                       cdq        
004f758d b93c000000               mov        ecx, 0x3c
004f7592 f7f9                     idiv       ecx
004f7594 0500f00000               add        eax, 0xf000
004f7599 8945ac                   mov        dword ptr [ebp - 0x54], eax
004f759c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f759f e8ec800000               call       0x4ff690
004f75a4 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
004f75a7 895004                   mov        dword ptr [eax + 4], edx
004f75aa 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f75ad e8de800000               call       0x4ff690
004f75b2 f30f2a4004               cvtsi2ss   xmm0, dword ptr [eax + 4]
004f75b7 f30f5e0598cd5600         divss      xmm0, dword ptr [0x56cd98]
004f75bf f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
004f75c4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f75c7 e88492f6ff               call       0x460850
004f75cc f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
004f75d1 f30f114004               movss      dword ptr [eax + 4], xmm0
004f75d6 6a01                     push       1
004f75d8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f75db e8b03c0000               call       0x4fb290
004f75e0 90                       nop        
004f75e1 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f75e4 05fc200000               add        eax, 0x20fc
004f75e9 8945bc                   mov        dword ptr [ebp - 0x44], eax
004f75ec 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004f75ef e8ac47f1ff               call       0x40bda0
004f75f4 8945d0                   mov        dword ptr [ebp - 0x30], eax
004f75f7 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004f75fa e841580000               call       0x4fce40
004f75ff 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004f7602 eb09                     jmp        0x4f760d
004f7604 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004f7607 83c108                   add        ecx, 8
004f760a 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004f760d 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004f7610 3b55a4                   cmp        edx, dword ptr [ebp - 0x5c]
004f7613 741d                     je         0x4f7632
004f7615 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004f7618 8945a0                   mov        dword ptr [ebp - 0x60], eax
004f761b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f761e e86d800000               call       0x4ff690
004f7623 8b08                     mov        ecx, dword ptr [eax]
004f7625 8b5004                   mov        edx, dword ptr [eax + 4]
004f7628 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
004f762b 8908                     mov        dword ptr [eax], ecx
004f762d 895004                   mov        dword ptr [eax + 4], edx
004f7630 ebd2                     jmp        0x4f7604
004f7632 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7635 81c144060000             add        ecx, 0x644
004f763b 894d9c                   mov        dword ptr [ebp - 0x64], ecx
004f763e 6a1e                     push       0x1e
004f7640 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004f7643 e878bff2ff               call       0x4235c0
004f7648 0fb6d0                   movzx      edx, al
004f764b 85d2                     test       edx, edx
004f764d 0f84dc000000             je         0x4f772f
004f7653 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7656 8b8858480100             mov        ecx, dword ptr [eax + 0x14858]
004f765c e89f4cf1ff               call       0x40c300
004f7661 894594                   mov        dword ptr [ebp - 0x6c], eax
004f7664 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7667 e8e491f6ff               call       0x460850
004f766c 894598                   mov        dword ptr [ebp - 0x68], eax
004f766f 6a00                     push       0
004f7671 689f860100               push       0x1869f
004f7676 6a00                     push       0
004f7678 51                       push       ecx
004f7679 f30f1005accd5600         movss      xmm0, dword ptr [0x56cdac]
004f7681 f30f110424               movss      dword ptr [esp], xmm0
004f7686 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004f7689 51                       push       ecx
004f768a 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004f768d e8ce58f8ff               call       0x47cf60
004f7692 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7695 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f769b e8f053f2ff               call       0x41ca90
004f76a0 89458c                   mov        dword ptr [ebp - 0x74], eax
004f76a3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f76a6 e8a591f6ff               call       0x460850
004f76ab 894590                   mov        dword ptr [ebp - 0x70], eax
004f76ae 6a00                     push       0
004f76b0 6a00                     push       0
004f76b2 51                       push       ecx
004f76b3 f30f1005accd5600         movss      xmm0, dword ptr [0x56cdac]
004f76bb f30f110424               movss      dword ptr [esp], xmm0
004f76c0 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004f76c3 50                       push       eax
004f76c4 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004f76c7 e80434fdff               call       0x4caad0
004f76cc 90                       nop        
004f76cd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f76d0 8b8958480100             mov        ecx, dword ptr [ecx + 0x14858]
004f76d6 e835b0f1ff               call       0x412710
004f76db 894584                   mov        dword ptr [ebp - 0x7c], eax
004f76de 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f76e1 e86a91f6ff               call       0x460850
004f76e6 894588                   mov        dword ptr [ebp - 0x78], eax
004f76e9 6a0a                     push       0xa
004f76eb 6a01                     push       1
004f76ed 51                       push       ecx
004f76ee 0f57c0                   xorps      xmm0, xmm0
004f76f1 f30f110424               movss      dword ptr [esp], xmm0
004f76f6 51                       push       ecx
004f76f7 f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
004f76ff f30f110424               movss      dword ptr [esp], xmm0
004f7704 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004f7707 52                       push       edx
004f7708 8d857cfeffff             lea        eax, [ebp - 0x184]
004f770e 50                       push       eax
004f770f 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004f7712 e869a4fcff               call       0x4c1b80
004f7717 894580                   mov        dword ptr [ebp - 0x80], eax
004f771a 8d8d78feffff             lea        ecx, [ebp - 0x188]
004f7720 51                       push       ecx
004f7721 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004f7724 e8e7a4fcff               call       0x4c1c10
004f7729 90                       nop        
004f772a e9a7000000               jmp        0x4f77d6
004f772f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7732 81c144060000             add        ecx, 0x644
004f7738 e85388f1ff               call       0x40ff90
004f773d f30f2ac0                 cvtsi2ss   xmm0, eax
004f7741 f30f590514f15600         mulss      xmm0, dword ptr [0x56f114]
004f7749 f30f5e0580f25600         divss      xmm0, dword ptr [0x56f280]
004f7751 f30f580528fa5600         addss      xmm0, dword ptr [0x56fa28]
004f7759 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004f775e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7761 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f7767 e82453f2ff               call       0x41ca90
004f776c 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004f7772 6a01                     push       1
004f7774 6a00                     push       0
004f7776 51                       push       ecx
004f7777 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004f777c f30f110424               movss      dword ptr [esp], xmm0
004f7781 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7784 0538060000               add        eax, 0x638
004f7789 50                       push       eax
004f778a 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004f7790 e83b33fdff               call       0x4caad0
004f7795 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7798 8b8958480100             mov        ecx, dword ptr [ecx + 0x14858]
004f779e e8ed52f2ff               call       0x41ca90
004f77a3 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004f77a9 6a00                     push       0
004f77ab 6a00                     push       0
004f77ad f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004f77b2 f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
004f77ba 51                       push       ecx
004f77bb f30f110424               movss      dword ptr [esp], xmm0
004f77c0 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f77c3 81c238060000             add        edx, 0x638
004f77c9 52                       push       edx
004f77ca 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004f77d0 e8fb32fdff               call       0x4caad0
004f77d5 90                       nop        
004f77d6 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f77d9 0544060000               add        eax, 0x644
004f77de 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004f77e4 6a3c                     push       0x3c
004f77e6 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004f77ec e8cfbdf2ff               call       0x4235c0
004f77f1 0fb6c8                   movzx      ecx, al
004f77f4 85c9                     test       ecx, ecx
004f77f6 7427                     je         0x4f781f
004f77f8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f77fb e8f0800000               call       0x4ff8f0
004f7800 6a01                     push       1
004f7802 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7805 e896b5f1ff               call       0x412da0
004f780a 6a00                     push       0
004f780c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f780f 81c144060000             add        ecx, 0x644
004f7815 e806bdf2ff               call       0x423520
004f781a 90                       nop        
004f781b eb07                     jmp        0x4f7824
004f781d eb05                     jmp        0x4f7824
004f781f e96f080000               jmp        0x4f8093
004f7824 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7827 81c244060000             add        edx, 0x644
004f782d 899570ffffff             mov        dword ptr [ebp - 0x90], edx
004f7833 6a01                     push       1
004f7835 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004f783b e850bdf2ff               call       0x423590
004f7840 0fb6c0                   movzx      eax, al
004f7843 85c0                     test       eax, eax
004f7845 7432                     je         0x4f7879
004f7847 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f784a 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004f784d 89956cffffff             mov        dword ptr [ebp - 0x94], edx
004f7853 b804000000               mov        eax, 4
004f7858 6bc800                   imul       ecx, eax, 0
004f785b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f785e 8b840aa0220000           mov        eax, dword ptr [edx + ecx + 0x22a0]
004f7865 50                       push       eax
004f7866 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7869 83c128                   add        ecx, 0x28
004f786c 51                       push       ecx
004f786d 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004f7873 e8080bf4ff               call       0x438380
004f7878 90                       nop        
004f7879 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004f787f e85c3afbff               call       0x4ab2e0
004f7884 85c0                     test       eax, eax
004f7886 0f8586000000             jne        0x4f7912
004f788c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f788f 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f7895 e876c7f2ff               call       0x424010
004f789a 85c0                     test       eax, eax
004f789c 7474                     je         0x4f7912
004f789e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f78a1 8b8858480100             mov        ecx, dword ptr [eax + 0x14858]
004f78a7 e864c7f2ff               call       0x424010
004f78ac 8bc8                     mov        ecx, eax
004f78ae e8ed07f8ff               call       0x4780a0
004f78b3 0fb6c8                   movzx      ecx, al
004f78b6 85c9                     test       ecx, ecx
004f78b8 7458                     je         0x4f7912
004f78ba 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
004f78be 7c2f                     jl         0x4f78ef
004f78c0 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004f78c3 833c959c885b0000         cmp        dword ptr [edx*4 + 0x5b889c], 0
004f78cb 7422                     je         0x4f78ef
004f78cd 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004f78d0 8b0c859c885b00           mov        ecx, dword ptr [eax*4 + 0x5b889c]
004f78d7 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
004f78dd 6a04                     push       4
004f78df 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004f78e5 e8f6dffbff               call       0x4b58e0
004f78ea 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004f78ed eb07                     jmp        0x4f78f6
004f78ef c745b400000000           mov        dword ptr [ebp - 0x4c], 0
004f78f6 837db400                 cmp        dword ptr [ebp - 0x4c], 0
004f78fa 7416                     je         0x4f7912
004f78fc 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f78ff 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f7905 e806c7f2ff               call       0x424010
004f790a 8bc8                     mov        ecx, eax
004f790c e81f05f8ff               call       0x477e30
004f7911 90                       nop        
004f7912 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7915 e816230000               call       0x4f9c30
004f791a 90                       nop        
004f791b e973070000               jmp        0x4f8093
004f7920 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7923 0544060000               add        eax, 0x644
004f7928 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
004f792e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7931 8b9104220000             mov        edx, dword ptr [ecx + 0x2204]
004f7937 52                       push       edx
004f7938 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
004f793e e87dbcf2ff               call       0x4235c0
004f7943 0fb6c0                   movzx      eax, al
004f7946 85c0                     test       eax, eax
004f7948 7418                     je         0x4f7962
004f794a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f794d e8ce0a0000               call       0x4f8420
004f7952 90                       nop        
004f7953 e99d000000               jmp        0x4f79f5
004f7958 e998000000               jmp        0x4f79f5
004f795d e98e000000               jmp        0x4f79f0
004f7962 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7965 8b8958480100             mov        ecx, dword ptr [ecx + 0x14858]
004f796b e8a0c6f2ff               call       0x424010
004f7970 85c0                     test       eax, eax
004f7972 747c                     je         0x4f79f0
004f7974 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
004f7978 7c2f                     jl         0x4f79a9
004f797a 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004f797d 833c959c885b0000         cmp        dword ptr [edx*4 + 0x5b889c], 0
004f7985 7422                     je         0x4f79a9
004f7987 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004f798a 8b0c859c885b00           mov        ecx, dword ptr [eax*4 + 0x5b889c]
004f7991 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
004f7997 6a04                     push       4
004f7999 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
004f799f e83cdffbff               call       0x4b58e0
004f79a4 8945b0                   mov        dword ptr [ebp - 0x50], eax
004f79a7 eb07                     jmp        0x4f79b0
004f79a9 c745b000000000           mov        dword ptr [ebp - 0x50], 0
004f79b0 837db000                 cmp        dword ptr [ebp - 0x50], 0
004f79b4 743a                     je         0x4f79f0
004f79b6 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f79b9 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f79bf e84cc6f2ff               call       0x424010
004f79c4 8bc8                     mov        ecx, eax
004f79c6 e8d506f8ff               call       0x4780a0
004f79cb 0fb6c0                   movzx      eax, al
004f79ce 85c0                     test       eax, eax
004f79d0 741e                     je         0x4f79f0
004f79d2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f79d5 8b8958480100             mov        ecx, dword ptr [ecx + 0x14858]
004f79db e830c6f2ff               call       0x424010
004f79e0 8bc8                     mov        ecx, eax
004f79e2 e84904f8ff               call       0x477e30
004f79e7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f79ea e8d10c0000               call       0x4f86c0
004f79ef 90                       nop        
004f79f0 e99e060000               jmp        0x4f8093
004f79f5 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f79f8 81c244060000             add        edx, 0x644
004f79fe 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
004f7a04 6a03                     push       3
004f7a06 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
004f7a0c e8cf0bf8ff               call       0x4785e0
004f7a11 0fb6c0                   movzx      eax, al
004f7a14 85c0                     test       eax, eax
004f7a16 0f84c8020000             je         0x4f7ce4
004f7a1c 6a00                     push       0
004f7a1e e85dc6f6ff               call       0x464080
004f7a23 83c404                   add        esp, 4
004f7a26 8bc8                     mov        ecx, eax
004f7a28 e87307fcff               call       0x4b81a0
004f7a2d 83f804                   cmp        eax, 4
004f7a30 7c4a                     jl         0x4f7a7c
004f7a32 6a00                     push       0
004f7a34 e847c6f6ff               call       0x464080
004f7a39 83c404                   add        esp, 4
004f7a3c 898554ffffff             mov        dword ptr [ebp - 0xac], eax
004f7a42 6a00                     push       0
004f7a44 e837c6f6ff               call       0x464080
004f7a49 83c404                   add        esp, 4
004f7a4c 8bc8                     mov        ecx, eax
004f7a4e e87d07fcff               call       0x4b81d0
004f7a53 6bc050                   imul       eax, eax, 0x50
004f7a56 99                       cdq        
004f7a57 b964000000               mov        ecx, 0x64
004f7a5c f7f9                     idiv       ecx
004f7a5e 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
004f7a64 8b9558ffffff             mov        edx, dword ptr [ebp - 0xa8]
004f7a6a 52                       push       edx
004f7a6b 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
004f7a71 e80a9cfeff               call       0x4e1680
004f7a76 90                       nop        
004f7a77 e902010000               jmp        0x4f7b7e
004f7a7c 6a00                     push       0
004f7a7e e8fdc5f6ff               call       0x464080
004f7a83 83c404                   add        esp, 4
004f7a86 8bc8                     mov        ecx, eax
004f7a88 e81307fcff               call       0x4b81a0
004f7a8d 83f803                   cmp        eax, 3
004f7a90 7c4a                     jl         0x4f7adc
004f7a92 6a00                     push       0
004f7a94 e8e7c5f6ff               call       0x464080
004f7a99 83c404                   add        esp, 4
004f7a9c 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
004f7aa2 6a00                     push       0
004f7aa4 e8d7c5f6ff               call       0x464080
004f7aa9 83c404                   add        esp, 4
004f7aac 8bc8                     mov        ecx, eax
004f7aae e81d07fcff               call       0x4b81d0
004f7ab3 6bc03c                   imul       eax, eax, 0x3c
004f7ab6 99                       cdq        
004f7ab7 b964000000               mov        ecx, 0x64
004f7abc f7f9                     idiv       ecx
004f7abe 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
004f7ac4 8b9550ffffff             mov        edx, dword ptr [ebp - 0xb0]
004f7aca 52                       push       edx
004f7acb 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
004f7ad1 e8aa9bfeff               call       0x4e1680
004f7ad6 90                       nop        
004f7ad7 e9a2000000               jmp        0x4f7b7e
004f7adc 6a00                     push       0
004f7ade e89dc5f6ff               call       0x464080
004f7ae3 83c404                   add        esp, 4
004f7ae6 8bc8                     mov        ecx, eax
004f7ae8 e8b306fcff               call       0x4b81a0
004f7aed 83f802                   cmp        eax, 2
004f7af0 7c47                     jl         0x4f7b39
004f7af2 6a00                     push       0
004f7af4 e887c5f6ff               call       0x464080
004f7af9 83c404                   add        esp, 4
004f7afc 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
004f7b02 6a00                     push       0
004f7b04 e877c5f6ff               call       0x464080
004f7b09 83c404                   add        esp, 4
004f7b0c 8bc8                     mov        ecx, eax
004f7b0e e8bd06fcff               call       0x4b81d0
004f7b13 6bc032                   imul       eax, eax, 0x32
004f7b16 99                       cdq        
004f7b17 b964000000               mov        ecx, 0x64
004f7b1c f7f9                     idiv       ecx
004f7b1e 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
004f7b24 8b9548ffffff             mov        edx, dword ptr [ebp - 0xb8]
004f7b2a 52                       push       edx
004f7b2b 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
004f7b31 e84a9bfeff               call       0x4e1680
004f7b36 90                       nop        
004f7b37 eb45                     jmp        0x4f7b7e
004f7b39 6a00                     push       0
004f7b3b e840c5f6ff               call       0x464080
004f7b40 83c404                   add        esp, 4
004f7b43 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
004f7b49 6a00                     push       0
004f7b4b e830c5f6ff               call       0x464080
004f7b50 83c404                   add        esp, 4
004f7b53 8bc8                     mov        ecx, eax
004f7b55 e87606fcff               call       0x4b81d0
004f7b5a 6bc028                   imul       eax, eax, 0x28
004f7b5d 99                       cdq        
004f7b5e b964000000               mov        ecx, 0x64
004f7b63 f7f9                     idiv       ecx
004f7b65 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
004f7b6b 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
004f7b71 52                       push       edx
004f7b72 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
004f7b78 e8039bfeff               call       0x4e1680
004f7b7d 90                       nop        
004f7b7e 51                       push       ecx
004f7b7f 0f57c0                   xorps      xmm0, xmm0
004f7b82 f30f110424               movss      dword ptr [esp], xmm0
004f7b87 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7b8a e8c18cf6ff               call       0x460850
004f7b8f f30f104004               movss      xmm0, dword ptr [eax + 4]
004f7b94 f30f5c0514ed5600         subss      xmm0, dword ptr [0x56ed14]
004f7b9c 51                       push       ecx
004f7b9d f30f110424               movss      dword ptr [esp], xmm0
004f7ba2 51                       push       ecx
004f7ba3 0f57c0                   xorps      xmm0, xmm0
004f7ba6 f30f110424               movss      dword ptr [esp], xmm0
004f7bab 8d4de4                   lea        ecx, [ebp - 0x1c]
004f7bae e81db2f2ff               call       0x422dd0
004f7bb3 8d45e4                   lea        eax, [ebp - 0x1c]
004f7bb6 50                       push       eax
004f7bb7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7bba e831780000               call       0x4ff3f0
004f7bbf d99d38ffffff             fstp       dword ptr [ebp - 0xc8]
004f7bc5 c78584feffff01000000     mov        dword ptr [ebp - 0x17c], 1
004f7bcf c78588feffff01000000     mov        dword ptr [ebp - 0x178], 1
004f7bd9 c7858cfeffff01000000     mov        dword ptr [ebp - 0x174], 1
004f7be3 c78590feffff01000000     mov        dword ptr [ebp - 0x170], 1
004f7bed c78594feffff01000000     mov        dword ptr [ebp - 0x16c], 1
004f7bf7 c78598feffff01000000     mov        dword ptr [ebp - 0x168], 1
004f7c01 c7859cfeffff01000000     mov        dword ptr [ebp - 0x164], 1
004f7c0b c745d800000000           mov        dword ptr [ebp - 0x28], 0
004f7c12 eb09                     jmp        0x4f7c1d
004f7c14 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004f7c17 83c101                   add        ecx, 1
004f7c1a 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004f7c1d 837dd807                 cmp        dword ptr [ebp - 0x28], 7
004f7c21 0f8db2000000             jge        0x4f7cd9
004f7c27 f30f2a45d8               cvtsi2ss   xmm0, dword ptr [ebp - 0x28]
004f7c2c f30f5905f0e05600         mulss      xmm0, dword ptr [0x56e0f0]
004f7c34 f30f5e0548fd5600         divss      xmm0, dword ptr [0x56fd48]
004f7c3c f30f588538ffffff         addss      xmm0, dword ptr [ebp - 0xc8]
004f7c44 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
004f7c4c f30f590ddcc85600         mulss      xmm1, dword ptr [0x56c8dc]
004f7c54 f30f5e0d48fd5600         divss      xmm1, dword ptr [0x56fd48]
004f7c5c f30f5cc1                 subss      xmm0, xmm1
004f7c60 f30f118534ffffff         movss      dword ptr [ebp - 0xcc], xmm0
004f7c68 6a00                     push       0
004f7c6a e86113faff               call       0x498fd0
004f7c6f 83c404                   add        esp, 4
004f7c72 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
004f7c78 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7c7b e8d08bf6ff               call       0x460850
004f7c80 898530ffffff             mov        dword ptr [ebp - 0xd0], eax
004f7c86 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004f7c89 8b849584feffff           mov        eax, dword ptr [ebp + edx*4 - 0x17c]
004f7c90 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
004f7c96 6aff                     push       -1
004f7c98 6a00                     push       0
004f7c9a 6a00                     push       0
004f7c9c 51                       push       ecx
004f7c9d f30f1005d8c85600         movss      xmm0, dword ptr [0x56c8d8]
004f7ca5 f30f110424               movss      dword ptr [esp], xmm0
004f7caa 51                       push       ecx
004f7cab f30f108534ffffff         movss      xmm0, dword ptr [ebp - 0xcc]
004f7cb3 f30f110424               movss      dword ptr [esp], xmm0
004f7cb8 6aff                     push       -1
004f7cba 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
004f7cc0 51                       push       ecx
004f7cc1 8b952cffffff             mov        edx, dword ptr [ebp - 0xd4]
004f7cc7 52                       push       edx
004f7cc8 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
004f7cce e8bdbffcff               call       0x4c3c90
004f7cd3 90                       nop        
004f7cd4 e93bffffff               jmp        0x4f7c14
004f7cd9 6aff                     push       -1
004f7cdb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7cde e8bd2f0000               call       0x4faca0
004f7ce3 90                       nop        
004f7ce4 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7ce7 0544060000               add        eax, 0x644
004f7cec 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
004f7cf2 6a1e                     push       0x1e
004f7cf4 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
004f7cfa e8c1b8f2ff               call       0x4235c0
004f7cff 0fb6c8                   movzx      ecx, al
004f7d02 85c9                     test       ecx, ecx
004f7d04 0f8415020000             je         0x4f7f1f
004f7d0a 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7d0d 8b4214                   mov        eax, dword ptr [edx + 0x14]
004f7d10 c1e80a                   shr        eax, 0xa
004f7d13 83e001                   and        eax, 1
004f7d16 7445                     je         0x4f7d5d
004f7d18 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7d1b 8b8958480100             mov        ecx, dword ptr [ecx + 0x14858]
004f7d21 e8da45f1ff               call       0x40c300
004f7d26 89851cffffff             mov        dword ptr [ebp - 0xe4], eax
004f7d2c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7d2f e81c8bf6ff               call       0x460850
004f7d34 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
004f7d3a 6a00                     push       0
004f7d3c 51                       push       ecx
004f7d3d f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
004f7d45 f30f110424               movss      dword ptr [esp], xmm0
004f7d4a 8b9520ffffff             mov        edx, dword ptr [ebp - 0xe0]
004f7d50 52                       push       edx
004f7d51 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
004f7d57 e8a453f8ff               call       0x47d100
004f7d5c 90                       nop        
004f7d5d 6a00                     push       0
004f7d5f e81cc3f6ff               call       0x464080
004f7d64 83c404                   add        esp, 4
004f7d67 8bc8                     mov        ecx, eax
004f7d69 e84203fcff               call       0x4b80b0
004f7d6e 85c0                     test       eax, eax
004f7d70 7d5f                     jge        0x4f7dd1
004f7d72 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7d75 0544060000               add        eax, 0x644
004f7d7a 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
004f7d80 6a1e                     push       0x1e
004f7d82 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
004f7d88 e85308f8ff               call       0x4785e0
004f7d8d 0fb6c8                   movzx      ecx, al
004f7d90 85c9                     test       ecx, ecx
004f7d92 743d                     je         0x4f7dd1
004f7d94 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004f7d9a e8610af9ff               call       0x488800
004f7d9f 85c0                     test       eax, eax
004f7da1 750c                     jne        0x4f7daf
004f7da3 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004f7da9 e822defeff               call       0x4e5bd0
004f7dae 90                       nop        
004f7daf 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7db2 81c244060000             add        edx, 0x644
004f7db8 899514ffffff             mov        dword ptr [ebp - 0xec], edx
004f7dbe 6a00                     push       0
004f7dc0 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
004f7dc6 e875b7f2ff               call       0x423540
004f7dcb 90                       nop        
004f7dcc e9c2020000               jmp        0x4f8093
004f7dd1 6a00                     push       0
004f7dd3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7dd6 e8c5aff1ff               call       0x412da0
004f7ddb 51                       push       ecx
004f7ddc f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f7de4 f30f110424               movss      dword ptr [esp], xmm0
004f7de9 b9e4ef5a00               mov        ecx, 0x5aefe4
004f7dee e8ad14f3ff               call       0x4292a0
004f7df3 90                       nop        
004f7df4 6a00                     push       0
004f7df6 e8c510f8ff               call       0x478ec0
004f7dfb 83c404                   add        esp, 4
004f7dfe 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
004f7e04 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7e07 e8448af6ff               call       0x460850
004f7e0c 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
004f7e12 6896000000               push       0x96
004f7e17 6a1e                     push       0x1e
004f7e19 51                       push       ecx
004f7e1a f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004f7e22 f30f110424               movss      dword ptr [esp], xmm0
004f7e27 51                       push       ecx
004f7e28 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004f7e30 f30f110424               movss      dword ptr [esp], xmm0
004f7e35 8b8510ffffff             mov        eax, dword ptr [ebp - 0xf0]
004f7e3b 50                       push       eax
004f7e3c 8d8d80feffff             lea        ecx, [ebp - 0x180]
004f7e42 51                       push       ecx
004f7e43 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
004f7e49 e8329dfcff               call       0x4c1b80
004f7e4e 90                       nop        
004f7e4f c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004f7e56 6a00                     push       0
004f7e58 e823c2f6ff               call       0x464080
004f7e5d 83c404                   add        esp, 4
004f7e60 8bc8                     mov        ecx, eax
004f7e62 e88901f8ff               call       0x477ff0
004f7e67 83f802                   cmp        eax, 2
004f7e6a 7d19                     jge        0x4f7e85
004f7e6c 6a00                     push       0
004f7e6e e80dc2f6ff               call       0x464080
004f7e73 83c404                   add        esp, 4
004f7e76 8bc8                     mov        ecx, eax
004f7e78 e89355fcff               call       0x4bd410
004f7e7d 0345d4                   add        eax, dword ptr [ebp - 0x2c]
004f7e80 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004f7e83 eb17                     jmp        0x4f7e9c
004f7e85 6a00                     push       0
004f7e87 e8f4c1f6ff               call       0x464080
004f7e8c 83c404                   add        esp, 4
004f7e8f 8bc8                     mov        ecx, eax
004f7e91 e85a01f8ff               call       0x477ff0
004f7e96 0345d4                   add        eax, dword ptr [ebp - 0x2c]
004f7e99 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004f7e9c 6a00                     push       0
004f7e9e e8ddc1f6ff               call       0x464080
004f7ea3 83c404                   add        esp, 4
004f7ea6 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
004f7eac 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004f7eaf 52                       push       edx
004f7eb0 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
004f7eb6 e82597feff               call       0x4e15e0
004f7ebb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7ebe e88d89f6ff               call       0x460850
004f7ec3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7ec6 81c138060000             add        ecx, 0x638
004f7ecc 8b10                     mov        edx, dword ptr [eax]
004f7ece 8911                     mov        dword ptr [ecx], edx
004f7ed0 8b5004                   mov        edx, dword ptr [eax + 4]
004f7ed3 895104                   mov        dword ptr [ecx + 4], edx
004f7ed6 8b4008                   mov        eax, dword ptr [eax + 8]
004f7ed9 894108                   mov        dword ptr [ecx + 8], eax
004f7edc 51                       push       ecx
004f7edd f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
004f7ee5 f30f110424               movss      dword ptr [esp], xmm0
004f7eea 51                       push       ecx
004f7eeb 0f57c0                   xorps      xmm0, xmm0
004f7eee f30f110424               movss      dword ptr [esp], xmm0
004f7ef3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7ef6 e8857e0000               call       0x4ffd80
004f7efb 6818010000               push       0x118
004f7f00 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7f03 81c150200000             add        ecx, 0x2050
004f7f09 e812b6f2ff               call       0x423520
004f7f0e 6a00                     push       0
004f7f10 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7f13 81c144060000             add        ecx, 0x644
004f7f19 e802b6f2ff               call       0x423520
004f7f1e 90                       nop        
004f7f1f e96f010000               jmp        0x4f8093
004f7f24 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7f27 81c144060000             add        ecx, 0x644
004f7f2d 898d04ffffff             mov        dword ptr [ebp - 0xfc], ecx
004f7f33 6a04                     push       4
004f7f35 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
004f7f3b e8a006f8ff               call       0x4785e0
004f7f40 0fb6d0                   movzx      edx, al
004f7f43 85d2                     test       edx, edx
004f7f45 7402                     je         0x4f7f49
004f7f47 eb63                     jmp        0x4f7fac
004f7f49 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7f4c 0544060000               add        eax, 0x644
004f7f51 898500ffffff             mov        dword ptr [ebp - 0x100], eax
004f7f57 6a0f                     push       0xf
004f7f59 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
004f7f5f e87c06f8ff               call       0x4785e0
004f7f64 0fb6c8                   movzx      ecx, al
004f7f67 85c9                     test       ecx, ecx
004f7f69 7426                     je         0x4f7f91
004f7f6b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7f6e 8b8a58480100             mov        ecx, dword ptr [edx + 0x14858]
004f7f74 e8174bf2ff               call       0x41ca90
004f7f79 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
004f7f7f 6a00                     push       0
004f7f81 6a01                     push       1
004f7f83 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
004f7f89 e80215fdff               call       0x4c9490
004f7f8e 90                       nop        
004f7f8f eb1b                     jmp        0x4f7fac
004f7f91 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f7f94 0544060000               add        eax, 0x644
004f7f99 8985f8feffff             mov        dword ptr [ebp - 0x108], eax
004f7f9f 6a1e                     push       0x1e
004f7fa1 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
004f7fa7 e83406f8ff               call       0x4785e0
004f7fac e9e2000000               jmp        0x4f8093
004f7fb1 8d4df0                   lea        ecx, [ebp - 0x10]
004f7fb4 e857aef2ff               call       0x422e10
004f7fb9 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f7fbc 83c218                   add        edx, 0x18
004f7fbf 8995f4feffff             mov        dword ptr [ebp - 0x10c], edx
004f7fc5 51                       push       ecx
004f7fc6 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f7fce f30f110424               movss      dword ptr [esp], xmm0
004f7fd3 6a00                     push       0
004f7fd5 8d45f0                   lea        eax, [ebp - 0x10]
004f7fd8 50                       push       eax
004f7fd9 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
004f7fdf e82c770000               call       0x4ff710
004f7fe4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f7fe7 81c144060000             add        ecx, 0x644
004f7fed e89e7ff1ff               call       0x40ff90
004f7ff2 f30f2ac0                 cvtsi2ss   xmm0, eax
004f7ff6 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
004f7ffe f30f5e0560055700         divss      xmm0, dword ptr [0x570560]
004f8006 f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
004f800e f30f5cc8                 subss      xmm1, xmm0
004f8012 51                       push       ecx
004f8013 f30f110c24               movss      dword ptr [esp], xmm1
004f8018 8d4df0                   lea        ecx, [ebp - 0x10]
004f801b 51                       push       ecx
004f801c 8d55f0                   lea        edx, [ebp - 0x10]
004f801f 52                       push       edx
004f8020 e89bb2f5ff               call       0x4532c0
004f8025 83c40c                   add        esp, 0xc
004f8028 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004f802d f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
004f8035 f30f2cc0                 cvttss2si  eax, xmm0
004f8039 50                       push       eax
004f803a f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004f803f f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
004f8047 f30f2cc8                 cvttss2si  ecx, xmm0
004f804b 51                       push       ecx
004f804c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f804f e8bc790000               call       0x4ffa10
004f8054 90                       nop        
004f8055 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f8058 81c244060000             add        edx, 0x644
004f805e 8995f0feffff             mov        dword ptr [ebp - 0x110], edx
004f8064 6a28                     push       0x28
004f8066 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
004f806c e84fb5f2ff               call       0x4235c0
004f8071 0fb6c0                   movzx      eax, al
004f8074 85c0                     test       eax, eax
004f8076 741b                     je         0x4f8093
004f8078 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f807b c7411001000000           mov        dword ptr [ecx + 0x10], 1
004f8082 6a00                     push       0
004f8084 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8087 81c144060000             add        ecx, 0x644
004f808d e88eb4f2ff               call       0x423520
004f8092 90                       nop        
004f8093 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f8096 81c260200000             add        edx, 0x2060
004f809c 8995ecfeffff             mov        dword ptr [ebp - 0x114], edx
004f80a2 6a00                     push       0
004f80a4 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
004f80aa e8c18ff6ff               call       0x461070
004f80af 0fb6c0                   movzx      eax, al
004f80b2 85c0                     test       eax, eax
004f80b4 741d                     je         0x4f80d3
004f80b6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f80b9 81c160200000             add        ecx, 0x2060
004f80bf 898de8feffff             mov        dword ptr [ebp - 0x118], ecx
004f80c5 6a00                     push       0
004f80c7 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
004f80cd e84e13f3ff               call       0x429420
004f80d2 90                       nop        
004f80d3 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f80d6 81c250200000             add        edx, 0x2050
004f80dc 8995e4feffff             mov        dword ptr [ebp - 0x11c], edx
004f80e2 6a00                     push       0
004f80e4 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
004f80ea e8818ff6ff               call       0x461070
004f80ef 0fb6c0                   movzx      eax, al
004f80f2 85c0                     test       eax, eax
004f80f4 0f8485000000             je         0x4f817f
004f80fa 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f80fd 81c150200000             add        ecx, 0x2050
004f8103 898de0feffff             mov        dword ptr [ebp - 0x120], ecx
004f8109 6a00                     push       0
004f810b 8b8de0feffff             mov        ecx, dword ptr [ebp - 0x120]
004f8111 e80a13f3ff               call       0x429420
004f8116 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f8119 83c228                   add        edx, 0x28
004f811c 8955cc                   mov        dword ptr [ebp - 0x34], edx
004f811f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f8122 0544060000               add        eax, 0x644
004f8127 8985dcfeffff             mov        dword ptr [ebp - 0x124], eax
004f812d 6a03                     push       3
004f812f 8b8ddcfeffff             mov        ecx, dword ptr [ebp - 0x124]
004f8135 e8c60cf8ff               call       0x478e00
004f813a 85c0                     test       eax, eax
004f813c 7427                     je         0x4f8165
004f813e 68ff0000ff               push       0xff0000ff
004f8143 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004f8146 e8c534fbff               call       0x4ab610
004f814b 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004f814e 8a91a1040000             mov        dl, byte ptr [ecx + 0x4a1]
004f8154 80e2e3                   and        dl, 0xe3
004f8157 80ca04                   or         dl, 4
004f815a 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004f815d 8890a1040000             mov        byte ptr [eax + 0x4a1], dl
004f8163 eb15                     jmp        0x4f817a
004f8165 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8168 8a91c9040000             mov        dl, byte ptr [ecx + 0x4c9]
004f816e 80e2e3                   and        dl, 0xe3
004f8171 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f8174 8890c9040000             mov        byte ptr [eax + 0x4c9], dl
004f817a e9de000000               jmp        0x4f825d
004f817f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8182 8a91c9040000             mov        dl, byte ptr [ecx + 0x4c9]
004f8188 80e2e3                   and        dl, 0xe3
004f818b 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f818e 8890c9040000             mov        byte ptr [eax + 0x4c9], dl
004f8194 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8197 e884760000               call       0x4ff820
004f819c 85c0                     test       eax, eax
004f819e 7451                     je         0x4f81f1
004f81a0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f81a3 83c128                   add        ecx, 0x28
004f81a6 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004f81a9 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f81ac 81c244060000             add        edx, 0x644
004f81b2 8995d8feffff             mov        dword ptr [ebp - 0x128], edx
004f81b8 6a08                     push       8
004f81ba 8b8dd8feffff             mov        ecx, dword ptr [ebp - 0x128]
004f81c0 e87b9ef7ff               call       0x472040
004f81c5 83f804                   cmp        eax, 4
004f81c8 7d25                     jge        0x4f81ef
004f81ca 680000ffff               push       0xffff0000
004f81cf 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004f81d2 e83934fbff               call       0x4ab610
004f81d7 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004f81da 8a88a1040000             mov        cl, byte ptr [eax + 0x4a1]
004f81e0 80e1e3                   and        cl, 0xe3
004f81e3 80c904                   or         cl, 4
004f81e6 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004f81e9 888aa1040000             mov        byte ptr [edx + 0x4a1], cl
004f81ef eb6c                     jmp        0x4f825d
004f81f1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f81f4 e817740000               call       0x4ff610
004f81f9 d99dd4feffff             fstp       dword ptr [ebp - 0x12c]
004f81ff f30f1085d4feffff         movss      xmm0, dword ptr [ebp - 0x12c]
004f8207 0f2f0530e75600           comiss     xmm0, dword ptr [0x56e730]
004f820e 764d                     jbe        0x4f825d
004f8210 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f8213 83c028                   add        eax, 0x28
004f8216 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004f8219 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f821c 81c144060000             add        ecx, 0x644
004f8222 898dd0feffff             mov        dword ptr [ebp - 0x130], ecx
004f8228 6a08                     push       8
004f822a 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
004f8230 e80b9ef7ff               call       0x472040
004f8235 83f804                   cmp        eax, 4
004f8238 7d23                     jge        0x4f825d
004f823a 6800ffffff               push       0xffffff00
004f823f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004f8242 e8c933fbff               call       0x4ab610
004f8247 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004f824a 8a82a1040000             mov        al, byte ptr [edx + 0x4a1]
004f8250 24e3                     and        al, 0xe3
004f8252 0c04                     or         al, 4
004f8254 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004f8257 8881a1040000             mov        byte ptr [ecx + 0x4a1], al
004f825d 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f8260 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f8268 f30f1182ec200000         movss      dword ptr [edx + 0x20ec], xmm0
004f8270 6864265700               push       0x572664
004f8275 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8278 e8637c0000               call       0x4ffee0
004f827d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8280 83c128                   add        ecx, 0x28
004f8283 e84833f3ff               call       0x42b5d0
004f8288 90                       nop        
004f8289 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f828c e86f750000               call       0x4ff800
004f8291 85c0                     test       eax, eax
004f8293 0f84c7000000             je         0x4f8360
004f8299 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f829c 81c10c220000             add        ecx, 0x220c
004f82a2 e84948f2ff               call       0x41caf0
004f82a7 85c0                     test       eax, eax
004f82a9 7417                     je         0x4f82c2
004f82ab 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f82ae 81c10c220000             add        ecx, 0x220c
004f82b4 e8571ef3ff               call       0x42a110
004f82b9 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f82bc d99838220000             fstp       dword ptr [eax + 0x2238]
004f82c2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f82c5 81c10c220000             add        ecx, 0x220c
004f82cb e82048f2ff               call       0x41caf0
004f82d0 85c0                     test       eax, eax
004f82d2 741a                     je         0x4f82ee
004f82d4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f82d7 81c144060000             add        ecx, 0x644
004f82dd e8ae7cf1ff               call       0x40ff90
004f82e2 99                       cdq        
004f82e3 b903000000               mov        ecx, 3
004f82e8 f7f9                     idiv       ecx
004f82ea 85d2                     test       edx, edx
004f82ec 743c                     je         0x4f832a
004f82ee 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f82f1 83c228                   add        edx, 0x28
004f82f4 8995ccfeffff             mov        dword ptr [ebp - 0x134], edx
004f82fa 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f82fd 51                       push       ecx
004f82fe f30f108038220000         movss      xmm0, dword ptr [eax + 0x2238]
004f8306 f30f110424               movss      dword ptr [esp], xmm0
004f830b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f830e 51                       push       ecx
004f830f f30f108138220000         movss      xmm0, dword ptr [ecx + 0x2238]
004f8317 f30f110424               movss      dword ptr [esp], xmm0
004f831c 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
004f8322 e8298bf7ff               call       0x470e50
004f8327 90                       nop        
004f8328 eb34                     jmp        0x4f835e
004f832a 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f832d 83c228                   add        edx, 0x28
004f8330 8995c8feffff             mov        dword ptr [ebp - 0x138], edx
004f8336 51                       push       ecx
004f8337 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f833f f30f110424               movss      dword ptr [esp], xmm0
004f8344 51                       push       ecx
004f8345 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f834d f30f110424               movss      dword ptr [esp], xmm0
004f8352 8b8dc8feffff             mov        ecx, dword ptr [ebp - 0x138]
004f8358 e8f38af7ff               call       0x470e50
004f835d 90                       nop        
004f835e eb34                     jmp        0x4f8394
004f8360 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004f8363 83c028                   add        eax, 0x28
004f8366 8985c4feffff             mov        dword ptr [ebp - 0x13c], eax
004f836c 51                       push       ecx
004f836d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f8375 f30f110424               movss      dword ptr [esp], xmm0
004f837a 51                       push       ecx
004f837b f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004f8383 f30f110424               movss      dword ptr [esp], xmm0
004f8388 8b8dc4feffff             mov        ecx, dword ptr [ebp - 0x13c]
004f838e e8bd8af7ff               call       0x470e50
004f8393 90                       nop        
004f8394 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f8397 81c144060000             add        ecx, 0x644
004f839d 898dc0feffff             mov        dword ptr [ebp - 0x140], ecx
004f83a3 6a00                     push       0
004f83a5 8b8dc0feffff             mov        ecx, dword ptr [ebp - 0x140]
004f83ab e890b1f2ff               call       0x423540
004f83b0 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004f83b3 81c254060000             add        edx, 0x654
004f83b9 8995bcfeffff             mov        dword ptr [ebp - 0x144], edx
004f83bf 6a00                     push       0
004f83c1 8b8dbcfeffff             mov        ecx, dword ptr [ebp - 0x144]
004f83c7 e874b1f2ff               call       0x423540
004f83cc 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f83cf 81c1b4220000             add        ecx, 0x22b4
004f83d5 e836be0000               call       0x504210
004f83da 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004f83dd 81c144220000             add        ecx, 0x2244
004f83e3 e828eeffff               call       0x4f7210
004f83e8 b801000000               mov        eax, 1
004f83ed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004f83f0 33cd                     xor        ecx, ebp
004f83f2 e8f7a50400               call       0x5429ee
004f83f7 8be5                     mov        esp, ebp
004f83f9 5d                       pop        ebp
004f83fa c3                       ret        
004f83fb 90                       nop        
004f83fc 23754f                   and        esi, dword ptr [ebp + 0x4f]
004f83ff 002478                   add        byte ptr [eax + edi*2], ah
004f8402 4f                       dec        edi
004f8403 00f5                     add        ch, dh
004f8405 794f                     jns        0x4f8456
004f8407 00247f                   add        byte ptr [edi + edi*2], ah
004f840a 4f                       dec        edi
004f840b 0020                     add        byte ptr [eax], ah
004f840d 794f                     jns        0x4f845e
004f840f 00b17f4f0092             add        byte ptr [ecx - 0x6dffb081], dh
004f8415 744f                     je         0x4f8466
004f8417 00cc                     add        ah, cl
004f8419 cc                       int3       
004f841a cc                       int3       
004f841b cc                       int3       
004f841c cc                       int3       
004f841d cc                       int3       
004f841e cc                       int3       
004f841f cc                       int3       
