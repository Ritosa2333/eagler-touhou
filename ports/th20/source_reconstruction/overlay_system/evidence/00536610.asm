00536610 55                       push       ebp
00536611 8bec                     mov        ebp, esp
00536613 83ec7c                   sub        esp, 0x7c
00536616 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053661b 33c5                     xor        eax, ebp
0053661d 8945fc                   mov        dword ptr [ebp - 4], eax
00536620 894db0                   mov        dword ptr [ebp - 0x50], ecx
00536623 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
0053662a eb09                     jmp        0x536635
0053662c 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
0053662f 83c001                   add        eax, 1
00536632 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00536635 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00536638 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
0053663b 0f8d7c010000             jge        0x5367bd
00536641 6a00                     push       0
00536643 e8e8a1f2ff               call       0x460830
00536648 83c404                   add        esp, 4
0053664b 8945a8                   mov        dword ptr [ebp - 0x58], eax
0053664e 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00536651 52                       push       edx
00536652 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00536655 e866f1ffff               call       0x5357c0
0053665a 8bc8                     mov        ecx, eax
0053665c e80fb3eeff               call       0x421970
00536661 8b08                     mov        ecx, dword ptr [eax]
00536663 894df0                   mov        dword ptr [ebp - 0x10], ecx
00536666 8b5004                   mov        edx, dword ptr [eax + 4]
00536669 8955f4                   mov        dword ptr [ebp - 0xc], edx
0053666c 8b4008                   mov        eax, dword ptr [eax + 8]
0053666f 8945f8                   mov        dword ptr [ebp - 8], eax
00536672 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00536675 51                       push       ecx
00536676 b984695c00               mov        ecx, 0x5c6984
0053667b e800dfedff               call       0x414580
00536680 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00536683 6a00                     push       0
00536685 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00536688 e8a32df1ff               call       0x449430
0053668d 0fb6d0                   movzx      edx, al
00536690 85d2                     test       edx, edx
00536692 0f8499000000             je         0x536731
00536698 8d4db8                   lea        ecx, [ebp - 0x48]
0053669b e89053f4ff               call       0x47ba30
005366a0 6a00                     push       0
005366a2 e889a1f2ff               call       0x460830
005366a7 83c404                   add        esp, 4
005366aa 8bc8                     mov        ecx, eax
005366ac e89fa1f2ff               call       0x460850
005366b1 8b08                     mov        ecx, dword ptr [eax]
005366b3 894db8                   mov        dword ptr [ebp - 0x48], ecx
005366b6 8b5004                   mov        edx, dword ptr [eax + 4]
005366b9 8955bc                   mov        dword ptr [ebp - 0x44], edx
005366bc 8b4008                   mov        eax, dword ptr [eax + 8]
005366bf 8945c0                   mov        dword ptr [ebp - 0x40], eax
005366c2 6a00                     push       0
005366c4 e8570ef0ff               call       0x437520
005366c9 83c404                   add        esp, 4
005366cc 8945a0                   mov        dword ptr [ebp - 0x60], eax
005366cf 6a00                     push       0
005366d1 8d4db8                   lea        ecx, [ebp - 0x48]
005366d4 51                       push       ecx
005366d5 6a08                     push       8
005366d7 8d5584                   lea        edx, [ebp - 0x7c]
005366da 52                       push       edx
005366db 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
005366de e88d74f6ff               call       0x49db70
005366e3 8b00                     mov        eax, dword ptr [eax]
005366e5 89459c                   mov        dword ptr [ebp - 0x64], eax
005366e8 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005366eb 51                       push       ecx
005366ec b984695c00               mov        ecx, 0x5c6984
005366f1 e88adeedff               call       0x414580
005366f6 8b559c                   mov        edx, dword ptr [ebp - 0x64]
005366f9 8910                     mov        dword ptr [eax], edx
005366fb 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
005366fe 50                       push       eax
005366ff b984695c00               mov        ecx, 0x5c6984
00536704 e877deedff               call       0x414580
00536709 894598                   mov        dword ptr [ebp - 0x68], eax
0053670c 51                       push       ecx
0053670d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00536715 f30f110424               movss      dword ptr [esp], xmm0
0053671a 51                       push       ecx
0053671b f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00536723 f30f110424               movss      dword ptr [esp], xmm0
00536728 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0053672b e8809ff1ff               call       0x4506b0
00536730 90                       nop        
00536731 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00536734 51                       push       ecx
00536735 b984695c00               mov        ecx, 0x5c6984
0053673a e841deedff               call       0x414580
0053673f 894594                   mov        dword ptr [ebp - 0x6c], eax
00536742 8d55f0                   lea        edx, [ebp - 0x10]
00536745 52                       push       edx
00536746 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00536749 e892def2ff               call       0x4645e0
0053674e 90                       nop        
0053674f 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00536752 83785800                 cmp        dword ptr [eax + 0x58], 0
00536756 740f                     je         0x536767
00536758 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0053675b 8b5158                   mov        edx, dword ptr [ecx + 0x58]
0053675e 83ea01                   sub        edx, 1
00536761 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00536764 895058                   mov        dword ptr [eax + 0x58], edx
00536767 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0053676a 83795800                 cmp        dword ptr [ecx + 0x58], 0
0053676e 7548                     jne        0x5367b8
00536770 6a00                     push       0
00536772 e80927f4ff               call       0x478e80
00536777 83c404                   add        esp, 4
0053677a 894590                   mov        dword ptr [ebp - 0x70], eax
0053677d 6a00                     push       0
0053677f 689f860100               push       0x1869f
00536784 6a00                     push       0
00536786 51                       push       ecx
00536787 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0053678f f30f110424               movss      dword ptr [esp], xmm0
00536794 8d55f0                   lea        edx, [ebp - 0x10]
00536797 52                       push       edx
00536798 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0053679b e8c067f4ff               call       0x47cf60
005367a0 894588                   mov        dword ptr [ebp - 0x78], eax
005367a3 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005367a6 8b4858                   mov        ecx, dword ptr [eax + 0x58]
005367a9 894d8c                   mov        dword ptr [ebp - 0x74], ecx
005367ac 8b558c                   mov        edx, dword ptr [ebp - 0x74]
005367af 035588                   add        edx, dword ptr [ebp - 0x78]
005367b2 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005367b5 895058                   mov        dword ptr [eax + 0x58], edx
005367b8 e96ffeffff               jmp        0x53662c
005367bd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005367c0 894dac                   mov        dword ptr [ebp - 0x54], ecx
005367c3 eb09                     jmp        0x5367ce
005367c5 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
005367c8 83c201                   add        edx, 1
005367cb 8955ac                   mov        dword ptr [ebp - 0x54], edx
005367ce 837dac04                 cmp        dword ptr [ebp - 0x54], 4
005367d2 7d2d                     jge        0x536801
005367d4 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
005367d7 50                       push       eax
005367d8 b984695c00               mov        ecx, 0x5c6984
005367dd e89eddedff               call       0x414580
005367e2 8bc8                     mov        ecx, eax
005367e4 e8e794f1ff               call       0x44fcd0
005367e9 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005367ec 51                       push       ecx
005367ed b998695c00               mov        ecx, 0x5c6998
005367f2 e889ddedff               call       0x414580
005367f7 8bc8                     mov        ecx, eax
005367f9 e802b7f8ff               call       0x4c1f00
005367fe 90                       nop        
005367ff ebc4                     jmp        0x5367c5
00536801 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536804 33cd                     xor        ecx, ebp
00536806 e8e3c10000               call       0x5429ee
0053680b 8be5                     mov        esp, ebp
0053680d 5d                       pop        ebp
0053680e c20400                   ret        4
00536811 cc                       int3       
00536812 cc                       int3       
00536813 cc                       int3       
00536814 cc                       int3       
00536815 cc                       int3       
00536816 cc                       int3       
00536817 cc                       int3       
00536818 cc                       int3       
00536819 cc                       int3       
0053681a cc                       int3       
0053681b cc                       int3       
0053681c cc                       int3       
0053681d cc                       int3       
0053681e cc                       int3       
0053681f cc                       int3       
