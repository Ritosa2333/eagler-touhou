00487b10 55                       push       ebp
00487b11 8bec                     mov        ebp, esp
00487b13 83ec08                   sub        esp, 8
00487b16 894dfc                   mov        dword ptr [ebp - 4], ecx
00487b19 8b4508                   mov        eax, dword ptr [ebp + 8]
00487b1c 50                       push       eax
00487b1d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487b20 e8fb0e0000               call       0x488a20
00487b25 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00487b28 51                       push       ecx
00487b29 b968a55b00               mov        ecx, 0x5ba568
00487b2e e88d40f8ff               call       0x40bbc0
00487b33 8945f8                   mov        dword ptr [ebp - 8], eax
00487b36 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487b39 52                       push       edx
00487b3a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00487b3d e85eb2f8ff               call       0x412da0
00487b42 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487b45 50                       push       eax
00487b46 68d0854800               push       0x4885d0
00487b4b 6a28                     push       0x28
00487b4d e8bea7f8ff               call       0x412310
00487b52 83c40c                   add        esp, 0xc
00487b55 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487b58 894108                   mov        dword ptr [ecx + 8], eax
00487b5b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487b5e 52                       push       edx
00487b5f 6880864800               push       0x488680
00487b64 6a0c                     push       0xc
00487b66 e845a8f8ff               call       0x4123b0
00487b6b 83c40c                   add        esp, 0xc
00487b6e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487b71 89410c                   mov        dword ptr [ecx + 0xc], eax
00487b74 6a00                     push       0
00487b76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487b79 83c124                   add        ecx, 0x24
00487b7c e89fb9f9ff               call       0x423520
00487b81 33c0                     xor        eax, eax
00487b83 8be5                     mov        esp, ebp
00487b85 5d                       pop        ebp
00487b86 c20400                   ret        4
00487b89 cc                       int3       
00487b8a cc                       int3       
00487b8b cc                       int3       
00487b8c cc                       int3       
00487b8d cc                       int3       
00487b8e cc                       int3       
00487b8f cc                       int3       
