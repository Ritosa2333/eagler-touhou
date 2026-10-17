0044eb30 55                       push       ebp
0044eb31 8bec                     mov        ebp, esp
0044eb33 83ec0c                   sub        esp, 0xc
0044eb36 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044eb39 8b4508                   mov        eax, dword ptr [ebp + 8]
0044eb3c 50                       push       eax
0044eb3d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044eb40 e82b8efeff               call       0x437970
0044eb45 85c0                     test       eax, eax
0044eb47 741f                     je         0x44eb68
0044eb49 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044eb4c 51                       push       ecx
0044eb4d 68f4e35600               push       0x56e3f4
0044eb52 e859dbfbff               call       0x40c6b0
0044eb57 83c408                   add        esp, 8
0044eb5a 8b5508                   mov        edx, dword ptr [ebp + 8]
0044eb5d 52                       push       edx
0044eb5e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044eb61 e80a8efeff               call       0x437970
0044eb66 eb78                     jmp        0x44ebe0
0044eb68 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044eb6b 50                       push       eax
0044eb6c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044eb6f 51                       push       ecx
0044eb70 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044eb73 e8c8010000               call       0x44ed40
0044eb78 8945f8                   mov        dword ptr [ebp - 8], eax
0044eb7b 837df800                 cmp        dword ptr [ebp - 8], 0
0044eb7f 7504                     jne        0x44eb85
0044eb81 33c0                     xor        eax, eax
0044eb83 eb5b                     jmp        0x44ebe0
0044eb85 6a01                     push       1
0044eb87 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044eb8a 83c15c                   add        ecx, 0x5c
0044eb8d e82ea8ffff               call       0x4493c0
0044eb92 90                       nop        
0044eb93 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044eb96 83c15c                   add        ecx, 0x5c
0044eb99 e822a7fdff               call       0x4292c0
0044eb9e 85c0                     test       eax, eax
0044eba0 7406                     je         0x44eba8
0044eba2 c645ff01                 mov        byte ptr [ebp - 1], 1
0044eba6 eb04                     jmp        0x44ebac
0044eba8 c645ff00                 mov        byte ptr [ebp - 1], 0
0044ebac 0fb655ff                 movzx      edx, byte ptr [ebp - 1]
0044ebb0 85d2                     test       edx, edx
0044ebb2 7418                     je         0x44ebcc
0044ebb4 a188585c00               mov        eax, dword ptr [0x5c5888]
0044ebb9 c1e805                   shr        eax, 5
0044ebbc 83e003                   and        eax, 3
0044ebbf 750b                     jne        0x44ebcc
0044ebc1 6a01                     push       1
0044ebc3 ff156cc15600             call       dword ptr [0x56c16c]
0044ebc9 90                       nop        
0044ebca ebc7                     jmp        0x44eb93
0044ebcc 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044ebcf 51                       push       ecx
0044ebd0 6810e45600               push       0x56e410
0044ebd5 e8d6dafbff               call       0x40c6b0
0044ebda 83c408                   add        esp, 8
0044ebdd 8b45f8                   mov        eax, dword ptr [ebp - 8]
0044ebe0 8be5                     mov        esp, ebp
0044ebe2 5d                       pop        ebp
0044ebe3 c20800                   ret        8
0044ebe6 cc                       int3       
0044ebe7 cc                       int3       
0044ebe8 cc                       int3       
0044ebe9 cc                       int3       
0044ebea cc                       int3       
0044ebeb cc                       int3       
0044ebec cc                       int3       
0044ebed cc                       int3       
0044ebee cc                       int3       
0044ebef cc                       int3       
