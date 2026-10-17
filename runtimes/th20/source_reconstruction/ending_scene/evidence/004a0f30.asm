004a0f30 55                       push       ebp
004a0f31 8bec                     mov        ebp, esp
004a0f33 83ec08                   sub        esp, 8
004a0f36 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0f39 6a00                     push       0
004a0f3b 6a00                     push       0
004a0f3d 8b4508                   mov        eax, dword ptr [ebp + 8]
004a0f40 50                       push       eax
004a0f41 e85ac50100               call       0x4bd4a0
004a0f46 83c404                   add        esp, 4
004a0f49 50                       push       eax
004a0f4a e851fbf6ff               call       0x410aa0
004a0f4f 83c40c                   add        esp, 0xc
004a0f52 8945f8                   mov        dword ptr [ebp - 8], eax
004a0f55 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0f58 83791400                 cmp        dword ptr [ecx + 0x14], 0
004a0f5c 741c                     je         0x4a0f7a
004a0f5e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0f61 8b4214                   mov        eax, dword ptr [edx + 0x14]
004a0f64 50                       push       eax
004a0f65 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a0f6b e800e7f7ff               call       0x41f670
004a0f70 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0f73 c7411400000000           mov        dword ptr [ecx + 0x14], 0
004a0f7a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0f7d 8b45f8                   mov        eax, dword ptr [ebp - 8]
004a0f80 894214                   mov        dword ptr [edx + 0x14], eax
004a0f83 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0f86 8b4114                   mov        eax, dword ptr [ecx + 0x14]
004a0f89 8be5                     mov        esp, ebp
004a0f8b 5d                       pop        ebp
004a0f8c c20400                   ret        4
004a0f8f cc                       int3       
