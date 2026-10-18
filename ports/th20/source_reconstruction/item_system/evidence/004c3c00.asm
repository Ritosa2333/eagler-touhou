004c3c00 55                       push       ebp
004c3c01 8bec                     mov        ebp, esp
004c3c03 83ec08                   sub        esp, 8
004c3c06 894dfc                   mov        dword ptr [ebp - 4], ecx
004c3c09 8b4508                   mov        eax, dword ptr [ebp + 8]
004c3c0c 50                       push       eax
004c3c0d b968a55b00               mov        ecx, 0x5ba568
004c3c12 e8a97ff4ff               call       0x40bbc0
004c3c17 8945f8                   mov        dword ptr [ebp - 8], eax
004c3c1a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c1d 51                       push       ecx
004c3c1e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3c21 e89af1f4ff               call       0x412dc0
004c3c26 8b5508                   mov        edx, dword ptr [ebp + 8]
004c3c29 52                       push       edx
004c3c2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c2d e89e130000               call       0x4c4fd0
004c3c32 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c3c35 50                       push       eax
004c3c36 68f0464c00               push       0x4c46f0
004c3c3b 6a27                     push       0x27
004c3c3d e8cee6f4ff               call       0x412310
004c3c42 83c40c                   add        esp, 0xc
004c3c45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c48 894108                   mov        dword ptr [ecx + 8], eax
004c3c4b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c3c4e 52                       push       edx
004c3c4f 6870474c00               push       0x4c4770
004c3c54 6a23                     push       0x23
004c3c56 e855e7f4ff               call       0x4123b0
004c3c5b 83c40c                   add        esp, 0xc
004c3c5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c61 89410c                   mov        dword ptr [ecx + 0xc], eax
004c3c64 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c3c67 52                       push       edx
004c3c68 6840474c00               push       0x4c4740
004c3c6d 6a13                     push       0x13
004c3c6f e83ce7f4ff               call       0x4123b0
004c3c74 83c40c                   add        esp, 0xc
004c3c77 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c7a 894114                   mov        dword ptr [ecx + 0x14], eax
004c3c7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c3c80 e89b85ffff               call       0x4bc220
004c3c85 33c0                     xor        eax, eax
004c3c87 8be5                     mov        esp, ebp
004c3c89 5d                       pop        ebp
004c3c8a c20400                   ret        4
004c3c8d cc                       int3       
004c3c8e cc                       int3       
004c3c8f cc                       int3       
