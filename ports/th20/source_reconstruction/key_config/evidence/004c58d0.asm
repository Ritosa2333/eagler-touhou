004c58d0 55                       push       ebp
004c58d1 8bec                     mov        ebp, esp
004c58d3 83ec08                   sub        esp, 8
004c58d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c58d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c58dc 50                       push       eax
004c58dd 68705d4c00               push       0x4c5d70
004c58e2 6a0a                     push       0xa
004c58e4 e8d7c9f4ff               call       0x4122c0
004c58e9 83c40c                   add        esp, 0xc
004c58ec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c58ef 894108                   mov        dword ptr [ecx + 8], eax
004c58f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c58f5 52                       push       edx
004c58f6 68805d4c00               push       0x4c5d80
004c58fb 6a5e                     push       0x5e
004c58fd e85ecaf4ff               call       0x412360
004c5902 83c40c                   add        esp, 0xc
004c5905 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5908 89410c                   mov        dword ptr [ecx + 0xc], eax
004c590b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c590e 8b02                     mov        eax, dword ptr [edx]
004c5910 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5913 8b5004                   mov        edx, dword ptr [eax + 4]
004c5916 ffd2                     call       edx
004c5918 6a00                     push       0
004c591a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c591d 83c114                   add        ecx, 0x14
004c5920 e82b94ffff               call       0x4bed50
004c5925 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5928 83c014                   add        eax, 0x14
004c592b 8945f8                   mov        dword ptr [ebp - 8], eax
004c592e 6a01                     push       1
004c5930 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c5933 e8a8a2ffff               call       0x4bfbe0
004c5938 6a00                     push       0
004c593a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c593d 81c1c0000000             add        ecx, 0xc0
004c5943 e8d8dbf5ff               call       0x423520
004c5948 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c594b 81c1e0000000             add        ecx, 0xe0
004c5951 8b5508                   mov        edx, dword ptr [ebp + 8]
004c5954 8b02                     mov        eax, dword ptr [edx]
004c5956 8901                     mov        dword ptr [ecx], eax
004c5958 8b4204                   mov        eax, dword ptr [edx + 4]
004c595b 894104                   mov        dword ptr [ecx + 4], eax
004c595e 8b5208                   mov        edx, dword ptr [edx + 8]
004c5961 895108                   mov        dword ptr [ecx + 8], edx
004c5964 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5967 c7801001000001000000     mov        dword ptr [eax + 0x110], 1
004c5971 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5974 c7810c01000000000000     mov        dword ptr [ecx + 0x10c], 0
004c597e 6a00                     push       0
004c5980 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5983 e848080000               call       0x4c61d0
004c5988 33c0                     xor        eax, eax
004c598a 8be5                     mov        esp, ebp
004c598c 5d                       pop        ebp
004c598d c20400                   ret        4
