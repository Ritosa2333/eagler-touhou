00483580 55                       push       ebp
00483581 8bec                     mov        ebp, esp
00483583 83ec70                   sub        esp, 0x70
00483586 56                       push       esi
00483587 894dfc                   mov        dword ptr [ebp - 4], ecx
0048358a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048358d 05a0000000               add        eax, 0xa0
00483592 8945ec                   mov        dword ptr [ebp - 0x14], eax
00483595 6a03                     push       3
00483597 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0048359a e8e190ffff               call       0x47c680
0048359f 8945e0                   mov        dword ptr [ebp - 0x20], eax
004835a2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004835a5 81c1a0000000             add        ecx, 0xa0
004835ab 894de8                   mov        dword ptr [ebp - 0x18], ecx
004835ae 6a03                     push       3
004835b0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004835b3 e8c890ffff               call       0x47c680
004835b8 8b5030                   mov        edx, dword ptr [eax + 0x30]
004835bb 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004835be 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004835c1 50                       push       eax
004835c2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004835c5 e8f6fff9ff               call       0x4235c0
004835ca 0fb6c8                   movzx      ecx, al
004835cd 85c9                     test       ecx, ecx
004835cf 0f8471020000             je         0x483846
004835d5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004835d8 837a4000                 cmp        dword ptr [edx + 0x40], 0
004835dc 7c14                     jl         0x4835f2
004835de 6a00                     push       0
004835e0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004835e3 8b4840                   mov        ecx, dword ptr [eax + 0x40]
004835e6 51                       push       ecx
004835e7 b930a85b00               mov        ecx, 0x5ba830
004835ec e87f37faff               call       0x426d70
004835f1 90                       nop        
004835f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004835f5 81c2a0000000             add        edx, 0xa0
004835fb 8955dc                   mov        dword ptr [ebp - 0x24], edx
004835fe 6a03                     push       3
00483600 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00483603 e87890ffff               call       0x47c680
00483608 83c038                   add        eax, 0x38
0048360b 8945f4                   mov        dword ptr [ebp - 0xc], eax
0048360e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00483611 8b08                     mov        ecx, dword ptr [eax]
00483613 83c101                   add        ecx, 1
00483616 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00483619 890a                     mov        dword ptr [edx], ecx
0048361b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048361e 05a0000000               add        eax, 0xa0
00483623 8945d8                   mov        dword ptr [ebp - 0x28], eax
00483626 6a03                     push       3
00483628 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0048362b e85090ffff               call       0x47c680
00483630 8b483c                   mov        ecx, dword ptr [eax + 0x3c]
00483633 894df0                   mov        dword ptr [ebp - 0x10], ecx
00483636 837df006                 cmp        dword ptr [ebp - 0x10], 6
0048363a 0f8738010000             ja         0x483778
00483640 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00483643 ff249504394800           jmp        dword ptr [edx*4 + 0x483904]
0048364a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048364d 05a0000000               add        eax, 0xa0
00483652 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00483655 6a03                     push       3
00483657 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0048365a e82190ffff               call       0x47c680
0048365f 51                       push       ecx
00483660 f30f104014               movss      xmm0, dword ptr [eax + 0x14]
00483665 f30f110424               movss      dword ptr [esp], xmm0
0048366a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048366d 81c184000000             add        ecx, 0x84
00483673 e848fafcff               call       0x4530c0
00483678 90                       nop        
00483679 e9fa000000               jmp        0x483778
0048367e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483681 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
00483687 e804c9f8ff               call       0x40ff90
0048368c 8945d0                   mov        dword ptr [ebp - 0x30], eax
0048368f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483692 81c2a0000000             add        edx, 0xa0
00483698 8955c8                   mov        dword ptr [ebp - 0x38], edx
0048369b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048369e 83c064                   add        eax, 0x64
004836a1 50                       push       eax
004836a2 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004836a5 e8a6bc0700               call       0x4ff350
004836aa d95dcc                   fstp       dword ptr [ebp - 0x34]
004836ad f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004836b2 6a03                     push       3
004836b4 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004836b7 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
004836bc e8bf8fffff               call       0x47c680
004836c1 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
004836c6 f30f584014               addss      xmm0, dword ptr [eax + 0x14]
004836cb 51                       push       ecx
004836cc f30f110424               movss      dword ptr [esp], xmm0
004836d1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004836d4 81c184000000             add        ecx, 0x84
004836da e841f8fcff               call       0x452f20
004836df 90                       nop        
004836e0 e993000000               jmp        0x483778
004836e5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004836e8 81c1a0000000             add        ecx, 0xa0
004836ee 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004836f1 6a03                     push       3
004836f3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004836f6 e8858fffff               call       0x47c680
004836fb 51                       push       ecx
004836fc f30f104014               movss      xmm0, dword ptr [eax + 0x14]
00483701 f30f110424               movss      dword ptr [esp], xmm0
00483706 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483709 81c184000000             add        ecx, 0x84
0048370f e80cf8fcff               call       0x452f20
00483714 90                       nop        
00483715 eb61                     jmp        0x483778
00483717 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048371a 81c2a0000000             add        edx, 0xa0
00483720 8955bc                   mov        dword ptr [ebp - 0x44], edx
00483723 6a03                     push       3
00483725 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00483728 e8538fffff               call       0x47c680
0048372d 51                       push       ecx
0048372e f30f104014               movss      xmm0, dword ptr [eax + 0x14]
00483733 f30f110424               movss      dword ptr [esp], xmm0
00483738 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048373b 81c184000000             add        ecx, 0x84
00483741 e8daf7fcff               call       0x452f20
00483746 90                       nop        
00483747 eb2f                     jmp        0x483778
00483749 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048374c 05a0000000               add        eax, 0xa0
00483751 8945b8                   mov        dword ptr [ebp - 0x48], eax
00483754 6a03                     push       3
00483756 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00483759 e8228fffff               call       0x47c680
0048375e 51                       push       ecx
0048375f f30f104014               movss      xmm0, dword ptr [eax + 0x14]
00483764 f30f110424               movss      dword ptr [esp], xmm0
00483769 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048376c 81c184000000             add        ecx, 0x84
00483772 e8a9f7fcff               call       0x452f20
00483777 90                       nop        
00483778 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048377b 81c1a0000000             add        ecx, 0xa0
00483781 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00483784 6a03                     push       3
00483786 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00483789 e8f28effff               call       0x47c680
0048378e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483791 8b4010                   mov        eax, dword ptr [eax + 0x10]
00483794 894220                   mov        dword ptr [edx + 0x20], eax
00483797 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048379a f30f104120               movss      xmm0, dword ptr [ecx + 0x20]
0048379f f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004837a4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004837a7 81c2a0000000             add        edx, 0xa0
004837ad 8955b0                   mov        dword ptr [ebp - 0x50], edx
004837b0 6a00                     push       0
004837b2 6a03                     push       3
004837b4 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004837b7 e8c48effff               call       0x47c680
004837bc 8bc8                     mov        ecx, eax
004837be e85dfdf9ff               call       0x423520
004837c3 90                       nop        
004837c4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004837c7 05a0000000               add        eax, 0xa0
004837cc 8945a8                   mov        dword ptr [ebp - 0x58], eax
004837cf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004837d2 81c1a0000000             add        ecx, 0xa0
004837d8 894dac                   mov        dword ptr [ebp - 0x54], ecx
004837db 6a03                     push       3
004837dd 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004837e0 e89b8effff               call       0x47c680
004837e5 8bf0                     mov        esi, eax
004837e7 6a03                     push       3
004837e9 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004837ec e88f8effff               call       0x47c680
004837f1 8b5038                   mov        edx, dword ptr [eax + 0x38]
004837f4 3b5634                   cmp        edx, dword ptr [esi + 0x34]
004837f7 7c4b                     jl         0x483844
004837f9 51                       push       ecx
004837fa f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004837ff f30f110424               movss      dword ptr [esp], xmm0
00483804 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483807 81c184000000             add        ecx, 0x84
0048380d e8ce5afaff               call       0x4292e0
00483812 51                       push       ecx
00483813 d91c24                   fstp       dword ptr [esp]
00483816 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483819 83c070                   add        eax, 0x70
0048381c 50                       push       eax
0048381d e80e5bfbff               call       0x439330
00483822 83c40c                   add        esp, 0xc
00483825 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483828 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
0048382e 83e2ef                   and        edx, 0xffffffef
00483831 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483834 899090000000             mov        dword ptr [eax + 0x90], edx
0048383a b801000000               mov        eax, 1
0048383f e9b9000000               jmp        0x4838fd
00483844 eb67                     jmp        0x4838ad
00483846 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483849 81c1a0000000             add        ecx, 0xa0
0048384f 894da0                   mov        dword ptr [ebp - 0x60], ecx
00483852 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483855 81c2a0000000             add        edx, 0xa0
0048385b 8955a4                   mov        dword ptr [ebp - 0x5c], edx
0048385e 6a03                     push       3
00483860 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00483863 e8188effff               call       0x47c680
00483868 f30f2a4030               cvtsi2ss   xmm0, dword ptr [eax + 0x30]
0048386d 6a03                     push       3
0048386f 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00483872 f30f114598               movss      dword ptr [ebp - 0x68], xmm0
00483877 e8048effff               call       0x47c680
0048387c 8bc8                     mov        ecx, eax
0048387e e84d03faff               call       0x423bd0
00483883 d95d9c                   fstp       dword ptr [ebp - 0x64]
00483886 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483889 f30f10459c               movss      xmm0, dword ptr [ebp - 0x64]
0048388e f30f594020               mulss      xmm0, dword ptr [eax + 0x20]
00483893 f30f104d98               movss      xmm1, dword ptr [ebp - 0x68]
00483898 f30f5ec1                 divss      xmm0, xmm1
0048389c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048389f f30f104920               movss      xmm1, dword ptr [ecx + 0x20]
004838a4 f30f5cc8                 subss      xmm1, xmm0
004838a8 f30f114df8               movss      dword ptr [ebp - 8], xmm1
004838ad 51                       push       ecx
004838ae f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004838b3 f30f110424               movss      dword ptr [esp], xmm0
004838b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004838bb 81c184000000             add        ecx, 0x84
004838c1 e81a5afaff               call       0x4292e0
004838c6 51                       push       ecx
004838c7 d91c24                   fstp       dword ptr [esp]
004838ca 8b55fc                   mov        edx, dword ptr [ebp - 4]
004838cd 83c270                   add        edx, 0x70
004838d0 52                       push       edx
004838d1 e85a5afbff               call       0x439330
004838d6 83c40c                   add        esp, 0xc
004838d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004838dc 05a0000000               add        eax, 0xa0
004838e1 894594                   mov        dword ptr [ebp - 0x6c], eax
004838e4 6a03                     push       3
004838e6 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004838e9 e8928dffff               call       0x47c680
004838ee 894590                   mov        dword ptr [ebp - 0x70], eax
004838f1 6a00                     push       0
004838f3 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004838f6 e845fcf9ff               call       0x423540
004838fb 33c0                     xor        eax, eax
004838fd 5e                       pop        esi
004838fe 8be5                     mov        esp, ebp
00483900 5d                       pop        ebp
00483901 c3                       ret        
00483902 6690                     nop        
00483904 4a                       dec        edx
00483905 3648                     dec        eax
00483907 007e36                   add        byte ptr [esi + 0x36], bh
0048390a 48                       dec        eax
0048390b 00e5                     add        ch, ah
0048390d 3648                     dec        eax
0048390f 0017                     add        byte ptr [edi], dl
00483911 37                       aaa        
00483912 48                       dec        eax
00483913 004937                   add        byte ptr [ecx + 0x37], cl
00483916 48                       dec        eax
00483917 004a36                   add        byte ptr [edx + 0x36], cl
0048391a 48                       dec        eax
0048391b 007e36                   add        byte ptr [esi + 0x36], bh
0048391e 48                       dec        eax
