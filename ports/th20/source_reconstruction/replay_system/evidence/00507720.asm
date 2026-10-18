00507720 55                       push       ebp
00507721 8bec                     mov        ebp, esp
00507723 83ec10                   sub        esp, 0x10
00507726 894dfc                   mov        dword ptr [ebp - 4], ecx
00507729 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050772c e89ffbffff               call       0x5072d0
00507731 68a08c0000               push       0x8ca0
00507736 6a00                     push       0
00507738 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050773b 05c04b0300               add        eax, 0x34bc0
00507740 50                       push       eax
00507741 e81ad00300               call       0x544760
00507746 83c40c                   add        esp, 0xc
00507749 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050774c c78160d8030000000000     mov        dword ptr [ecx + 0x3d860], 0
00507756 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507759 c78264d8030000000000     mov        dword ptr [edx + 0x3d864], 0
00507763 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507766 81c168d80300             add        ecx, 0x3d868
0050776c e89f12f1ff               call       0x418a10
00507771 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507774 8945f8                   mov        dword ptr [ebp - 8], eax
00507777 6a00                     push       0
00507779 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050777c e84f030000               call       0x507ad0
00507781 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507784 898160d80300             mov        dword ptr [ecx + 0x3d860], eax
0050778a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050778d 81c2c04b0300             add        edx, 0x34bc0
00507793 8955f4                   mov        dword ptr [ebp - 0xc], edx
00507796 6a00                     push       0
00507798 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050779b e870a4f0ff               call       0x411c10
005077a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005077a3 898164d80300             mov        dword ptr [ecx + 0x3d864], eax
005077a9 8b55fc                   mov        edx, dword ptr [ebp - 4]
005077ac 81c268d80300             add        edx, 0x3d868
005077b2 8955f0                   mov        dword ptr [ebp - 0x10], edx
005077b5 8b45fc                   mov        eax, dword ptr [ebp - 4]
005077b8 50                       push       eax
005077b9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005077bc e8af2ef4ff               call       0x44a670
005077c1 90                       nop        
005077c2 8b45fc                   mov        eax, dword ptr [ebp - 4]
005077c5 8be5                     mov        esp, ebp
005077c7 5d                       pop        ebp
005077c8 c3                       ret        
005077c9 cc                       int3       
005077ca cc                       int3       
005077cb cc                       int3       
005077cc cc                       int3       
005077cd cc                       int3       
005077ce cc                       int3       
005077cf cc                       int3       
