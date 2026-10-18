004b5820 55                       push       ebp
004b5821 8bec                     mov        ebp, esp
004b5823 83ec08                   sub        esp, 8
004b5826 894dfc                   mov        dword ptr [ebp - 4], ecx
004b5829 833d184a5c0000           cmp        dword ptr [0x5c4a18], 0
004b5830 750b                     jne        0x4b583d
004b5832 6a0a                     push       0xa
004b5834 ff156cc15600             call       dword ptr [0x56c16c]
004b583a 90                       nop        
004b583b ebec                     jmp        0x4b5829
004b583d 6858065700               push       0x570658
004b5842 6a05                     push       5
004b5844 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004b584a e8e192f9ff               call       0x44eb30
004b584f 8945f8                   mov        dword ptr [ebp - 8], eax
004b5852 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b5855 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b5858 8988cc020000             mov        dword ptr [eax + 0x2cc], ecx
004b585e 837df800                 cmp        dword ptr [ebp - 8], 0
004b5862 7517                     jne        0x4b587b
004b5864 6810f65600               push       0x56f610
004b5869 6878065c00               push       0x5c0678
004b586e e8dde8f9ff               call       0x454150
004b5873 83c408                   add        esp, 8
004b5876 83c8ff                   or         eax, 0xffffffff
004b5879 eb61                     jmp        0x4b58dc
004b587b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b587e e8dd000000               call       0x4b5960
004b5883 85c0                     test       eax, eax
004b5885 7405                     je         0x4b588c
004b5887 83c8ff                   or         eax, 0xffffffff
004b588a eb50                     jmp        0x4b58dc
004b588c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b588f 52                       push       edx
004b5890 68007c4b00               push       0x4b7c00
004b5895 6a2a                     push       0x2a
004b5897 e874caf5ff               call       0x412310
004b589c 83c40c                   add        esp, 0xc
004b589f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b58a2 894108                   mov        dword ptr [ecx + 8], eax
004b58a5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b58a8 52                       push       edx
004b58a9 68707d4b00               push       0x4b7d70
004b58ae 6a3a                     push       0x3a
004b58b0 e8fbcaf5ff               call       0x4123b0
004b58b5 83c40c                   add        esp, 0xc
004b58b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b58bb 89410c                   mov        dword ptr [ecx + 0xc], eax
004b58be 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b58c1 52                       push       edx
004b58c2 68607d4b00               push       0x4b7d60
004b58c7 6a34                     push       0x34
004b58c9 e8e2caf5ff               call       0x4123b0
004b58ce 83c40c                   add        esp, 0xc
004b58d1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b58d4 898150010000             mov        dword ptr [ecx + 0x150], eax
004b58da 33c0                     xor        eax, eax
004b58dc 8be5                     mov        esp, ebp
004b58de 5d                       pop        ebp
004b58df c3                       ret        
