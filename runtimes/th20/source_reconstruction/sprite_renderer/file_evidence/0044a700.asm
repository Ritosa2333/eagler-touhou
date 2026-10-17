0044a700 55                       push       ebp
0044a701 8bec                     mov        ebp, esp
0044a703 6aff                     push       -1
0044a705 68dc845600               push       0x5684dc
0044a70a 64a100000000             mov        eax, dword ptr fs:[0]
0044a710 50                       push       eax
0044a711 81ecdc010000             sub        esp, 0x1dc
0044a717 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044a71c 33c5                     xor        eax, ebp
0044a71e 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044a721 50                       push       eax
0044a722 8d45f4                   lea        eax, [ebp - 0xc]
0044a725 64a300000000             mov        dword ptr fs:[0], eax
0044a72b 898dccfeffff             mov        dword ptr [ebp - 0x134], ecx
0044a731 8b4508                   mov        eax, dword ptr [ebp + 8]
0044a734 50                       push       eax
0044a735 68e4e05600               push       0x56e0e4
0044a73a 8d8decfeffff             lea        ecx, [ebp - 0x114]
0044a740 51                       push       ecx
0044a741 e8dae9fdff               call       0x429120
0044a746 83c40c                   add        esp, 0xc
0044a749 6a00                     push       0
0044a74b 6a00                     push       0
0044a74d 8d95ecfeffff             lea        edx, [ebp - 0x114]
0044a753 52                       push       edx
0044a754 e84763fcff               call       0x410aa0
0044a759 83c40c                   add        esp, 0xc
0044a75c 8985bcfeffff             mov        dword ptr [ebp - 0x144], eax
0044a762 83bdbcfeffff00           cmp        dword ptr [ebp - 0x144], 0
0044a769 7508                     jne        0x44a773
0044a76b 83c8ff                   or         eax, 0xffffffff
0044a76e e970030000               jmp        0x44aae3
0044a773 c785c4feffff00000000     mov        dword ptr [ebp - 0x13c], 0
0044a77d c785c8feffff00000000     mov        dword ptr [ebp - 0x138], 0
0044a787 c785c0feffff00000000     mov        dword ptr [ebp - 0x140], 0
0044a791 c785b4feffff00000000     mov        dword ptr [ebp - 0x14c], 0
0044a79b 8b85ccfeffff             mov        eax, dword ptr [ebp - 0x134]
0044a7a1 8b8dbcfeffff             mov        ecx, dword ptr [ebp - 0x144]
0044a7a7 89483c                   mov        dword ptr [eax + 0x3c], ecx
0044a7aa 6a00                     push       0
0044a7ac 8d5508                   lea        edx, [ebp + 8]
0044a7af 52                       push       edx
0044a7b0 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0044a7b6 e8651ffcff               call       0x40c720
0044a7bb c745fc00000000           mov        dword ptr [ebp - 4], 0
0044a7c2 8d8548feffff             lea        eax, [ebp - 0x1b8]
0044a7c8 50                       push       eax
0044a7c9 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0044a7cf e8bc1d0000               call       0x44c590
0044a7d4 8985b0feffff             mov        dword ptr [ebp - 0x150], eax
0044a7da 8b8db0feffff             mov        ecx, dword ptr [ebp - 0x150]
0044a7e0 898dacfeffff             mov        dword ptr [ebp - 0x154], ecx
0044a7e6 c645fc01                 mov        byte ptr [ebp - 4], 1
0044a7ea 8b95acfeffff             mov        edx, dword ptr [ebp - 0x154]
0044a7f0 8995a4feffff             mov        dword ptr [ebp - 0x15c], edx
0044a7f6 8d8dbbfeffff             lea        ecx, [ebp - 0x145]
0044a7fc e89f15fcff               call       0x40bda0
0044a801 8985a8feffff             mov        dword ptr [ebp - 0x158], eax
0044a807 8b85a8feffff             mov        eax, dword ptr [ebp - 0x158]
0044a80d 50                       push       eax
0044a80e 8d8d60feffff             lea        ecx, [ebp - 0x1a0]
0044a814 51                       push       ecx
0044a815 8b8da4feffff             mov        ecx, dword ptr [ebp - 0x15c]
0044a81b e8a0e1fcff               call       0x4189c0
0044a820 8985a0feffff             mov        dword ptr [ebp - 0x160], eax
0044a826 8b95a0feffff             mov        edx, dword ptr [ebp - 0x160]
0044a82c 89959cfeffff             mov        dword ptr [ebp - 0x164], edx
0044a832 c645fc02                 mov        byte ptr [ebp - 4], 2
0044a836 8b859cfeffff             mov        eax, dword ptr [ebp - 0x164]
0044a83c 50                       push       eax
0044a83d 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044a843 83c104                   add        ecx, 4
0044a846 e875c5ffff               call       0x446dc0
0044a84b c645fc01                 mov        byte ptr [ebp - 4], 1
0044a84f 8d8d60feffff             lea        ecx, [ebp - 0x1a0]
0044a855 e8463dfcff               call       0x40e5a0
0044a85a c645fc00                 mov        byte ptr [ebp - 4], 0
0044a85e 8d8d48feffff             lea        ecx, [ebp - 0x1b8]
0044a864 e8573efcff               call       0x40e6c0
0044a869 90                       nop        
0044a86a 8d8d18feffff             lea        ecx, [ebp - 0x1e8]
0044a870 51                       push       ecx
0044a871 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0044a877 e8046d0000               call       0x451580
0044a87c 898598feffff             mov        dword ptr [ebp - 0x168], eax
0044a882 8b9598feffff             mov        edx, dword ptr [ebp - 0x168]
0044a888 899594feffff             mov        dword ptr [ebp - 0x16c], edx
0044a88e c645fc03                 mov        byte ptr [ebp - 4], 3
0044a892 8b8594feffff             mov        eax, dword ptr [ebp - 0x16c]
0044a898 89858cfeffff             mov        dword ptr [ebp - 0x174], eax
0044a89e 8d8dbafeffff             lea        ecx, [ebp - 0x146]
0044a8a4 e8f714fcff               call       0x40bda0
0044a8a9 898590feffff             mov        dword ptr [ebp - 0x170], eax
0044a8af 8b8d90feffff             mov        ecx, dword ptr [ebp - 0x170]
0044a8b5 51                       push       ecx
0044a8b6 8d9530feffff             lea        edx, [ebp - 0x1d0]
0044a8bc 52                       push       edx
0044a8bd 8b8d8cfeffff             mov        ecx, dword ptr [ebp - 0x174]
0044a8c3 e8f8e0fcff               call       0x4189c0
0044a8c8 898588feffff             mov        dword ptr [ebp - 0x178], eax
0044a8ce 8b8588feffff             mov        eax, dword ptr [ebp - 0x178]
0044a8d4 898584feffff             mov        dword ptr [ebp - 0x17c], eax
0044a8da c645fc04                 mov        byte ptr [ebp - 4], 4
0044a8de 8b8d84feffff             mov        ecx, dword ptr [ebp - 0x17c]
0044a8e4 51                       push       ecx
0044a8e5 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044a8eb 83c120                   add        ecx, 0x20
0044a8ee e8cdc4ffff               call       0x446dc0
0044a8f3 c645fc03                 mov        byte ptr [ebp - 4], 3
0044a8f7 8d8d30feffff             lea        ecx, [ebp - 0x1d0]
0044a8fd e89e3cfcff               call       0x40e5a0
0044a902 c645fc00                 mov        byte ptr [ebp - 4], 0
0044a906 8d8d18feffff             lea        ecx, [ebp - 0x1e8]
0044a90c e8af3dfcff               call       0x40e6c0
0044a911 90                       nop        
0044a912 8b95bcfeffff             mov        edx, dword ptr [ebp - 0x144]
0044a918 8995d0feffff             mov        dword ptr [ebp - 0x130], edx
0044a91e 8b85c4feffff             mov        eax, dword ptr [ebp - 0x13c]
0044a924 83c001                   add        eax, 1
0044a927 8985c4feffff             mov        dword ptr [ebp - 0x13c], eax
0044a92d 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
0044a933 0fb75106                 movzx      edx, word ptr [ecx + 6]
0044a937 0395c8feffff             add        edx, dword ptr [ebp - 0x138]
0044a93d 8995c8feffff             mov        dword ptr [ebp - 0x138], edx
0044a943 8b85d0feffff             mov        eax, dword ptr [ebp - 0x130]
0044a949 0fb74804                 movzx      ecx, word ptr [eax + 4]
0044a94d 038dc0feffff             add        ecx, dword ptr [ebp - 0x140]
0044a953 898dc0feffff             mov        dword ptr [ebp - 0x140], ecx
0044a959 8b95d0feffff             mov        edx, dword ptr [ebp - 0x130]
0044a95f 837a2400                 cmp        dword ptr [edx + 0x24], 0
0044a963 7502                     jne        0x44a967
0044a965 eb17                     jmp        0x44a97e
0044a967 8b85d0feffff             mov        eax, dword ptr [ebp - 0x130]
0044a96d 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
0044a973 034824                   add        ecx, dword ptr [eax + 0x24]
0044a976 898dd0feffff             mov        dword ptr [ebp - 0x130], ecx
0044a97c eba0                     jmp        0x44a91e
0044a97e 8b95ccfeffff             mov        edx, dword ptr [ebp - 0x134]
0044a984 8b85c4feffff             mov        eax, dword ptr [ebp - 0x13c]
0044a98a 894244                   mov        dword ptr [edx + 0x44], eax
0044a98d 8b8dc4feffff             mov        ecx, dword ptr [ebp - 0x13c]
0044a993 51                       push       ecx
0044a994 6870e25600               push       0x56e270
0044a999 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044a99f e84cccffff               call       0x4475f0
0044a9a4 8b95ccfeffff             mov        edx, dword ptr [ebp - 0x134]
0044a9aa 894258                   mov        dword ptr [edx + 0x58], eax
0044a9ad 8b85c0feffff             mov        eax, dword ptr [ebp - 0x140]
0044a9b3 50                       push       eax
0044a9b4 68b8e25600               push       0x56e2b8
0044a9b9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044a9bf e85ccbffff               call       0x447520
0044a9c4 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044a9ca 894150                   mov        dword ptr [ecx + 0x50], eax
0044a9cd 8b95c8feffff             mov        edx, dword ptr [ebp - 0x138]
0044a9d3 52                       push       edx
0044a9d4 68fce25600               push       0x56e2fc
0044a9d9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044a9df e89ccaffff               call       0x447480
0044a9e4 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044a9ea 894154                   mov        dword ptr [ecx + 0x54], eax
0044a9ed 8b95ccfeffff             mov        edx, dword ptr [ebp - 0x134]
0044a9f3 8b85c8feffff             mov        eax, dword ptr [ebp - 0x138]
0044a9f9 894248                   mov        dword ptr [edx + 0x48], eax
0044a9fc 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044aa02 8b95c0feffff             mov        edx, dword ptr [ebp - 0x140]
0044aa08 89514c                   mov        dword ptr [ecx + 0x4c], edx
0044aa0b 8b85bcfeffff             mov        eax, dword ptr [ebp - 0x144]
0044aa11 8985d0feffff             mov        dword ptr [ebp - 0x130], eax
0044aa17 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
0044aa1d 51                       push       ecx
0044aa1e 8b95b4feffff             mov        edx, dword ptr [ebp - 0x14c]
0044aa24 52                       push       edx
0044aa25 8b85ccfeffff             mov        eax, dword ptr [ebp - 0x134]
0044aa2b 50                       push       eax
0044aa2c 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0044aa32 e8b9410000               call       0x44ebf0
0044aa37 898580feffff             mov        dword ptr [ebp - 0x180], eax
0044aa3d 83bd80feffff00           cmp        dword ptr [ebp - 0x180], 0
0044aa44 7d24                     jge        0x44aa6a
0044aa46 c7857cfeffff00000000     mov        dword ptr [ebp - 0x184], 0
0044aa50 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044aa57 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0044aa5d e85e3cfcff               call       0x40e6c0
0044aa62 8b857cfeffff             mov        eax, dword ptr [ebp - 0x184]
0044aa68 eb79                     jmp        0x44aae3
0044aa6a 8b8db4feffff             mov        ecx, dword ptr [ebp - 0x14c]
0044aa70 83c101                   add        ecx, 1
0044aa73 898db4feffff             mov        dword ptr [ebp - 0x14c], ecx
0044aa79 8b95d0feffff             mov        edx, dword ptr [ebp - 0x130]
0044aa7f 837a2400                 cmp        dword ptr [edx + 0x24], 0
0044aa83 7502                     jne        0x44aa87
0044aa85 eb1a                     jmp        0x44aaa1
0044aa87 8b85d0feffff             mov        eax, dword ptr [ebp - 0x130]
0044aa8d 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
0044aa93 034824                   add        ecx, dword ptr [eax + 0x24]
0044aa96 898dd0feffff             mov        dword ptr [ebp - 0x130], ecx
0044aa9c e976ffffff               jmp        0x44aa17
0044aaa1 8b95c8feffff             mov        edx, dword ptr [ebp - 0x138]
0044aaa7 52                       push       edx
0044aaa8 6838e35600               push       0x56e338
0044aaad 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044aab3 e808ccffff               call       0x4476c0
0044aab8 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0044aabe 894140                   mov        dword ptr [ecx + 0x40], eax
0044aac1 c78578feffff00000000     mov        dword ptr [ebp - 0x188], 0
0044aacb c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044aad2 8d8dd4feffff             lea        ecx, [ebp - 0x12c]
0044aad8 e8e33bfcff               call       0x40e6c0
0044aadd 8b8578feffff             mov        eax, dword ptr [ebp - 0x188]
0044aae3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044aae6 64890d00000000           mov        dword ptr fs:[0], ecx
0044aaed 59                       pop        ecx
0044aaee 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044aaf1 33cd                     xor        ecx, ebp
0044aaf3 e8f67e0f00               call       0x5429ee
0044aaf8 8be5                     mov        esp, ebp
0044aafa 5d                       pop        ebp
0044aafb c20400                   ret        4
0044aafe cc                       int3       
0044aaff cc                       int3       
