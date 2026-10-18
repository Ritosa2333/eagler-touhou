004e5620 55                       push       ebp
004e5621 8bec                     mov        ebp, esp
004e5623 83ec50                   sub        esp, 0x50
004e5626 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e562b 33c5                     xor        eax, ebp
004e562d 8945fc                   mov        dword ptr [ebp - 4], eax
004e5630 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004e5633 51                       push       ecx
004e5634 0f57c0                   xorps      xmm0, xmm0
004e5637 f30f110424               movss      dword ptr [esp], xmm0
004e563c 51                       push       ecx
004e563d f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
004e5645 f30f110424               movss      dword ptr [esp], xmm0
004e564a 51                       push       ecx
004e564b f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
004e5653 f30f110424               movss      dword ptr [esp], xmm0
004e5658 8d4df0                   lea        ecx, [ebp - 0x10]
004e565b e870d7f3ff               call       0x422dd0
004e5660 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004e5663 83c130                   add        ecx, 0x30
004e5666 e8956cf2ff               call       0x40c300
004e566b 8945bc                   mov        dword ptr [ebp - 0x44], eax
004e566e f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
004e5676 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e567b 6898255700               push       0x572598
004e5680 8d45f0                   lea        eax, [ebp - 0x10]
004e5683 50                       push       eax
004e5684 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e568a 51                       push       ecx
004e568b e80073f8ff               call       0x46c990
004e5690 83c40c                   add        esp, 0xc
004e5693 f30f2a45bc               cvtsi2ss   xmm0, dword ptr [ebp - 0x44]
004e5698 f30f590544265700         mulss      xmm0, dword ptr [0x572644]
004e56a0 f30f58057cfe5600         addss      xmm0, dword ptr [0x56fe7c]
004e56a8 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e56ad f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004e56b2 f30f580548265700         addss      xmm0, dword ptr [0x572648]
004e56ba f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004e56bf 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004e56c2 83bae400000000           cmp        dword ptr [edx + 0xe4], 0
004e56c9 7521                     jne        0x4e56ec
004e56cb 83ec0c                   sub        esp, 0xc
004e56ce 8bc4                     mov        eax, esp
004e56d0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e56d3 8908                     mov        dword ptr [eax], ecx
004e56d5 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004e56d8 895004                   mov        dword ptr [eax + 4], edx
004e56db 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e56de 894808                   mov        dword ptr [eax + 8], ecx
004e56e1 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004e56e4 e837f9ffff               call       0x4e5020
004e56e9 90                       nop        
004e56ea eb07                     jmp        0x4e56f3
004e56ec c745bcffffffff           mov        dword ptr [ebp - 0x44], 0xffffffff
004e56f3 f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
004e56fb f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004e5700 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
004e5708 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e570d 33d2                     xor        edx, edx
004e570f 8955cc                   mov        dword ptr [ebp - 0x34], edx
004e5712 8955d0                   mov        dword ptr [ebp - 0x30], edx
004e5715 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004e5718 8955d8                   mov        dword ptr [ebp - 0x28], edx
004e571b 8955dc                   mov        dword ptr [ebp - 0x24], edx
004e571e 8955e0                   mov        dword ptr [ebp - 0x20], edx
004e5721 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004e5724 8955e8                   mov        dword ptr [ebp - 0x18], edx
004e5727 8955ec                   mov        dword ptr [ebp - 0x14], edx
004e572a c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004e5731 eb09                     jmp        0x4e573c
004e5733 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004e5736 83c001                   add        eax, 1
004e5739 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004e573c 837dc40a                 cmp        dword ptr [ebp - 0x3c], 0xa
004e5740 0f8d35010000             jge        0x4e587b
004e5746 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004e5749 3b4dbc                   cmp        ecx, dword ptr [ebp - 0x44]
004e574c 7513                     jne        0x4e5761
004e574e 6800ffffff               push       0xffffff00
004e5753 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5759 e872b3f8ff               call       0x470ad0
004e575e 90                       nop        
004e575f eb11                     jmp        0x4e5772
004e5761 68808080ff               push       0xff808080
004e5766 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e576c e85fb3f8ff               call       0x470ad0
004e5771 90                       nop        
004e5772 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004e5778 e8d3a40200               call       0x50fc50
004e577d 83c018                   add        eax, 0x18
004e5780 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004e5783 b968a55b00               mov        ecx, 0x5ba568
004e5788 e813e9f7ff               call       0x4640a0
004e578d 69d090010000             imul       edx, eax, 0x190
004e5793 8955b0                   mov        dword ptr [ebp - 0x50], edx
004e5796 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
004e5799 0345b0                   add        eax, dword ptr [ebp - 0x50]
004e579c 6b4dc428                 imul       ecx, dword ptr [ebp - 0x3c], 0x28
004e57a0 03c1                     add        eax, ecx
004e57a2 8945c8                   mov        dword ptr [ebp - 0x38], eax
004e57a5 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004e57a8 8955b8                   mov        dword ptr [ebp - 0x48], edx
004e57ab 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004e57ae 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004e57b1 8b5018                   mov        edx, dword ptr [eax + 0x18]
004e57b4 0b511c                   or         edx, dword ptr [ecx + 0x1c]
004e57b7 7473                     je         0x4e582c
004e57b9 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004e57bc 83c018                   add        eax, 0x18
004e57bf 50                       push       eax
004e57c0 8d4dcc                   lea        ecx, [ebp - 0x34]
004e57c3 51                       push       ecx
004e57c4 e8a76afcff               call       0x4ac270
004e57c9 83c408                   add        esp, 8
004e57cc 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004e57cf 0fbe4208                 movsx      eax, byte ptr [edx + 8]
004e57d3 8b0c85a8ff5a00           mov        ecx, dword ptr [eax*4 + 0x5affa8]
004e57da 51                       push       ecx
004e57db 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004e57de 52                       push       edx
004e57df 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004e57e2 83c001                   add        eax, 1
004e57e5 50                       push       eax
004e57e6 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e57e9 99                       cdq        
004e57ea b964000000               mov        ecx, 0x64
004e57ef f7f9                     idiv       ecx
004e57f1 52                       push       edx
004e57f2 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004e57f5 0fbe4209                 movsx      eax, byte ptr [edx + 9]
004e57f9 50                       push       eax
004e57fa 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e57fd 8b5104                   mov        edx, dword ptr [ecx + 4]
004e5800 52                       push       edx
004e5801 8b01                     mov        eax, dword ptr [ecx]
004e5803 50                       push       eax
004e5804 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e5807 83c10a                   add        ecx, 0xa
004e580a 51                       push       ecx
004e580b 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004e580e 83c201                   add        edx, 1
004e5811 52                       push       edx
004e5812 68b4255700               push       0x5725b4
004e5817 8d45f0                   lea        eax, [ebp - 0x10]
004e581a 50                       push       eax
004e581b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5821 51                       push       ecx
004e5822 e86971f8ff               call       0x46c990
004e5827 83c430                   add        esp, 0x30
004e582a eb38                     jmp        0x4e5864
004e582c 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004e582f 0fbe4209                 movsx      eax, byte ptr [edx + 9]
004e5833 50                       push       eax
004e5834 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e5837 8b5104                   mov        edx, dword ptr [ecx + 4]
004e583a 52                       push       edx
004e583b 8b01                     mov        eax, dword ptr [ecx]
004e583d 50                       push       eax
004e583e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e5841 83c10a                   add        ecx, 0xa
004e5844 51                       push       ecx
004e5845 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004e5848 83c201                   add        edx, 1
004e584b 52                       push       edx
004e584c 68d8255700               push       0x5725d8
004e5851 8d45f0                   lea        eax, [ebp - 0x10]
004e5854 50                       push       eax
004e5855 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e585b 51                       push       ecx
004e585c e82f71f8ff               call       0x46c990
004e5861 83c420                   add        esp, 0x20
004e5864 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004e5869 f30f580544265700         addss      xmm0, dword ptr [0x572644]
004e5871 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e5876 e9b8feffff               jmp        0x4e5733
004e587b 6aff                     push       -1
004e587d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5883 e848b2f8ff               call       0x470ad0
004e5888 90                       nop        
004e5889 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e588c 33cd                     xor        ecx, ebp
004e588e e85bd10500               call       0x5429ee
004e5893 8be5                     mov        esp, ebp
004e5895 5d                       pop        ebp
004e5896 c3                       ret        
004e5897 cc                       int3       
004e5898 cc                       int3       
004e5899 cc                       int3       
004e589a cc                       int3       
004e589b cc                       int3       
004e589c cc                       int3       
004e589d cc                       int3       
004e589e cc                       int3       
004e589f cc                       int3       
