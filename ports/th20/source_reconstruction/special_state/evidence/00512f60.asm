00512f60 55                       push       ebp
00512f61 8bec                     mov        ebp, esp
00512f63 83ec0c                   sub        esp, 0xc
00512f66 894df8                   mov        dword ptr [ebp - 8], ecx
00512f69 6840425700               push       0x574240
00512f6e 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00512f74 e8f7e8ffff               call       0x511870
00512f79 8945fc                   mov        dword ptr [ebp - 4], eax
00512f7c 8b45f8                   mov        eax, dword ptr [ebp - 8]
00512f7f 8945f4                   mov        dword ptr [ebp - 0xc], eax
00512f82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512f85 51                       push       ecx
00512f86 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00512f89 e812efefff               call       0x411ea0
00512f8e 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00512f91 52                       push       edx
00512f92 8b4508                   mov        eax, dword ptr [ebp + 8]
00512f95 50                       push       eax
00512f96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512f99 e812000000               call       0x512fb0
00512f9e 90                       nop        
00512f9f 8be5                     mov        esp, ebp
00512fa1 5d                       pop        ebp
00512fa2 c20800                   ret        8
00512fa5 cc                       int3       
00512fa6 cc                       int3       
00512fa7 cc                       int3       
00512fa8 cc                       int3       
00512fa9 cc                       int3       
00512faa cc                       int3       
00512fab cc                       int3       
00512fac cc                       int3       
00512fad cc                       int3       
00512fae cc                       int3       
00512faf cc                       int3       
