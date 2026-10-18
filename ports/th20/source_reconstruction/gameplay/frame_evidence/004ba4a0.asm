004ba4a0 55                       push       ebp
004ba4a1 8bec                     mov        ebp, esp
004ba4a3 83ec34                   sub        esp, 0x34
004ba4a6 894dfc                   mov        dword ptr [ebp - 4], ecx
004ba4a9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba4ac 8b4830                   mov        ecx, dword ptr [eax + 0x30]
004ba4af 83c101                   add        ecx, 1
004ba4b2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba4b5 894a30                   mov        dword ptr [edx + 0x30], ecx
004ba4b8 e883eafdff               call       0x498f40
004ba4bd 8945f4                   mov        dword ptr [ebp - 0xc], eax
004ba4c0 6a01                     push       1
004ba4c2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ba4c5 e886280000               call       0x4bcd50
004ba4ca 90                       nop        
004ba4cb a188585c00               mov        eax, dword ptr [0x5c5888]
004ba4d0 c1e805                   shr        eax, 5
004ba4d3 83e003                   and        eax, 3
004ba4d6 7407                     je         0x4ba4df
004ba4d8 33c0                     xor        eax, eax
004ba4da e989030000               jmp        0x4ba868
004ba4df 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba4e2 e8890dffff               call       0x4ab270
004ba4e7 0fb6c8                   movzx      ecx, al
004ba4ea 85c9                     test       ecx, ecx
004ba4ec 0f84c9000000             je         0x4ba5bb
004ba4f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba4f5 8b82e8000000             mov        eax, dword ptr [edx + 0xe8]
004ba4fb c1e804                   shr        eax, 4
004ba4fe 83e001                   and        eax, 1
004ba501 740a                     je         0x4ba50d
004ba503 b803000000               mov        eax, 3
004ba508 e95b030000               jmp        0x4ba868
004ba50d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba510 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004ba516 83c201                   add        edx, 1
004ba519 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba51c 89900c010000             mov        dword ptr [eax + 0x10c], edx
004ba522 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba525 e806e3fcff               call       0x488830
004ba52a 85c0                     test       eax, eax
004ba52c 7418                     je         0x4ba546
004ba52e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba531 83b90c01000078           cmp        dword ptr [ecx + 0x10c], 0x78
004ba538 750c                     jne        0x4ba546
004ba53a 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004ba540 e8ebb90200               call       0x4e5f30
004ba545 90                       nop        
004ba546 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba549 e8e2e2fcff               call       0x488830
004ba54e 85c0                     test       eax, eax
004ba550 7523                     jne        0x4ba575
004ba552 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba555 81ba0c010000b4000000     cmp        dword ptr [edx + 0x10c], 0xb4
004ba55f 7514                     jne        0x4ba575
004ba561 6a67                     push       0x67
004ba563 6a00                     push       0
004ba565 6a00                     push       0
004ba567 6a00                     push       0
004ba569 6a3c                     push       0x3c
004ba56b 6a05                     push       5
004ba56d e84eabf6ff               call       0x4250c0
004ba572 83c418                   add        esp, 0x18
004ba575 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba578 e8b3e2fcff               call       0x488830
004ba57d 85c0                     test       eax, eax
004ba57f 753a                     jne        0x4ba5bb
004ba581 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba584 81b80c010000f0000000     cmp        dword ptr [eax + 0x10c], 0xf0
004ba58e 7c2b                     jl         0x4ba5bb
004ba590 b968a55b00               mov        ecx, 0x5ba568
004ba595 e8069bfaff               call       0x4640a0
004ba59a 83f804                   cmp        eax, 4
004ba59d 740f                     je         0x4ba5ae
004ba59f 6a0f                     push       0xf
004ba5a1 b9404d5c00               mov        ecx, 0x5c4d40
004ba5a6 e8056afeff               call       0x4a0fb0
004ba5ab 90                       nop        
004ba5ac eb0d                     jmp        0x4ba5bb
004ba5ae 6a10                     push       0x10
004ba5b0 b9404d5c00               mov        ecx, 0x5c4d40
004ba5b5 e8f669feff               call       0x4a0fb0
004ba5ba 90                       nop        
004ba5bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba5be 83c110                   add        ecx, 0x10
004ba5c1 894df0                   mov        dword ptr [ebp - 0x10], ecx
004ba5c4 6a00                     push       0
004ba5c6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ba5c9 e812e0fbff               call       0x4785e0
004ba5ce 0fb6d0                   movzx      edx, al
004ba5d1 85d2                     test       edx, edx
004ba5d3 7418                     je         0x4ba5ed
004ba5d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba5d8 e863030000               call       0x4ba940
004ba5dd 85c0                     test       eax, eax
004ba5df 740a                     je         0x4ba5eb
004ba5e1 b801000000               mov        eax, 1
004ba5e6 e97d020000               jmp        0x4ba868
004ba5eb eb23                     jmp        0x4ba610
004ba5ed 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba5f0 83c010                   add        eax, 0x10
004ba5f3 8945ec                   mov        dword ptr [ebp - 0x14], eax
004ba5f6 6a1e                     push       0x1e
004ba5f8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004ba5fb e8e0dffbff               call       0x4785e0
004ba600 0fb6c8                   movzx      ecx, al
004ba603 85c9                     test       ecx, ecx
004ba605 7409                     je         0x4ba610
004ba607 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba60a e881190000               call       0x4bbf90
004ba60f 90                       nop        
004ba610 833da0065c0000           cmp        dword ptr [0x5c06a0], 0
004ba617 741e                     je         0x4ba637
004ba619 8b0da0065c00             mov        ecx, dword ptr [0x5c06a0]
004ba61f e8ec300000               call       0x4bd710
004ba624 85c0                     test       eax, eax
004ba626 740f                     je         0x4ba637
004ba628 8b15a0065c00             mov        edx, dword ptr [0x5c06a0]
004ba62e 52                       push       edx
004ba62f e88c71f6ff               call       0x4217c0
004ba634 83c404                   add        esp, 4
004ba637 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba63a 83c010                   add        eax, 0x10
004ba63d 8945e8                   mov        dword ptr [ebp - 0x18], eax
004ba640 6a05                     push       5
004ba642 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004ba645 e896dffbff               call       0x4785e0
004ba64a 0fb6c8                   movzx      ecx, al
004ba64d 85c9                     test       ecx, ecx
004ba64f 740a                     je         0x4ba65b
004ba651 c70558885b0002000000     mov        dword ptr [0x5b8858], 2
004ba65b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba65e 8b82e8000000             mov        eax, dword ptr [edx + 0xe8]
004ba664 c1e802                   shr        eax, 2
004ba667 83e001                   and        eax, 1
004ba66a 7422                     je         0x4ba68e
004ba66c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba66f 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
004ba675 81ca00010000             or         edx, 0x100
004ba67b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba67e 8990e8000000             mov        dword ptr [eax + 0xe8], edx
004ba684 b801000000               mov        eax, 1
004ba689 e9da010000               jmp        0x4ba868
004ba68e 8b0dd4a55b00             mov        ecx, dword ptr [0x5ba5d4]
004ba694 c1e905                   shr        ecx, 5
004ba697 83e101                   and        ecx, 1
004ba69a 0f84e2000000             je         0x4ba782
004ba6a0 33d2                     xor        edx, edx
004ba6a2 7534                     jne        0x4ba6d8
004ba6a4 b804000000               mov        eax, 4
004ba6a9 6bc800                   imul       ecx, eax, 0
004ba6ac 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004ba6b3 7423                     je         0x4ba6d8
004ba6b5 ba04000000               mov        edx, 4
004ba6ba 6bc200                   imul       eax, edx, 0
004ba6bd 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004ba6c3 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004ba6c6 680f010800               push       0x8010f
004ba6cb 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004ba6ce e8cdeef8ff               call       0x4495a0
004ba6d3 8945f8                   mov        dword ptr [ebp - 8], eax
004ba6d6 eb07                     jmp        0x4ba6df
004ba6d8 c745f800000000           mov        dword ptr [ebp - 8], 0
004ba6df 837df800                 cmp        dword ptr [ebp - 8], 0
004ba6e3 7533                     jne        0x4ba718
004ba6e5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba6e8 8b82e8000000             mov        eax, dword ptr [edx + 0xe8]
004ba6ee c1e804                   shr        eax, 4
004ba6f1 83e001                   and        eax, 1
004ba6f4 7522                     jne        0x4ba718
004ba6f6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba6f9 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
004ba6ff c1ea05                   shr        edx, 5
004ba702 83e201                   and        edx, 1
004ba705 7511                     jne        0x4ba718
004ba707 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba70a 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba710 c1e906                   shr        ecx, 6
004ba713 83e101                   and        ecx, 1
004ba716 740d                     je         0x4ba725
004ba718 6a04                     push       4
004ba71a b9404d5c00               mov        ecx, 0x5c4d40
004ba71f e88c68feff               call       0x4a0fb0
004ba724 90                       nop        
004ba725 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba728 83c210                   add        edx, 0x10
004ba72b 8955e0                   mov        dword ptr [ebp - 0x20], edx
004ba72e 68d40d0000               push       0xdd4
004ba733 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004ba736 e8a5defbff               call       0x4785e0
004ba73b 0fb6c0                   movzx      eax, al
004ba73e 85c0                     test       eax, eax
004ba740 7416                     je         0x4ba758
004ba742 6a6d                     push       0x6d
004ba744 6a00                     push       0
004ba746 6a00                     push       0
004ba748 6a00                     push       0
004ba74a 6a3c                     push       0x3c
004ba74c 6a05                     push       5
004ba74e e86da9f6ff               call       0x4250c0
004ba753 83c418                   add        esp, 0x18
004ba756 eb2a                     jmp        0x4ba782
004ba758 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ba75b 83c110                   add        ecx, 0x10
004ba75e 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004ba761 68100e0000               push       0xe10
004ba766 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004ba769 e872defbff               call       0x4785e0
004ba76e 0fb6d0                   movzx      edx, al
004ba771 85d2                     test       edx, edx
004ba773 740d                     je         0x4ba782
004ba775 6a04                     push       4
004ba777 b9404d5c00               mov        ecx, 0x5c4d40
004ba77c e82f68feff               call       0x4a0fb0
004ba781 90                       nop        
004ba782 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004ba788 e8039dffff               call       0x4b4490
004ba78d 90                       nop        
004ba78e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba791 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba797 c1e904                   shr        ecx, 4
004ba79a 83e101                   and        ecx, 1
004ba79d 740a                     je         0x4ba7a9
004ba79f b803000000               mov        eax, 3
004ba7a4 e9bf000000               jmp        0x4ba868
004ba7a9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba7ac 8b82e8000000             mov        eax, dword ptr [edx + 0xe8]
004ba7b2 c1e805                   shr        eax, 5
004ba7b5 83e001                   and        eax, 1
004ba7b8 740a                     je         0x4ba7c4
004ba7ba b803000000               mov        eax, 3
004ba7bf e9a4000000               jmp        0x4ba868
004ba7c4 8b0dec4e5c00             mov        ecx, dword ptr [0x5c4eec]
004ba7ca e8010ef7ff               call       0x42b5d0
004ba7cf 8b0df04e5c00             mov        ecx, dword ptr [0x5c4ef0]
004ba7d5 e8f60df7ff               call       0x42b5d0
004ba7da 8b0df44e5c00             mov        ecx, dword ptr [0x5c4ef4]
004ba7e0 e8eb0df7ff               call       0x42b5d0
004ba7e5 8b0df84e5c00             mov        ecx, dword ptr [0x5c4ef8]
004ba7eb e8e00df7ff               call       0x42b5d0
004ba7f0 6a00                     push       0
004ba7f2 e88998faff               call       0x464080
004ba7f7 83c404                   add        esp, 4
004ba7fa 8bc8                     mov        ecx, eax
004ba7fc e8df3df5ff               call       0x40e5e0
004ba801 e83ae7fdff               call       0x498f40
004ba806 8945d8                   mov        dword ptr [ebp - 0x28], eax
004ba809 6a00                     push       0
004ba80b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004ba80e e89d250000               call       0x4bcdb0
004ba813 e828e7fdff               call       0x498f40
004ba818 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004ba81b 6a00                     push       0
004ba81d 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004ba820 e82b250000               call       0x4bcd50
004ba825 90                       nop        
004ba826 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
004ba82d 740f                     je         0x4ba83e
004ba82f 8b0d18a55b00             mov        ecx, dword ptr [0x5ba518]
004ba835 83e901                   sub        ecx, 1
004ba838 890d18a55b00             mov        dword ptr [0x5ba518], ecx
004ba83e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ba841 83c210                   add        edx, 0x10
004ba844 8955d0                   mov        dword ptr [ebp - 0x30], edx
004ba847 6a00                     push       0
004ba849 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004ba84c e8ef8cf6ff               call       0x423540
004ba851 e8eae6fdff               call       0x498f40
004ba856 8945cc                   mov        dword ptr [ebp - 0x34], eax
004ba859 6a01                     push       1
004ba85b 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004ba85e e84d250000               call       0x4bcdb0
004ba863 b801000000               mov        eax, 1
004ba868 8be5                     mov        esp, ebp
004ba86a 5d                       pop        ebp
004ba86b c3                       ret        
004ba86c cc                       int3       
004ba86d cc                       int3       
004ba86e cc                       int3       
004ba86f cc                       int3       
