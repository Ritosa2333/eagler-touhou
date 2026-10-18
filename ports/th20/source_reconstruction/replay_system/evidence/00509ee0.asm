00509ee0 55                       push       ebp
00509ee1 8bec                     mov        ebp, esp
00509ee3 83ec0c                   sub        esp, 0xc
00509ee6 894dfc                   mov        dword ptr [ebp - 4], ecx
00509ee9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509eec 8b8864d80300             mov        ecx, dword ptr [eax + 0x3d864]
00509ef2 8a5508                   mov        dl, byte ptr [ebp + 8]
00509ef5 8811                     mov        byte ptr [ecx], dl
00509ef7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509efa 8b8864d80300             mov        ecx, dword ptr [eax + 0x3d864]
00509f00 83c101                   add        ecx, 1
00509f03 8b55fc                   mov        edx, dword ptr [ebp - 4]
00509f06 898a64d80300             mov        dword ptr [edx + 0x3d864], ecx
00509f0c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509f0f 05c04b0300               add        eax, 0x34bc0
00509f14 8945f4                   mov        dword ptr [ebp - 0xc], eax
00509f17 6a00                     push       0
00509f19 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00509f1c e8ef7cf0ff               call       0x411c10
00509f21 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509f24 8b9164d80300             mov        edx, dword ptr [ecx + 0x3d864]
00509f2a 2bd0                     sub        edx, eax
00509f2c 81fab0040000             cmp        edx, 0x4b0
00509f32 7c09                     jl         0x509f3d
00509f34 c745f801000000           mov        dword ptr [ebp - 8], 1
00509f3b eb07                     jmp        0x509f44
00509f3d c745f800000000           mov        dword ptr [ebp - 8], 0
00509f44 8b45f8                   mov        eax, dword ptr [ebp - 8]
00509f47 8be5                     mov        esp, ebp
00509f49 5d                       pop        ebp
00509f4a c20400                   ret        4
00509f4d cc                       int3       
00509f4e cc                       int3       
00509f4f cc                       int3       
