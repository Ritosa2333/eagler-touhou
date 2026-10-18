00537730 55                       push       ebp
00537731 8bec                     mov        ebp, esp
00537733 81ec98000000             sub        esp, 0x98
00537739 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053773e 33c5                     xor        eax, ebp
00537740 8945fc                   mov        dword ptr [ebp - 4], eax
00537743 56                       push       esi
00537744 894db0                   mov        dword ptr [ebp - 0x50], ecx
00537747 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0053774a 83c014                   add        eax, 0x14
0053774d 8945a8                   mov        dword ptr [ebp - 0x58], eax
00537750 6a00                     push       0
00537752 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00537755 e836beeeff               call       0x423590
0053775a 0fb6c8                   movzx      ecx, al
0053775d 85c9                     test       ecx, ecx
0053775f 740f                     je         0x537770
00537761 b801000000               mov        eax, 1
00537766 e98c020000               jmp        0x5379f7
0053776b e96c020000               jmp        0x5379dc
00537770 6a00                     push       0
00537772 e809c9f2ff               call       0x464080
00537777 83c404                   add        esp, 4
0053777a 8945a0                   mov        dword ptr [ebp - 0x60], eax
0053777d 6a00                     push       0
0053777f e8fcc8f2ff               call       0x464080
00537784 83c404                   add        esp, 4
00537787 8bc8                     mov        ecx, eax
00537789 e89241feff               call       0x51b920
0053778e 8bf0                     mov        esi, eax
00537790 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00537793 83c114                   add        ecx, 0x14
00537796 e8f587edff               call       0x40ff90
0053779b 0faff0                   imul       esi, eax
0053779e e8bdcaffff               call       0x534260
005377a3 8bc8                     mov        ecx, eax
005377a5 8bc6                     mov        eax, esi
005377a7 99                       cdq        
005377a8 f7f9                     idiv       ecx
005377aa 8945a4                   mov        dword ptr [ebp - 0x5c], eax
005377ad 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
005377b0 52                       push       edx
005377b1 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
005377b4 e88771f8ff               call       0x4be940
005377b9 90                       nop        
005377ba c745ac00000000           mov        dword ptr [ebp - 0x54], 0
005377c1 eb09                     jmp        0x5377cc
005377c3 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
005377c6 83c001                   add        eax, 1
005377c9 8945ac                   mov        dword ptr [ebp - 0x54], eax
005377cc 6a00                     push       0
005377ce e8adc8f2ff               call       0x464080
005377d3 83c404                   add        esp, 4
005377d6 8bc8                     mov        ecx, eax
005377d8 e8c309f8ff               call       0x4b81a0
005377dd 3945ac                   cmp        dword ptr [ebp - 0x54], eax
005377e0 0f8df6010000             jge        0x5379dc
005377e6 6a00                     push       0
005377e8 e84390f2ff               call       0x460830
005377ed 83c404                   add        esp, 4
005377f0 89459c                   mov        dword ptr [ebp - 0x64], eax
005377f3 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005377f6 51                       push       ecx
005377f7 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005377fa e8c1dfffff               call       0x5357c0
005377ff 8bc8                     mov        ecx, eax
00537801 e86aa1eeff               call       0x421970
00537806 8b10                     mov        edx, dword ptr [eax]
00537808 8955ec                   mov        dword ptr [ebp - 0x14], edx
0053780b 8b4804                   mov        ecx, dword ptr [eax + 4]
0053780e 894df0                   mov        dword ptr [ebp - 0x10], ecx
00537811 8b5008                   mov        edx, dword ptr [eax + 8]
00537814 8955f4                   mov        dword ptr [ebp - 0xc], edx
00537817 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
0053781a 50                       push       eax
0053781b b9a8695c00               mov        ecx, 0x5c69a8
00537820 e85bcdedff               call       0x414580
00537825 894598                   mov        dword ptr [ebp - 0x68], eax
00537828 6a00                     push       0
0053782a 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0053782d e8fe1bf1ff               call       0x449430
00537832 0fb6c8                   movzx      ecx, al
00537835 85c9                     test       ecx, ecx
00537837 0f849c000000             je         0x5378d9
0053783d 8d4db4                   lea        ecx, [ebp - 0x4c]
00537840 e8eb41f4ff               call       0x47ba30
00537845 6a00                     push       0
00537847 e8e48ff2ff               call       0x460830
0053784c 83c404                   add        esp, 4
0053784f 8bc8                     mov        ecx, eax
00537851 e8fa8ff2ff               call       0x460850
00537856 8b10                     mov        edx, dword ptr [eax]
00537858 8955b4                   mov        dword ptr [ebp - 0x4c], edx
0053785b 8b4804                   mov        ecx, dword ptr [eax + 4]
0053785e 894db8                   mov        dword ptr [ebp - 0x48], ecx
00537861 8b5008                   mov        edx, dword ptr [eax + 8]
00537864 8955bc                   mov        dword ptr [ebp - 0x44], edx
00537867 6a00                     push       0
00537869 e8b2fcefff               call       0x437520
0053786e 83c404                   add        esp, 4
00537871 894594                   mov        dword ptr [ebp - 0x6c], eax
00537874 6a00                     push       0
00537876 8d45b4                   lea        eax, [ebp - 0x4c]
00537879 50                       push       eax
0053787a 6a08                     push       8
0053787c 8d8d6cffffff             lea        ecx, [ebp - 0x94]
00537882 51                       push       ecx
00537883 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00537886 e8e562f6ff               call       0x49db70
0053788b 8b10                     mov        edx, dword ptr [eax]
0053788d 895590                   mov        dword ptr [ebp - 0x70], edx
00537890 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00537893 50                       push       eax
00537894 b9a8695c00               mov        ecx, 0x5c69a8
00537899 e8e2ccedff               call       0x414580
0053789e 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005378a1 8908                     mov        dword ptr [eax], ecx
005378a3 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
005378a6 52                       push       edx
005378a7 b9a8695c00               mov        ecx, 0x5c69a8
005378ac e8cfccedff               call       0x414580
005378b1 89458c                   mov        dword ptr [ebp - 0x74], eax
005378b4 51                       push       ecx
005378b5 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005378bd f30f110424               movss      dword ptr [esp], xmm0
005378c2 51                       push       ecx
005378c3 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
005378cb f30f110424               movss      dword ptr [esp], xmm0
005378d0 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
005378d3 e8d88df1ff               call       0x4506b0
005378d8 90                       nop        
005378d9 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
005378dc 50                       push       eax
005378dd b9a8695c00               mov        ecx, 0x5c69a8
005378e2 e899ccedff               call       0x414580
005378e7 894588                   mov        dword ptr [ebp - 0x78], eax
005378ea 8d4dec                   lea        ecx, [ebp - 0x14]
005378ed 51                       push       ecx
005378ee 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005378f1 e8eaccf2ff               call       0x4645e0
005378f6 6a04                     push       4
005378f8 8d4df8                   lea        ecx, [ebp - 8]
005378fb e88047edff               call       0x40c080
00537900 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00537903 52                       push       edx
00537904 8d4df8                   lea        ecx, [ebp - 8]
00537907 e894c9edff               call       0x4142a0
0053790c 90                       nop        
0053790d 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537910 83785800                 cmp        dword ptr [eax + 0x58], 0
00537914 740f                     je         0x537925
00537916 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00537919 8b5158                   mov        edx, dword ptr [ecx + 0x58]
0053791c 83ea01                   sub        edx, 1
0053791f 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00537922 895058                   mov        dword ptr [eax + 0x58], edx
00537925 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00537928 83795800                 cmp        dword ptr [ecx + 0x58], 0
0053792c 0f85a5000000             jne        0x5379d7
00537932 6a00                     push       0
00537934 e84715f4ff               call       0x478e80
00537939 83c404                   add        esp, 4
0053793c 894584                   mov        dword ptr [ebp - 0x7c], eax
0053793f 83ec28                   sub        esp, 0x28
00537942 8bcc                     mov        ecx, esp
00537944 89a568ffffff             mov        dword ptr [ebp - 0x98], esp
0053794a 8d55f8                   lea        edx, [ebp - 8]
0053794d 52                       push       edx
0053794e e80df5ffff               call       0x536e60
00537953 51                       push       ecx
00537954 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
0053795c f30f110424               movss      dword ptr [esp], xmm0
00537961 8d45ec                   lea        eax, [ebp - 0x14]
00537964 50                       push       eax
00537965 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00537968 e8d358f4ff               call       0x47d240
0053796d 894580                   mov        dword ptr [ebp - 0x80], eax
00537970 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00537973 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
00537979 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
0053797c 8b4258                   mov        eax, dword ptr [edx + 0x58]
0053797f 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00537985 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0053798b 038d78ffffff             add        ecx, dword ptr [ebp - 0x88]
00537991 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00537994 894a58                   mov        dword ptr [edx + 0x58], ecx
00537997 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0053799a 8b4858                   mov        ecx, dword ptr [eax + 0x58]
0053799d d1e1                     shl        ecx, 1
0053799f 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005379a2 894a58                   mov        dword ptr [edx + 0x58], ecx
005379a5 6a00                     push       0
005379a7 e83415f4ff               call       0x478ee0
005379ac 83c404                   add        esp, 4
005379af 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
005379b5 6a01                     push       1
005379b7 6a00                     push       0
005379b9 51                       push       ecx
005379ba f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
005379c2 f30f110424               movss      dword ptr [esp], xmm0
005379c7 8d45ec                   lea        eax, [ebp - 0x14]
005379ca 50                       push       eax
005379cb 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
005379d1 e8fa30f9ff               call       0x4caad0
005379d6 90                       nop        
005379d7 e9e7fdffff               jmp        0x5377c3
005379dc 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005379df 83c114                   add        ecx, 0x14
005379e2 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
005379e8 6a00                     push       0
005379ea 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
005379f0 e82b1aefff               call       0x429420
005379f5 33c0                     xor        eax, eax
005379f7 5e                       pop        esi
005379f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005379fb 33cd                     xor        ecx, ebp
005379fd e8ecaf0000               call       0x5429ee
00537a02 8be5                     mov        esp, ebp
00537a04 5d                       pop        ebp
00537a05 c3                       ret        
00537a06 cc                       int3       
00537a07 cc                       int3       
00537a08 cc                       int3       
00537a09 cc                       int3       
00537a0a cc                       int3       
00537a0b cc                       int3       
00537a0c cc                       int3       
00537a0d cc                       int3       
00537a0e cc                       int3       
00537a0f cc                       int3       
