00477f60 55                       push       ebp
00477f61 8bec                     mov        ebp, esp
00477f63 83ec0c                   sub        esp, 0xc
00477f66 894dfc                   mov        dword ptr [ebp - 4], ecx
00477f69 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477f6c 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
00477f6f 034d08                   add        ecx, dword ptr [ebp + 8]
00477f72 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477f75 894a5c                   mov        dword ptr [edx + 0x5c], ecx
00477f78 c745f810270000           mov        dword ptr [ebp - 8], 0x2710
00477f7f c745f400000000           mov        dword ptr [ebp - 0xc], 0
00477f86 8d45f8                   lea        eax, [ebp - 8]
00477f89 50                       push       eax
00477f8a 8d4df4                   lea        ecx, [ebp - 0xc]
00477f8d 51                       push       ecx
00477f8e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477f91 83c25c                   add        edx, 0x5c
00477f94 52                       push       edx
00477f95 e8d691ffff               call       0x471170
00477f9a 83c40c                   add        esp, 0xc
00477f9d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477fa0 8b10                     mov        edx, dword ptr [eax]
00477fa2 89515c                   mov        dword ptr [ecx + 0x5c], edx
00477fa5 8be5                     mov        esp, ebp
00477fa7 5d                       pop        ebp
00477fa8 c20400                   ret        4
00477fab cc                       int3       
00477fac cc                       int3       
00477fad cc                       int3       
00477fae cc                       int3       
00477faf cc                       int3       
