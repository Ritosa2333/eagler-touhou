0044c150 55                       push       ebp
0044c151 8bec                     mov        ebp, esp
0044c153 83ec10                   sub        esp, 0x10
0044c156 894df0                   mov        dword ptr [ebp - 0x10], ecx
0044c159 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c15c 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0044c15f 83e1fc                   and        ecx, 0xfffffffc
0044c162 83c901                   or         ecx, 1
0044c165 8b5508                   mov        edx, dword ptr [ebp + 8]
0044c168 894a14                   mov        dword ptr [edx + 0x14], ecx
0044c16b b9404d5c00               mov        ecx, 0x5c4d40
0044c170 e8bb65fcff               call       0x412730
0044c175 8945fc                   mov        dword ptr [ebp - 4], eax
0044c178 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044c17b 8b08                     mov        ecx, dword ptr [eax]
0044c17d 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0044c180 8955f4                   mov        dword ptr [ebp - 0xc], edx
0044c183 6a00                     push       0
0044c185 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c188 50                       push       eax
0044c189 6a00                     push       0
0044c18b 6a15                     push       0x15
0044c18d 6a01                     push       1
0044c18f 6a01                     push       1
0044c191 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0044c194 51                       push       ecx
0044c195 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044c198 52                       push       edx
0044c199 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044c19c 50                       push       eax
0044c19d ff55f4                   call       dword ptr [ebp - 0xc]
0044c1a0 85c0                     test       eax, eax
0044c1a2 740d                     je         0x44c1b1
0044c1a4 6850e25600               push       0x56e250
0044c1a9 e80205fcff               call       0x40c6b0
0044c1ae 83c404                   add        esp, 4
0044c1b1 833d2c4e5c0015           cmp        dword ptr [0x5c4e2c], 0x15
0044c1b8 7509                     jne        0x44c1c3
0044c1ba c745f804000000           mov        dword ptr [ebp - 8], 4
0044c1c1 eb07                     jmp        0x44c1ca
0044c1c3 c745f802000000           mov        dword ptr [ebp - 8], 2
0044c1ca 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c1cd 8b55f8                   mov        edx, dword ptr [ebp - 8]
0044c1d0 89510c                   mov        dword ptr [ecx + 0xc], edx
0044c1d3 33c0                     xor        eax, eax
0044c1d5 8be5                     mov        esp, ebp
0044c1d7 5d                       pop        ebp
0044c1d8 c20c00                   ret        0xc
0044c1db cc                       int3       
0044c1dc cc                       int3       
0044c1dd cc                       int3       
0044c1de cc                       int3       
0044c1df cc                       int3       
