00509280 55                       push       ebp
00509281 8bec                     mov        ebp, esp
00509283 6aff                     push       -1
00509285 68e5a75600               push       0x56a7e5
0050928a 64a100000000             mov        eax, dword ptr fs:[0]
00509290 50                       push       eax
00509291 81ecf8000000             sub        esp, 0xf8
00509297 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0050929c 33c5                     xor        eax, ebp
0050929e 8945f0                   mov        dword ptr [ebp - 0x10], eax
005092a1 50                       push       eax
005092a2 8d45f4                   lea        eax, [ebp - 0xc]
005092a5 64a300000000             mov        dword ptr fs:[0], eax
005092ab 894d90                   mov        dword ptr [ebp - 0x70], ecx
005092ae c7856cffffff00000000     mov        dword ptr [ebp - 0x94], 0
005092b8 c78558ffffff00000000     mov        dword ptr [ebp - 0xa8], 0
005092c2 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005092c5 50                       push       eax
005092c6 6a09                     push       9
005092c8 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005092cb 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
005092ce 52                       push       edx
005092cf e83cf30300               call       0x548610
005092d4 83c40c                   add        esp, 0xc
005092d7 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005092da 50                       push       eax
005092db e8406b0400               call       0x54fe20
005092e0 83c404                   add        esp, 4
005092e3 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
005092e9 eb0f                     jmp        0x5092fa
005092eb 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
005092f1 83c101                   add        ecx, 1
005092f4 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
005092fa 83bd60ffffff08           cmp        dword ptr [ebp - 0xa0], 8
00509301 7d12                     jge        0x509315
00509303 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509306 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00509309 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
0050930f c6040820                 mov        byte ptr [eax + ecx], 0x20
00509313 ebd6                     jmp        0x5092eb
00509315 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509318 8b825c020000             mov        eax, dword ptr [edx + 0x25c]
0050931e 83e001                   and        eax, 1
00509321 7553                     jne        0x509376
00509323 837d1400                 cmp        dword ptr [ebp + 0x14], 0
00509327 744d                     je         0x509376
00509329 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0050932c 8b89e0000000             mov        ecx, dword ptr [ecx + 0xe0]
00509332 e8c92ff0ff               call       0x40c300
00509337 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
0050933d 68ffff0000               push       0xffff
00509342 68ffff0000               push       0xffff
00509347 68ffff0000               push       0xffff
0050934c 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
00509352 e8e90a0000               call       0x509e40
00509357 85c0                     test       eax, eax
00509359 741b                     je         0x509376
0050935b 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0050935e 8b8258020000             mov        eax, dword ptr [edx + 0x258]
00509364 50                       push       eax
00509365 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509368 e8730f0000               call       0x50a2e0
0050936d 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509370 8981e0000000             mov        dword ptr [ecx + 0xe0], eax
00509376 c7458c00010000           mov        dword ptr [ebp - 0x74], 0x100
0050937d c7855cffffff00000000     mov        dword ptr [ebp - 0xa4], 0
00509387 c7458400000000           mov        dword ptr [ebp - 0x7c], 0
0050938e eb09                     jmp        0x509399
00509390 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
00509393 83c201                   add        edx, 1
00509396 895584                   mov        dword ptr [ebp - 0x7c], edx
00509399 837d8408                 cmp        dword ptr [ebp - 0x7c], 8
0050939d 0f8db1010000             jge        0x509554
005093a3 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005093a6 83c020                   add        eax, 0x20
005093a9 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
005093af 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005093b2 51                       push       ecx
005093b3 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
005093b9 e8c2b1f0ff               call       0x414580
005093be 833800                   cmp        dword ptr [eax], 0
005093c1 7502                     jne        0x5093c5
005093c3 ebcb                     jmp        0x509390
005093c5 83bd6cffffff00           cmp        dword ptr [ebp - 0x94], 0
005093cc 7509                     jne        0x5093d7
005093ce 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
005093d1 89956cffffff             mov        dword ptr [ebp - 0x94], edx
005093d7 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
005093da 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
005093e0 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005093e3 8b915c020000             mov        edx, dword ptr [ecx + 0x25c]
005093e9 83e201                   and        edx, 1
005093ec 7524                     jne        0x509412
005093ee 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005093f1 83c020                   add        eax, 0x20
005093f4 898500ffffff             mov        dword ptr [ebp - 0x100], eax
005093fa 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005093fd 51                       push       ecx
005093fe 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
00509404 e877b1f0ff               call       0x414580
00509409 8b10                     mov        edx, dword ptr [eax]
0050940b c7420c00000000           mov        dword ptr [edx + 0xc], 0
00509412 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00509415 05a0020000               add        eax, 0x2a0
0050941a 89458c                   mov        dword ptr [ebp - 0x74], eax
0050941d 6b4d8414                 imul       ecx, dword ptr [ebp - 0x7c], 0x14
00509421 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509424 8d4c0a40                 lea        ecx, [edx + ecx + 0x40]
00509428 e8636bf0ff               call       0x40ff90
0050942d 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00509433 83bd68ffffff00           cmp        dword ptr [ebp - 0x98], 0
0050943a 0f8400010000             je         0x509540
00509440 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00509446 e8b52ef0ff               call       0x40c300
0050944b 8bc8                     mov        ecx, eax
0050944d e8be0c0000               call       0x50a110
00509452 03458c                   add        eax, dword ptr [ebp - 0x74]
00509455 89458c                   mov        dword ptr [ebp - 0x74], eax
00509458 8b4590                   mov        eax, dword ptr [ebp - 0x70]
0050945b 8b885c020000             mov        ecx, dword ptr [eax + 0x25c]
00509461 83e101                   and        ecx, 1
00509464 0f85c0000000             jne        0x50952a
0050946a 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00509470 e88b2ef0ff               call       0x40c300
00509475 8bc8                     mov        ecx, eax
00509477 e8940c0000               call       0x50a110
0050947c 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00509482 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509485 83c220                   add        edx, 0x20
00509488 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
0050948e 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
00509491 50                       push       eax
00509492 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00509498 e8e3b0f0ff               call       0x414580
0050949d 8b08                     mov        ecx, dword ptr [eax]
0050949f 83c10c                   add        ecx, 0xc
005094a2 898d54ffffff             mov        dword ptr [ebp - 0xac], ecx
005094a8 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
005094ae 8b02                     mov        eax, dword ptr [edx]
005094b0 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
005094b6 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
005094bc 038d44ffffff             add        ecx, dword ptr [ebp - 0xbc]
005094c2 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
005094c8 890a                     mov        dword ptr [edx], ecx
005094ca 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
005094d0 e82b2ef0ff               call       0x40c300
005094d5 8bc8                     mov        ecx, eax
005094d7 e8c40b0000               call       0x50a0a0
005094dc 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
005094e2 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005094e5 83c020                   add        eax, 0x20
005094e8 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
005094ee 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005094f1 51                       push       ecx
005094f2 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
005094f8 e883b0f0ff               call       0x414580
005094fd 8b10                     mov        edx, dword ptr [eax]
005094ff 83c208                   add        edx, 8
00509502 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
00509508 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
0050950e 8b08                     mov        ecx, dword ptr [eax]
00509510 898d3cffffff             mov        dword ptr [ebp - 0xc4], ecx
00509516 8b953cffffff             mov        edx, dword ptr [ebp - 0xc4]
0050951c 039538ffffff             add        edx, dword ptr [ebp - 0xc8]
00509522 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
00509528 8910                     mov        dword ptr [eax], edx
0050952a 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
00509530 e85b6af0ff               call       0x40ff90
00509535 898568ffffff             mov        dword ptr [ebp - 0x98], eax
0050953b e9f3feffff               jmp        0x509433
00509540 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
00509546 83c101                   add        ecx, 1
00509549 898d5cffffff             mov        dword ptr [ebp - 0xa4], ecx
0050954f e93cfeffff               jmp        0x509390
00509554 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509557 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0050955a 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
00509560 8988d4000000             mov        dword ptr [eax + 0xd4], ecx
00509566 b968a55b00               mov        ecx, 0x5ba568
0050956b e820edfaff               call       0x4b8290
00509570 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
00509576 89950cffffff             mov        dword ptr [ebp - 0xf4], edx
0050957c 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0050957f 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00509582 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
00509588 8b950cffffff             mov        edx, dword ptr [ebp - 0xf4]
0050958e 894818                   mov        dword ptr [eax + 0x18], ecx
00509591 89501c                   mov        dword ptr [eax + 0x1c], edx
00509594 8b0d004a5c00             mov        ecx, dword ptr [0x5c4a00]
0050959a e8410c0000               call       0x50a1e0
0050959f d99d34ffffff             fstp       dword ptr [ebp - 0xcc]
005095a5 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005095a8 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
005095ab f30f108534ffffff         movss      xmm0, dword ptr [ebp - 0xcc]
005095b3 f30f1181d0000000         movss      dword ptr [ecx + 0xd0], xmm0
005095bb 68ac345700               push       0x5734ac
005095c0 8b558c                   mov        edx, dword ptr [ebp - 0x74]
005095c3 52                       push       edx
005095c4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005095ca e84160f1ff               call       0x41f610
005095cf 898578ffffff             mov        dword ptr [ebp - 0x88], eax
005095d5 c7458c00000000           mov        dword ptr [ebp - 0x74], 0
005095dc 6800010000               push       0x100
005095e1 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005095e4 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
005095e7 51                       push       ecx
005095e8 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005095ee 03558c                   add        edx, dword ptr [ebp - 0x74]
005095f1 52                       push       edx
005095f2 e829a80300               call       0x543e20
005095f7 83c40c                   add        esp, 0xc
005095fa 8b458c                   mov        eax, dword ptr [ebp - 0x74]
005095fd 0500010000               add        eax, 0x100
00509602 89458c                   mov        dword ptr [ebp - 0x74], eax
00509605 c78574ffffff00000000     mov        dword ptr [ebp - 0x8c], 0
0050960f eb0f                     jmp        0x509620
00509611 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00509617 83c101                   add        ecx, 1
0050961a 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
00509620 83bd74ffffff08           cmp        dword ptr [ebp - 0x8c], 8
00509627 0f8dae010000             jge        0x5097db
0050962d 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509630 83c220                   add        edx, 0x20
00509633 899530ffffff             mov        dword ptr [ebp - 0xd0], edx
00509639 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
0050963f 50                       push       eax
00509640 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
00509646 e835aff0ff               call       0x414580
0050964b 833800                   cmp        dword ptr [eax], 0
0050964e 7502                     jne        0x509652
00509650 ebbf                     jmp        0x509611
00509652 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509655 83c120                   add        ecx, 0x20
00509658 898d2cffffff             mov        dword ptr [ebp - 0xd4], ecx
0050965e 68a0020000               push       0x2a0
00509663 8b9574ffffff             mov        edx, dword ptr [ebp - 0x8c]
00509669 52                       push       edx
0050966a 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
00509670 e80baff0ff               call       0x414580
00509675 8b00                     mov        eax, dword ptr [eax]
00509677 50                       push       eax
00509678 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0050967e 034d8c                   add        ecx, dword ptr [ebp - 0x74]
00509681 51                       push       ecx
00509682 e899a70300               call       0x543e20
00509687 83c40c                   add        esp, 0xc
0050968a 8b558c                   mov        edx, dword ptr [ebp - 0x74]
0050968d 81c2a0020000             add        edx, 0x2a0
00509693 89558c                   mov        dword ptr [ebp - 0x74], edx
00509696 6b8574ffffff14           imul       eax, dword ptr [ebp - 0x8c], 0x14
0050969d 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005096a0 8d4c0140                 lea        ecx, [ecx + eax + 0x40]
005096a4 e8e768f0ff               call       0x40ff90
005096a9 894580                   mov        dword ptr [ebp - 0x80], eax
005096ac 837d8000                 cmp        dword ptr [ebp - 0x80], 0
005096b0 747f                     je         0x509731
005096b2 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
005096b5 e8462cf0ff               call       0x40c300
005096ba 898570ffffff             mov        dword ptr [ebp - 0x90], eax
005096c0 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
005096c6 e8d50a0000               call       0x50a1a0
005096cb 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
005096d1 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
005096d7 899514ffffff             mov        dword ptr [ebp - 0xec], edx
005096dd 6a00                     push       0
005096df 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
005096e5 e8e6e3ffff               call       0x507ad0
005096ea 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
005096f0 8b8524ffffff             mov        eax, dword ptr [ebp - 0xdc]
005096f6 50                       push       eax
005096f7 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
005096fd 51                       push       ecx
005096fe 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00509704 03558c                   add        edx, dword ptr [ebp - 0x74]
00509707 52                       push       edx
00509708 e813a70300               call       0x543e20
0050970d 83c40c                   add        esp, 0xc
00509710 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00509716 e8850a0000               call       0x50a1a0
0050971b 03458c                   add        eax, dword ptr [ebp - 0x74]
0050971e 89458c                   mov        dword ptr [ebp - 0x74], eax
00509721 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00509724 e86768f0ff               call       0x40ff90
00509729 894580                   mov        dword ptr [ebp - 0x80], eax
0050972c e97bffffff               jmp        0x5096ac
00509731 6b8574ffffff14           imul       eax, dword ptr [ebp - 0x8c], 0x14
00509738 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0050973b 8d4c0140                 lea        ecx, [ecx + eax + 0x40]
0050973f e84c68f0ff               call       0x40ff90
00509744 894580                   mov        dword ptr [ebp - 0x80], eax
00509747 837d8000                 cmp        dword ptr [ebp - 0x80], 0
0050974b 0f8485000000             je         0x5097d6
00509751 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00509754 e8a72bf0ff               call       0x40c300
00509759 898570ffffff             mov        dword ptr [ebp - 0x90], eax
0050975f 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00509765 e8060a0000               call       0x50a170
0050976a 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
00509770 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00509776 81c2c04b0300             add        edx, 0x34bc0
0050977c 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
00509782 6a00                     push       0
00509784 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
0050978a e88184f0ff               call       0x411c10
0050978f 8985fcfeffff             mov        dword ptr [ebp - 0x104], eax
00509795 8b8518ffffff             mov        eax, dword ptr [ebp - 0xe8]
0050979b 50                       push       eax
0050979c 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
005097a2 51                       push       ecx
005097a3 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005097a9 03558c                   add        edx, dword ptr [ebp - 0x74]
005097ac 52                       push       edx
005097ad e86ea60300               call       0x543e20
005097b2 83c40c                   add        esp, 0xc
005097b5 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
005097bb e8b0090000               call       0x50a170
005097c0 03458c                   add        eax, dword ptr [ebp - 0x74]
005097c3 89458c                   mov        dword ptr [ebp - 0x74], eax
005097c6 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
005097c9 e8c267f0ff               call       0x40ff90
005097ce 894580                   mov        dword ptr [ebp - 0x80], eax
005097d1 e971ffffff               jmp        0x509747
005097d6 e936feffff               jmp        0x509611
005097db 8d45ec                   lea        eax, [ebp - 0x14]
005097de 50                       push       eax
005097df 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
005097e2 51                       push       ecx
005097e3 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005097e9 52                       push       edx
005097ea e861fd0200               call       0x539550
005097ef 83c40c                   add        esp, 0xc
005097f2 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
005097f8 83bd78ffffff00           cmp        dword ptr [ebp - 0x88], 0
005097ff 741c                     je         0x50981d
00509801 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00509807 50                       push       eax
00509808 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0050980e e85d5ef1ff               call       0x41f670
00509813 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
0050981d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00509820 51                       push       ecx
00509821 6800010000               push       0x100
00509826 6a3a                     push       0x3a
00509828 6a7d                     push       0x7d
0050982a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0050982d 52                       push       edx
0050982e 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00509834 50                       push       eax
00509835 e8866bf0ff               call       0x4103c0
0050983a 83c418                   add        esp, 0x18
0050983d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00509840 51                       push       ecx
00509841 6800040000               push       0x400
00509846 68e1000000               push       0xe1
0050984b 6a5c                     push       0x5c
0050984d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00509850 52                       push       edx
00509851 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00509857 50                       push       eax
00509858 e8636bf0ff               call       0x4103c0
0050985d 83c418                   add        esp, 0x18
00509860 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509863 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00509866 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00509869 89422c                   mov        dword ptr [edx + 0x2c], eax
0050986c 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0050986f 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00509872 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00509875 894228                   mov        dword ptr [edx + 0x28], eax
00509878 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0050987b 8b5118                   mov        edx, dword ptr [ecx + 0x18]
0050987e 8b4228                   mov        eax, dword ptr [edx + 0x28]
00509881 83c030                   add        eax, 0x30
00509884 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509887 8b5118                   mov        edx, dword ptr [ecx + 0x18]
0050988a 89420c                   mov        dword ptr [edx + 0xc], eax
0050988d 6a00                     push       0
0050988f 68e1675b00               push       0x5b67e1
00509894 8d4dd4                   lea        ecx, [ebp - 0x2c]
00509897 e834e4f0ff               call       0x417cd0
0050989c c745fc00000000           mov        dword ptr [ebp - 4], 0
005098a3 68a4ca5600               push       0x56caa4
005098a8 8d4dd4                   lea        ecx, [ebp - 0x2c]
005098ab e890e5f0ff               call       0x417e40
005098b0 8d4508                   lea        eax, [ebp + 8]
005098b3 50                       push       eax
005098b4 8d4dd4                   lea        ecx, [ebp - 0x2c]
005098b7 e864edfcff               call       0x4d8620
005098bc 8d4dbc                   lea        ecx, [ebp - 0x44]
005098bf 51                       push       ecx
005098c0 8d4dd4                   lea        ecx, [ebp - 0x2c]
005098c3 e8c84ef1ff               call       0x41e790
005098c8 c645fc01                 mov        byte ptr [ebp - 4], 1
005098cc 8d4dbc                   lea        ecx, [ebp - 0x44]
005098cf e85c66f0ff               call       0x40ff30
005098d4 898510ffffff             mov        dword ptr [ebp - 0xf0], eax
005098da 8b9510ffffff             mov        edx, dword ptr [ebp - 0xf0]
005098e0 52                       push       edx
005098e1 e85a6ff0ff               call       0x410840
005098e6 83c404                   add        esp, 4
005098e9 6a30                     push       0x30
005098eb 8b4590                   mov        eax, dword ptr [ebp - 0x70]
005098ee 8b4818                   mov        ecx, dword ptr [eax + 0x18]
005098f1 51                       push       ecx
005098f2 e86977f0ff               call       0x411060
005098f7 83c408                   add        esp, 8
005098fa 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005098fd 52                       push       edx
005098fe 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00509904 50                       push       eax
00509905 e85677f0ff               call       0x411060
0050990a 83c408                   add        esp, 8
0050990d 83bd64ffffff00           cmp        dword ptr [ebp - 0x9c], 0
00509914 741c                     je         0x509932
00509916 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
0050991c 51                       push       ecx
0050991d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00509923 e8485df1ff               call       0x41f670
00509928 c78564ffffff00000000     mov        dword ptr [ebp - 0x9c], 0
00509932 c645fc00                 mov        byte ptr [ebp - 4], 0
00509936 8d4dbc                   lea        ecx, [ebp - 0x44]
00509939 e8624cf0ff               call       0x40e5a0
0050993e c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00509945 8d4dd4                   lea        ecx, [ebp - 0x2c]
00509948 e8734df0ff               call       0x40e6c0
0050994d 90                       nop        
0050994e 68e8345700               push       0x5734e8
00509953 68ffff0000               push       0xffff
00509958 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0050995e e8ad5cf1ff               call       0x41f610
00509963 894588                   mov        dword ptr [ebp - 0x78], eax
00509966 68ffff0000               push       0xffff
0050996b 6a00                     push       0
0050996d 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00509970 52                       push       edx
00509971 e8eaad0300               call       0x544760
00509976 83c40c                   add        esp, 0xc
00509979 8b4588                   mov        eax, dword ptr [ebp - 0x78]
0050997c 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00509982 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00509988 e873f1ffff               call       0x508b00
0050998d 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00509993 c6410800                 mov        byte ptr [ecx + 8], 0
00509997 ba0c000000               mov        edx, 0xc
0050999c c1e200                   shl        edx, 0
0050999f 03957cffffff             add        edx, dword ptr [ebp - 0x84]
005099a5 895594                   mov        dword ptr [ebp - 0x6c], edx
005099a8 6824355700               push       0x573524
005099ad 6830355700               push       0x573530
005099b2 6860ea0000               push       0xea60
005099b7 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
005099ba 50                       push       eax
005099bb e86058f1ff               call       0x41f220
005099c0 83c410                   add        esp, 0x10
005099c3 034594                   add        eax, dword ptr [ebp - 0x6c]
005099c6 894594                   mov        dword ptr [ebp - 0x6c], eax
005099c9 684c355700               push       0x57354c
005099ce 6854355700               push       0x573554
005099d3 6860ea0000               push       0xea60
005099d8 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005099db 51                       push       ecx
005099dc e83f58f1ff               call       0x41f220
005099e1 83c410                   add        esp, 0x10
005099e4 034594                   add        eax, dword ptr [ebp - 0x6c]
005099e7 894594                   mov        dword ptr [ebp - 0x6c], eax
005099ea 8b5590                   mov        edx, dword ptr [ebp - 0x70]
005099ed 8b421c                   mov        eax, dword ptr [edx + 0x1c]
005099f0 50                       push       eax
005099f1 6864355700               push       0x573564
005099f6 6860ea0000               push       0xea60
005099fb 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005099fe 51                       push       ecx
005099ff e81c58f1ff               call       0x41f220
00509a04 83c410                   add        esp, 0x10
00509a07 034594                   add        eax, dword ptr [ebp - 0x6c]
00509a0a 894594                   mov        dword ptr [ebp - 0x6c], eax
00509a0d 33d2                     xor        edx, edx
00509a0f 895598                   mov        dword ptr [ebp - 0x68], edx
00509a12 89559c                   mov        dword ptr [ebp - 0x64], edx
00509a15 8955a0                   mov        dword ptr [ebp - 0x60], edx
00509a18 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00509a1b 8955a8                   mov        dword ptr [ebp - 0x58], edx
00509a1e 8955ac                   mov        dword ptr [ebp - 0x54], edx
00509a21 8955b0                   mov        dword ptr [ebp - 0x50], edx
00509a24 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00509a27 8955b8                   mov        dword ptr [ebp - 0x48], edx
00509a2a 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509a2d 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00509a30 83c110                   add        ecx, 0x10
00509a33 51                       push       ecx
00509a34 8d5598                   lea        edx, [ebp - 0x68]
00509a37 52                       push       edx
00509a38 e83328faff               call       0x4ac270
00509a3d 83c408                   add        esp, 8
00509a40 8b459c                   mov        eax, dword ptr [ebp - 0x64]
00509a43 50                       push       eax
00509a44 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00509a47 51                       push       ecx
00509a48 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00509a4b 52                       push       edx
00509a4c 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
00509a4f 83c001                   add        eax, 1
00509a52 50                       push       eax
00509a53 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00509a56 99                       cdq        
00509a57 b964000000               mov        ecx, 0x64
00509a5c f7f9                     idiv       ecx
00509a5e 52                       push       edx
00509a5f 6870355700               push       0x573570
00509a64 6860ea0000               push       0xea60
00509a69 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509a6c 52                       push       edx
00509a6d e8ae57f1ff               call       0x41f220
00509a72 83c420                   add        esp, 0x20
00509a75 034594                   add        eax, dword ptr [ebp - 0x6c]
00509a78 894594                   mov        dword ptr [ebp - 0x6c], eax
00509a7b 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509a7e 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00509a81 6b91d800000009           imul       edx, dword ptr [ecx + 0xd8], 9
00509a88 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509a8b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00509a8e b804000000               mov        eax, 4
00509a93 6bc000                   imul       eax, eax, 0
00509a96 039401dc000000           add        edx, dword ptr [ecx + eax + 0xdc]
00509a9d 8b0c9580ff5a00           mov        ecx, dword ptr [edx*4 + 0x5aff80]
00509aa4 51                       push       ecx
00509aa5 6890355700               push       0x573590
00509aaa 6860ea0000               push       0xea60
00509aaf 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509ab2 52                       push       edx
00509ab3 e86857f1ff               call       0x41f220
00509ab8 83c410                   add        esp, 0x10
00509abb 034594                   add        eax, dword ptr [ebp - 0x6c]
00509abe 894594                   mov        dword ptr [ebp - 0x6c], eax
00509ac1 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509ac4 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00509ac7 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
00509acd 8b049590ff5a00           mov        eax, dword ptr [edx*4 + 0x5aff90]
00509ad4 50                       push       eax
00509ad5 689c355700               push       0x57359c
00509ada 6860ea0000               push       0xea60
00509adf 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509ae2 51                       push       ecx
00509ae3 e83857f1ff               call       0x41f220
00509ae8 83c410                   add        esp, 0x10
00509aeb 034594                   add        eax, dword ptr [ebp - 0x6c]
00509aee 894594                   mov        dword ptr [ebp - 0x6c], eax
00509af1 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00509af4 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00509af7 83b8f400000007           cmp        dword ptr [eax + 0xf4], 7
00509afe 7e48                     jle        0x509b48
00509b00 83bd6cffffff07           cmp        dword ptr [ebp - 0x94], 7
00509b07 751e                     jne        0x509b27
00509b09 68a8355700               push       0x5735a8
00509b0e 6860ea0000               push       0xea60
00509b13 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509b16 51                       push       ecx
00509b17 e80457f1ff               call       0x41f220
00509b1c 83c40c                   add        esp, 0xc
00509b1f 034594                   add        eax, dword ptr [ebp - 0x6c]
00509b22 894594                   mov        dword ptr [ebp - 0x6c], eax
00509b25 eb1c                     jmp        0x509b43
00509b27 68bc355700               push       0x5735bc
00509b2c 6860ea0000               push       0xea60
00509b31 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509b34 52                       push       edx
00509b35 e8e656f1ff               call       0x41f220
00509b3a 83c40c                   add        esp, 0xc
00509b3d 034594                   add        eax, dword ptr [ebp - 0x6c]
00509b40 894594                   mov        dword ptr [ebp - 0x6c], eax
00509b43 e984000000               jmp        0x509bcc
00509b48 8b856cffffff             mov        eax, dword ptr [ebp - 0x94]
00509b4e 3b8558ffffff             cmp        eax, dword ptr [ebp - 0xa8]
00509b54 754c                     jne        0x509ba2
00509b56 83bd6cffffff07           cmp        dword ptr [ebp - 0x94], 7
00509b5d 751e                     jne        0x509b7d
00509b5f 68d0355700               push       0x5735d0
00509b64 6860ea0000               push       0xea60
00509b69 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509b6c 51                       push       ecx
00509b6d e8ae56f1ff               call       0x41f220
00509b72 83c40c                   add        esp, 0xc
00509b75 034594                   add        eax, dword ptr [ebp - 0x6c]
00509b78 894594                   mov        dword ptr [ebp - 0x6c], eax
00509b7b eb23                     jmp        0x509ba0
00509b7d 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00509b83 52                       push       edx
00509b84 68e0355700               push       0x5735e0
00509b89 6860ea0000               push       0xea60
00509b8e 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509b91 50                       push       eax
00509b92 e88956f1ff               call       0x41f220
00509b97 83c410                   add        esp, 0x10
00509b9a 034594                   add        eax, dword ptr [ebp - 0x6c]
00509b9d 894594                   mov        dword ptr [ebp - 0x6c], eax
00509ba0 eb2a                     jmp        0x509bcc
00509ba2 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00509ba8 51                       push       ecx
00509ba9 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00509baf 52                       push       edx
00509bb0 68ec355700               push       0x5735ec
00509bb5 6860ea0000               push       0xea60
00509bba 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509bbd 50                       push       eax
00509bbe e85d56f1ff               call       0x41f220
00509bc3 83c414                   add        esp, 0x14
00509bc6 034594                   add        eax, dword ptr [ebp - 0x6c]
00509bc9 894594                   mov        dword ptr [ebp - 0x6c], eax
00509bcc 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509bcf 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00509bd2 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00509bd5 50                       push       eax
00509bd6 8b4a18                   mov        ecx, dword ptr [edx + 0x18]
00509bd9 51                       push       ecx
00509bda 6800365700               push       0x573600
00509bdf 6860ea0000               push       0xea60
00509be4 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509be7 52                       push       edx
00509be8 e83356f1ff               call       0x41f220
00509bed 83c414                   add        esp, 0x14
00509bf0 034594                   add        eax, dword ptr [ebp - 0x6c]
00509bf3 894594                   mov        dword ptr [ebp - 0x6c], eax
00509bf6 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509bf9 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00509bfc f30f5a81d0000000         cvtss2sd   xmm0, dword ptr [ecx + 0xd0]
00509c04 83ec08                   sub        esp, 8
00509c07 f20f110424               movsd      qword ptr [esp], xmm0
00509c0c 6810365700               push       0x573610
00509c11 6860ea0000               push       0xea60
00509c16 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509c19 52                       push       edx
00509c1a e80156f1ff               call       0x41f220
00509c1f 83c414                   add        esp, 0x14
00509c22 034594                   add        eax, dword ptr [ebp - 0x6c]
00509c25 894594                   mov        dword ptr [ebp - 0x6c], eax
00509c28 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509c2b 83c001                   add        eax, 1
00509c2e 894594                   mov        dword ptr [ebp - 0x6c], eax
00509c31 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509c34 2b4d88                   sub        ecx, dword ptr [ebp - 0x78]
00509c37 81e103000080             and        ecx, 0x80000003
00509c3d 7905                     jns        0x509c44
00509c3f 49                       dec        ecx
00509c40 83c9fc                   or         ecx, 0xfffffffc
00509c43 41                       inc        ecx
00509c44 85c9                     test       ecx, ecx
00509c46 7420                     je         0x509c68
00509c48 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509c4b 2b5588                   sub        edx, dword ptr [ebp - 0x78]
00509c4e 81e203000080             and        edx, 0x80000003
00509c54 7905                     jns        0x509c5b
00509c56 4a                       dec        edx
00509c57 83cafc                   or         edx, 0xfffffffc
00509c5a 42                       inc        edx
00509c5b b804000000               mov        eax, 4
00509c60 2bc2                     sub        eax, edx
00509c62 034594                   add        eax, dword ptr [ebp - 0x6c]
00509c65 894594                   mov        dword ptr [ebp - 0x6c], eax
00509c68 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509c6b 2b4d88                   sub        ecx, dword ptr [ebp - 0x78]
00509c6e 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
00509c74 894a04                   mov        dword ptr [edx + 4], ecx
00509c77 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509c7a 2b4588                   sub        eax, dword ptr [ebp - 0x78]
00509c7d 50                       push       eax
00509c7e 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00509c81 51                       push       ecx
00509c82 e8d973f0ff               call       0x411060
00509c87 83c408                   add        esp, 8
00509c8a 68ffff0000               push       0xffff
00509c8f 6a00                     push       0
00509c91 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00509c94 52                       push       edx
00509c95 e8c6aa0300               call       0x544760
00509c9a 83c40c                   add        esp, 0xc
00509c9d 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00509ca0 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00509ca6 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00509cac e84feeffff               call       0x508b00
00509cb1 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00509cb7 c6410801                 mov        byte ptr [ecx + 8], 1
00509cbb ba0c000000               mov        edx, 0xc
00509cc0 c1e200                   shl        edx, 0
00509cc3 03957cffffff             add        edx, dword ptr [ebp - 0x84]
00509cc9 895594                   mov        dword ptr [ebp - 0x6c], edx
00509ccc 6824365700               push       0x573624
00509cd1 6860ea0000               push       0xea60
00509cd6 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509cd9 50                       push       eax
00509cda e84155f1ff               call       0x41f220
00509cdf 83c40c                   add        esp, 0xc
00509ce2 034594                   add        eax, dword ptr [ebp - 0x6c]
00509ce5 894594                   mov        dword ptr [ebp - 0x6c], eax
00509ce8 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509ceb 83c101                   add        ecx, 1
00509cee 894d94                   mov        dword ptr [ebp - 0x6c], ecx
00509cf1 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509cf4 2b5588                   sub        edx, dword ptr [ebp - 0x78]
00509cf7 81e203000080             and        edx, 0x80000003
00509cfd 7905                     jns        0x509d04
00509cff 4a                       dec        edx
00509d00 83cafc                   or         edx, 0xfffffffc
00509d03 42                       inc        edx
00509d04 85d2                     test       edx, edx
00509d06 741f                     je         0x509d27
00509d08 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00509d0b 2b4588                   sub        eax, dword ptr [ebp - 0x78]
00509d0e 2503000080               and        eax, 0x80000003
00509d13 7905                     jns        0x509d1a
00509d15 48                       dec        eax
00509d16 83c8fc                   or         eax, 0xfffffffc
00509d19 40                       inc        eax
00509d1a b904000000               mov        ecx, 4
00509d1f 2bc8                     sub        ecx, eax
00509d21 034d94                   add        ecx, dword ptr [ebp - 0x6c]
00509d24 894d94                   mov        dword ptr [ebp - 0x6c], ecx
00509d27 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00509d2a 2b5588                   sub        edx, dword ptr [ebp - 0x78]
00509d2d 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
00509d33 895004                   mov        dword ptr [eax + 4], edx
00509d36 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00509d39 2b4d88                   sub        ecx, dword ptr [ebp - 0x78]
00509d3c 51                       push       ecx
00509d3d 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00509d40 52                       push       edx
00509d41 e81a73f0ff               call       0x411060
00509d46 83c408                   add        esp, 8
00509d49 837d8800                 cmp        dword ptr [ebp - 0x78], 0
00509d4d 7416                     je         0x509d65
00509d4f 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00509d52 50                       push       eax
00509d53 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00509d59 e81259f1ff               call       0x41f670
00509d5e c7458800000000           mov        dword ptr [ebp - 0x78], 0
00509d65 e87669f0ff               call       0x4106e0
00509d6a 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00509d6d 8b915c020000             mov        edx, dword ptr [ecx + 0x25c]
00509d73 83ca01                   or         edx, 1
00509d76 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00509d79 89905c020000             mov        dword ptr [eax + 0x25c], edx
00509d7f 33c0                     xor        eax, eax
00509d81 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00509d84 64890d00000000           mov        dword ptr fs:[0], ecx
00509d8b 59                       pop        ecx
00509d8c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00509d8f 33cd                     xor        ecx, ebp
00509d91 e8588c0300               call       0x5429ee
00509d96 8be5                     mov        esp, ebp
00509d98 5d                       pop        ebp
00509d99 c21000                   ret        0x10
00509d9c cc                       int3       
00509d9d cc                       int3       
00509d9e cc                       int3       
00509d9f cc                       int3       
