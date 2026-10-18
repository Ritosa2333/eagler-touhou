00508480 53                       push       ebx
00508481 8bdc                     mov        ebx, esp
00508483 83ec08                   sub        esp, 8
00508486 83e4f8                   and        esp, 0xfffffff8
00508489 83c404                   add        esp, 4
0050848c 55                       push       ebp
0050848d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
00508490 896c2404                 mov        dword ptr [esp + 4], ebp
00508494 8bec                     mov        ebp, esp
00508496 81ecb8020000             sub        esp, 0x2b8
0050849c a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005084a1 33c5                     xor        eax, ebp
005084a3 8945fc                   mov        dword ptr [ebp - 4], eax
005084a6 56                       push       esi
005084a7 57                       push       edi
005084a8 894de0                   mov        dword ptr [ebp - 0x20], ecx
005084ab 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005084ae 8b4b08                   mov        ecx, dword ptr [ebx + 8]
005084b1 894810                   mov        dword ptr [eax + 0x10], ecx
005084b4 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005084b7 837a1000                 cmp        dword ptr [edx + 0x10], 0
005084bb 0f85ad030000             jne        0x50886e
005084c1 b968a55b00               mov        ecx, 0x5ba568
005084c6 e895c8f6ff               call       0x474d60
005084cb 6bc014                   imul       eax, eax, 0x14
005084ce 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005084d1 8d4c0140                 lea        ecx, [ecx + eax + 0x40]
005084d5 e846060000               call       0x508b20
005084da 6838335700               push       0x573338
005084df 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005084e5 e816ebffff               call       0x507000
005084ea 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005084ed 894218                   mov        dword ptr [edx + 0x18], eax
005084f0 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005084f3 8b4818                   mov        ecx, dword ptr [eax + 0x18]
005084f6 c7411c30000000           mov        dword ptr [ecx + 0x1c], 0x30
005084fd 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508500 8b4218                   mov        eax, dword ptr [edx + 0x18]
00508503 c7402000010000           mov        dword ptr [eax + 0x20], 0x100
0050850a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050850d 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00508510 c74224a0020000           mov        dword ptr [edx + 0x24], 0x2a0
00508517 6880335700               push       0x573380
0050851c 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00508522 e859eaffff               call       0x506f80
00508527 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050852a 89411c                   mov        dword ptr [ecx + 0x1c], eax
0050852d 68c8335700               push       0x5733c8
00508532 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00508538 e843ebffff               call       0x507080
0050853d 894588                   mov        dword ptr [ebp - 0x78], eax
00508540 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508543 83c220                   add        edx, 0x20
00508546 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00508549 b968a55b00               mov        ecx, 0x5ba568
0050854e e80dc8f6ff               call       0x474d60
00508553 50                       push       eax
00508554 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00508557 e824c0f0ff               call       0x414580
0050855c 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
0050855f 8908                     mov        dword ptr [eax], ecx
00508561 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508564 83c220                   add        edx, 0x20
00508567 89558c                   mov        dword ptr [ebp - 0x74], edx
0050856a b968a55b00               mov        ecx, 0x5ba568
0050856f e8ecc7f6ff               call       0x474d60
00508574 50                       push       eax
00508575 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00508578 e803c0f0ff               call       0x414580
0050857d 8b00                     mov        eax, dword ptr [eax]
0050857f 8945dc                   mov        dword ptr [ebp - 0x24], eax
00508582 b968a55b00               mov        ecx, 0x5ba568
00508587 e8e4f9faff               call       0x4b7f70
0050858c 8945b0                   mov        dword ptr [ebp - 0x50], eax
0050858f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508592 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00508595 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00508598 8982d8000000             mov        dword ptr [edx + 0xd8], eax
0050859e c745f000000000           mov        dword ptr [ebp - 0x10], 0
005085a5 8d4df0                   lea        ecx, [ebp - 0x10]
005085a8 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005085ab 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
005085ae 8b02                     mov        eax, dword ptr [edx]
005085b0 50                       push       eax
005085b1 8d4df8                   lea        ecx, [ebp - 8]
005085b4 51                       push       ecx
005085b5 e8568ff5ff               call       0x461510
005085ba 83c408                   add        esp, 8
005085bd 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
005085c0 8b02                     mov        eax, dword ptr [edx]
005085c2 50                       push       eax
005085c3 8d4dec                   lea        ecx, [ebp - 0x14]
005085c6 51                       push       ecx
005085c7 e80490f5ff               call       0x4615d0
005085cc 83c408                   add        esp, 8
005085cf eb09                     jmp        0x5085da
005085d1 8d4df8                   lea        ecx, [ebp - 8]
005085d4 e88792f5ff               call       0x461860
005085d9 90                       nop        
005085da 8d55ec                   lea        edx, [ebp - 0x14]
005085dd 52                       push       edx
005085de 8d4df8                   lea        ecx, [ebp - 8]
005085e1 e87a0ef4ff               call       0x449460
005085e6 0fb6c0                   movzx      eax, al
005085e9 85c0                     test       eax, eax
005085eb 746e                     je         0x50865b
005085ed 8d4df8                   lea        ecx, [ebp - 8]
005085f0 e80b3df0ff               call       0x40c300
005085f5 8945d0                   mov        dword ptr [ebp - 0x30], eax
005085f8 6a00                     push       0
005085fa e881baf5ff               call       0x464080
005085ff 83c404                   add        esp, 4
00508602 8945ac                   mov        dword ptr [ebp - 0x54], eax
00508605 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00508608 51                       push       ecx
00508609 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0050860c e86f1c0000               call       0x50a280
00508611 8945a8                   mov        dword ptr [ebp - 0x58], eax
00508614 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508617 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0050861a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0050861d 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
00508620 899488dc000000           mov        dword ptr [eax + ecx*4 + 0xdc], edx
00508627 6a00                     push       0
00508629 e852baf5ff               call       0x464080
0050862e 83c404                   add        esp, 4
00508631 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00508634 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00508637 50                       push       eax
00508638 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
0050863b e8e0bdf5ff               call       0x464420
00508640 8845da                   mov        byte ptr [ebp - 0x26], al
00508643 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508646 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00508649 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0050864c 8a4dda                   mov        cl, byte ptr [ebp - 0x26]
0050864f 888c02ec000000           mov        byte ptr [edx + eax + 0xec], cl
00508656 e976ffffff               jmp        0x5085d1
0050865b b968a55b00               mov        ecx, 0x5ba568
00508660 e83bbaf5ff               call       0x4640a0
00508665 8945a0                   mov        dword ptr [ebp - 0x60], eax
00508668 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0050866b 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0050866e 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00508671 8988f0000000             mov        dword ptr [eax + 0xf0], ecx
00508677 b968a55b00               mov        ecx, 0x5ba568
0050867c e89f51fbff               call       0x4bd820
00508681 8845db                   mov        byte ptr [ebp - 0x25], al
00508684 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508687 8b421c                   mov        eax, dword ptr [edx + 0x1c]
0050868a 8a4ddb                   mov        cl, byte ptr [ebp - 0x25]
0050868d 80e101                   and        cl, 1
00508690 8a500a                   mov        dl, byte ptr [eax + 0xa]
00508693 80e2fe                   and        dl, 0xfe
00508696 0ad1                     or         dl, cl
00508698 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0050869b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0050869e 88510a                   mov        byte ptr [ecx + 0xa], dl
005086a1 b968a55b00               mov        ecx, 0x5ba568
005086a6 e8a501f8ff               call       0x488850
005086ab 0fb6d0                   movzx      edx, al
005086ae 85d2                     test       edx, edx
005086b0 7409                     je         0x5086bb
005086b2 c745c001000000           mov        dword ptr [ebp - 0x40], 1
005086b9 eb07                     jmp        0x5086c2
005086bb c745c000000000           mov        dword ptr [ebp - 0x40], 0
005086c2 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005086c5 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
005086c8 8a55c0                   mov        dl, byte ptr [ebp - 0x40]
005086cb 80e201                   and        dl, 1
005086ce d0e2                     shl        dl, 1
005086d0 8a410a                   mov        al, byte ptr [ecx + 0xa]
005086d3 24fd                     and        al, 0xfd
005086d5 0ac2                     or         al, dl
005086d7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005086da 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
005086dd 88420a                   mov        byte ptr [edx + 0xa], al
005086e0 b968a55b00               mov        ecx, 0x5ba568
005086e5 e8760df9ff               call       0x499460
005086ea 89459c                   mov        dword ptr [ebp - 0x64], eax
005086ed 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005086f0 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
005086f3 8b559c                   mov        edx, dword ptr [ebp - 0x64]
005086f6 8991fc000000             mov        dword ptr [ecx + 0xfc], edx
005086fc 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00508703 7419                     je         0x50871e
00508705 8b3528a85b00             mov        esi, dword ptr [0x5ba828]
0050870b 83c638                   add        esi, 0x38
0050870e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00508711 8b781c                   mov        edi, dword ptr [eax + 0x1c]
00508714 83c720                   add        edi, 0x20
00508717 b92c000000               mov        ecx, 0x2c
0050871c f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
0050871e b968a55b00               mov        ecx, 0x5ba568
00508723 e838c6f6ff               call       0x474d60
00508728 0fbfc8                   movsx      ecx, ax
0050872b 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0050872e 890a                     mov        dword ptr [edx], ecx
00508730 b9a8a45b00               mov        ecx, 0x5ba4a8
00508735 e85643f1ff               call       0x41ca90
0050873a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0050873d 894104                   mov        dword ptr [ecx + 4], eax
00508740 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00508743 8b4204                   mov        eax, dword ptr [edx + 4]
00508746 50                       push       eax
00508747 6818345700               push       0x573418
0050874c e85f3ff0ff               call       0x40c6b0
00508751 83c408                   add        esp, 8
00508754 b9a8a45b00               mov        ecx, 0x5ba4a8
00508759 e8f252f9ff               call       0x49da50
0050875e e89d38f9ff               call       0x49c000
00508763 894598                   mov        dword ptr [ebp - 0x68], eax
00508766 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00508769 83e101                   and        ecx, 1
0050876c 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0050876f 8b8298020000             mov        eax, dword ptr [edx + 0x298]
00508775 83e0fe                   and        eax, 0xfffffffe
00508778 0bc1                     or         eax, ecx
0050877a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0050877d 898198020000             mov        dword ptr [ecx + 0x298], eax
00508783 e87838f9ff               call       0x49c000
00508788 85c0                     test       eax, eax
0050878a 7414                     je         0x5087a0
0050878c 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0050878f c7421000000000           mov        dword ptr [edx + 0x10], 0
00508796 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00508799 c7401400000000           mov        dword ptr [eax + 0x14], 0
005087a0 e89b07f9ff               call       0x498f40
005087a5 8b7ddc                   mov        edi, dword ptr [ebp - 0x24]
005087a8 83c770                   add        edi, 0x70
005087ab b98a000000               mov        ecx, 0x8a
005087b0 8bf0                     mov        esi, eax
005087b2 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
005087b4 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
005087bb eb09                     jmp        0x5087c6
005087bd 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
005087c0 83c101                   add        ecx, 1
005087c3 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
005087c6 837dd414                 cmp        dword ptr [ebp - 0x2c], 0x14
005087ca 7d13                     jge        0x5087df
005087cc 6955d4addeadde           imul       edx, dword ptr [ebp - 0x2c], 0xdeaddead
005087d3 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
005087d6 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005087d9 8954811c                 mov        dword ptr [ecx + eax*4 + 0x1c], edx
005087dd ebde                     jmp        0x5087bd
005087df b968a55b00               mov        ecx, 0x5ba568
005087e4 e8b7f7faff               call       0x4b7fa0
005087e9 894594                   mov        dword ptr [ebp - 0x6c], eax
005087ec 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005087ef 8b421c                   mov        eax, dword ptr [edx + 0x1c]
005087f2 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005087f5 8988f8000000             mov        dword ptr [eax + 0xf8], ecx
005087fb 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005087fe 52                       push       edx
005087ff 68609f5000               push       0x509f60
00508804 6a15                     push       0x15
00508806 e8059bf0ff               call       0x412310
0050880b 83c40c                   add        esp, 0xc
0050880e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508811 894108                   mov        dword ptr [ecx + 8], eax
00508814 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508817 52                       push       edx
00508818 68709f5000               push       0x509f70
0050881d 6a2e                     push       0x2e
0050881f e8ec9af0ff               call       0x412310
00508824 83c40c                   add        esp, 0xc
00508827 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050882a 898154020000             mov        dword ptr [ecx + 0x254], eax
00508830 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508833 52                       push       edx
00508834 6800a05000               push       0x50a000
00508839 6a5b                     push       0x5b
0050883b e8709bf0ff               call       0x4123b0
00508840 83c40c                   add        esp, 0xc
00508843 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508846 89410c                   mov        dword ptr [ecx + 0xc], eax
00508849 b968a55b00               mov        ecx, 0x5ba568
0050884e e80dc5f6ff               call       0x474d60
00508853 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508856 898258020000             mov        dword ptr [edx + 0x258], eax
0050885c 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0050885f c78050020000ffffffff     mov        dword ptr [eax + 0x250], 0xffffffff
00508869 e978020000               jmp        0x508ae6
0050886e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508871 83791001                 cmp        dword ptr [ecx + 0x10], 1
00508875 0f854d020000             jne        0x508ac8
0050887b 8b530c                   mov        edx, dword ptr [ebx + 0xc]
0050887e 52                       push       edx
0050887f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508882 e809030000               call       0x508b90
00508887 85c0                     test       eax, eax
00508889 7408                     je         0x508893
0050888b 83c8ff                   or         eax, 0xffffffff
0050888e e955020000               jmp        0x508ae8
00508893 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00508896 8b701c                   mov        esi, dword ptr [eax + 0x1c]
00508899 83c620                   add        esi, 0x20
0050889c 8b3d28a85b00             mov        edi, dword ptr [0x5ba828]
005088a2 83c738                   add        edi, 0x38
005088a5 b92c000000               mov        ecx, 0x2c
005088aa f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
005088ac 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005088af 81c1e8000000             add        ecx, 0xe8
005088b5 894d90                   mov        dword ptr [ebp - 0x70], ecx
005088b8 b968a55b00               mov        ecx, 0x5ba568
005088bd e89ec4f6ff               call       0x474d60
005088c2 50                       push       eax
005088c3 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005088c6 e865e5f7ff               call       0x486e30
005088cb 8945bc                   mov        dword ptr [ebp - 0x44], eax
005088ce 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
005088d1 8b4210                   mov        eax, dword ptr [edx + 0x10]
005088d4 8945c8                   mov        dword ptr [ebp - 0x38], eax
005088d7 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005088da e831150000               call       0x509e10
005088df 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005088e2 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
005088e5 8b82d8000000             mov        eax, dword ptr [edx + 0xd8]
005088eb 50                       push       eax
005088ec b968a55b00               mov        ecx, 0x5ba568
005088f1 e8ea1a0000               call       0x50a3e0
005088f6 90                       nop        
005088f7 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
005088fe c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00508905 8d4de4                   lea        ecx, [ebp - 0x1c]
00508908 894db8                   mov        dword ptr [ebp - 0x48], ecx
0050890b 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0050890e 8b02                     mov        eax, dword ptr [edx]
00508910 50                       push       eax
00508911 8d4df4                   lea        ecx, [ebp - 0xc]
00508914 51                       push       ecx
00508915 e8f68bf5ff               call       0x461510
0050891a 83c408                   add        esp, 8
0050891d 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00508920 8b02                     mov        eax, dword ptr [edx]
00508922 50                       push       eax
00508923 8d4de8                   lea        ecx, [ebp - 0x18]
00508926 51                       push       ecx
00508927 e8a48cf5ff               call       0x4615d0
0050892c 83c408                   add        esp, 8
0050892f eb09                     jmp        0x50893a
00508931 8d4df4                   lea        ecx, [ebp - 0xc]
00508934 e8278ff5ff               call       0x461860
00508939 90                       nop        
0050893a 8d55e8                   lea        edx, [ebp - 0x18]
0050893d 52                       push       edx
0050893e 8d4df4                   lea        ecx, [ebp - 0xc]
00508941 e81a0bf4ff               call       0x449460
00508946 0fb6c0                   movzx      eax, al
00508949 85c0                     test       eax, eax
0050894b 744d                     je         0x50899a
0050894d 8d4df4                   lea        ecx, [ebp - 0xc]
00508950 e8ab39f0ff               call       0x40c300
00508955 894580                   mov        dword ptr [ebp - 0x80], eax
00508958 6a00                     push       0
0050895a e821b7f5ff               call       0x464080
0050895f 83c404                   add        esp, 4
00508962 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00508968 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050896b 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0050896e 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00508971 8b8c82dc000000           mov        ecx, dword ptr [edx + eax*4 + 0xdc]
00508978 894d84                   mov        dword ptr [ebp - 0x7c], ecx
0050897b 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0050897e 83c201                   add        edx, 1
00508981 8955cc                   mov        dword ptr [ebp - 0x34], edx
00508984 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
00508987 50                       push       eax
00508988 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0050898b 51                       push       ecx
0050898c 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00508992 e8d91e0000               call       0x50a870
00508997 90                       nop        
00508998 eb97                     jmp        0x508931
0050899a 6a00                     push       0
0050899c e88fb8f5ff               call       0x464230
005089a1 83c404                   add        esp, 4
005089a4 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
005089aa 6a00                     push       0
005089ac e8cfb6f5ff               call       0x464080
005089b1 83c404                   add        esp, 4
005089b4 8bc8                     mov        ecx, eax
005089b6 e8759df0ff               call       0x412730
005089bb 898578ffffff             mov        dword ptr [ebp - 0x88], eax
005089c1 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
005089c7 52                       push       edx
005089c8 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
005089ce e8adb60200               call       0x534080
005089d3 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
005089d6 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
005089d9 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
005089df 52                       push       edx
005089e0 b968a55b00               mov        ecx, 0x5ba568
005089e5 e8c657fbff               call       0x4be1b0
005089ea 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
005089ed 8b4804                   mov        ecx, dword ptr [eax + 4]
005089f0 51                       push       ecx
005089f1 b9a8a45b00               mov        ecx, 0x5ba4a8
005089f6 e8450ffdff               call       0x4d9940
005089fb 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
005089fe 8b4204                   mov        eax, dword ptr [edx + 4]
00508a01 50                       push       eax
00508a02 6818345700               push       0x573418
00508a07 e8a43cf0ff               call       0x40c6b0
00508a0c 83c408                   add        esp, 8
00508a0f b9a8a45b00               mov        ecx, 0x5ba4a8
00508a14 e83750f9ff               call       0x49da50
00508a19 90                       nop        
00508a1a 8b75c8                   mov        esi, dword ptr [ebp - 0x38]
00508a1d 83c670                   add        esi, 0x70
00508a20 b98a000000               mov        ecx, 0x8a
00508a25 8dbd48fdffff             lea        edi, [ebp - 0x2b8]
00508a2b f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00508a2d e80e05f9ff               call       0x498f40
00508a32 b98a000000               mov        ecx, 0x8a
00508a37 8db548fdffff             lea        esi, [ebp - 0x2b8]
00508a3d 8bf8                     mov        edi, eax
00508a3f f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00508a41 b968a55b00               mov        ecx, 0x5ba568
00508a46 e8150af9ff               call       0x499460
00508a4b 85c0                     test       eax, eax
00508a4d 7c0f                     jl         0x508a5e
00508a4f 6a02                     push       2
00508a51 b968a55b00               mov        ecx, 0x5ba568
00508a56 e8451a0000               call       0x50a4a0
00508a5b 90                       nop        
00508a5c eb0d                     jmp        0x508a6b
00508a5e 6a00                     push       0
00508a60 b968a55b00               mov        ecx, 0x5ba568
00508a65 e8361a0000               call       0x50a4a0
00508a6a 90                       nop        
00508a6b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508a6e 51                       push       ecx
00508a6f 68509f5000               push       0x509f50
00508a74 6a15                     push       0x15
00508a76 e89598f0ff               call       0x412310
00508a7b 83c40c                   add        esp, 0xc
00508a7e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508a81 894208                   mov        dword ptr [edx + 8], eax
00508a84 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00508a87 50                       push       eax
00508a88 68709f5000               push       0x509f70
00508a8d 6a2e                     push       0x2e
00508a8f e87c98f0ff               call       0x412310
00508a94 83c40c                   add        esp, 0xc
00508a97 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508a9a 898154020000             mov        dword ptr [ecx + 0x254], eax
00508aa0 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508aa3 52                       push       edx
00508aa4 6800a05000               push       0x50a000
00508aa9 6a5b                     push       0x5b
00508aab e80099f0ff               call       0x4123b0
00508ab0 83c40c                   add        esp, 0xc
00508ab3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508ab6 89410c                   mov        dword ptr [ecx + 0xc], eax
00508ab9 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00508abc c78258020000ffffffff     mov        dword ptr [edx + 0x258], 0xffffffff
00508ac6 eb1e                     jmp        0x508ae6
00508ac8 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00508acb 83781002                 cmp        dword ptr [eax + 0x10], 2
00508acf 7515                     jne        0x508ae6
00508ad1 8b4b0c                   mov        ecx, dword ptr [ebx + 0xc]
00508ad4 51                       push       ecx
00508ad5 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00508ad8 e8b3000000               call       0x508b90
00508add 85c0                     test       eax, eax
00508adf 7405                     je         0x508ae6
00508ae1 83c8ff                   or         eax, 0xffffffff
00508ae4 eb02                     jmp        0x508ae8
00508ae6 33c0                     xor        eax, eax
00508ae8 5f                       pop        edi
00508ae9 5e                       pop        esi
00508aea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00508aed 33cd                     xor        ecx, ebp
00508aef e8fa9e0300               call       0x5429ee
00508af4 8be5                     mov        esp, ebp
00508af6 5d                       pop        ebp
00508af7 8be3                     mov        esp, ebx
00508af9 5b                       pop        ebx
00508afa c20800                   ret        8
00508afd cc                       int3       
00508afe cc                       int3       
00508aff cc                       int3       
