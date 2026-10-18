0044ebf0 55                       push       ebp
0044ebf1 8bec                     mov        ebp, esp
0044ebf3 81ec20010000             sub        esp, 0x120
0044ebf9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044ebfe 33c5                     xor        eax, ebp
0044ec00 8945fc                   mov        dword ptr [ebp - 4], eax
0044ec03 898de4feffff             mov        dword ptr [ebp - 0x11c], ecx
0044ec09 c785e0feffff00000000     mov        dword ptr [ebp - 0x120], 0
0044ec13 837d1000                 cmp        dword ptr [ebp + 0x10], 0
0044ec17 751a                     jne        0x44ec33
0044ec19 6828e45600               push       0x56e428
0044ec1e 6878065c00               push       0x5c0678
0044ec23 e808560000               call       0x454230
0044ec28 83c408                   add        esp, 8
0044ec2b 83c8ff                   or         eax, 0xffffffff
0044ec2e e9fd000000               jmp        0x44ed30
0044ec33 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0044ec36 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
0044ec3c 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0044ec42 833908                   cmp        dword ptr [ecx], 8
0044ec45 741a                     je         0x44ec61
0044ec47 6864e45600               push       0x56e464
0044ec4c 6878065c00               push       0x5c0678
0044ec51 e8da550000               call       0x454230
0044ec56 83c408                   add        esp, 8
0044ec59 83c8ff                   or         eax, 0xffffffff
0044ec5c e9cf000000               jmp        0x44ed30
0044ec61 8b95f0feffff             mov        edx, dword ptr [ebp - 0x110]
0044ec67 0fb64220                 movzx      eax, byte ptr [edx + 0x20]
0044ec6b 85c0                     test       eax, eax
0044ec6d 0f85b8000000             jne        0x44ed2b
0044ec73 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0044ec79 8b95f0feffff             mov        edx, dword ptr [ebp - 0x110]
0044ec7f 035110                   add        edx, dword ptr [ecx + 0x10]
0044ec82 8995ecfeffff             mov        dword ptr [ebp - 0x114], edx
0044ec88 b801000000               mov        eax, 1
0044ec8d 6bc800                   imul       ecx, eax, 0
0044ec90 8b95ecfeffff             mov        edx, dword ptr [ebp - 0x114]
0044ec96 0fbe040a                 movsx      eax, byte ptr [edx + ecx]
0044ec9a 83f840                   cmp        eax, 0x40
0044ec9d 0f8488000000             je         0x44ed2b
0044eca3 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
0044eca9 51                       push       ecx
0044ecaa 68e4e05600               push       0x56e0e4
0044ecaf 8d95f8feffff             lea        edx, [ebp - 0x108]
0044ecb5 52                       push       edx
0044ecb6 e865a4fdff               call       0x429120
0044ecbb 83c40c                   add        esp, 0xc
0044ecbe 6a01                     push       1
0044ecc0 8d85f4feffff             lea        eax, [ebp - 0x10c]
0044ecc6 50                       push       eax
0044ecc7 8d8df8feffff             lea        ecx, [ebp - 0x108]
0044eccd 51                       push       ecx
0044ecce e8cd1dfcff               call       0x410aa0
0044ecd3 83c40c                   add        esp, 0xc
0044ecd6 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
0044ecdc 83bde8feffff00           cmp        dword ptr [ebp - 0x118], 0
0044ece3 751e                     jne        0x44ed03
0044ece5 8b95ecfeffff             mov        edx, dword ptr [ebp - 0x114]
0044eceb 52                       push       edx
0044ecec 6888e45600               push       0x56e488
0044ecf1 6878065c00               push       0x5c0678
0044ecf6 e835550000               call       0x454230
0044ecfb 83c40c                   add        esp, 0xc
0044ecfe 83c8ff                   or         eax, 0xffffffff
0044ed01 eb2d                     jmp        0x44ed30
0044ed03 6b450c18                 imul       eax, dword ptr [ebp + 0xc], 0x18
0044ed07 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044ed0a 8b5158                   mov        edx, dword ptr [ecx + 0x58]
0044ed0d 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
0044ed13 894c0208                 mov        dword ptr [edx + eax + 8], ecx
0044ed17 6b550c18                 imul       edx, dword ptr [ebp + 0xc], 0x18
0044ed1b 8b4508                   mov        eax, dword ptr [ebp + 8]
0044ed1e 8b4858                   mov        ecx, dword ptr [eax + 0x58]
0044ed21 8b85e8feffff             mov        eax, dword ptr [ebp - 0x118]
0044ed27 89441104                 mov        dword ptr [ecx + edx + 4], eax
0044ed2b b801000000               mov        eax, 1
0044ed30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ed33 33cd                     xor        ecx, ebp
0044ed35 e8b43c0f00               call       0x5429ee
0044ed3a 8be5                     mov        esp, ebp
0044ed3c 5d                       pop        ebp
0044ed3d c20c00                   ret        0xc
