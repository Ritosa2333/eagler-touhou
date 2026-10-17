00477ae0 55                       push       ebp
00477ae1 8bec                     mov        ebp, esp
00477ae3 6aff                     push       -1
00477ae5 6800765600               push       0x567600
00477aea 64a100000000             mov        eax, dword ptr fs:[0]
00477af0 50                       push       eax
00477af1 51                       push       ecx
00477af2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00477af7 33c5                     xor        eax, ebp
00477af9 50                       push       eax
00477afa 8d45f4                   lea        eax, [ebp - 0xc]
00477afd 64a300000000             mov        dword ptr fs:[0], eax
00477b03 894df0                   mov        dword ptr [ebp - 0x10], ecx
00477b06 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477b09 c70094fa5600             mov        dword ptr [eax], 0x56fa94
00477b0f 68b8fa5600               push       0x56fab8
00477b14 e8974bf9ff               call       0x40c6b0
00477b19 83c404                   add        esp, 4
00477b1c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477b1f 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00477b22 52                       push       edx
00477b23 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00477b29 e8927cfaff               call       0x41f7c0
00477b2e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477b31 c7401400000000           mov        dword ptr [eax + 0x14], 0
00477b38 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477b3b 8b5108                   mov        edx, dword ptr [ecx + 8]
00477b3e 52                       push       edx
00477b3f 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00477b45 e866a9f9ff               call       0x4124b0
00477b4a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477b4d 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00477b50 51                       push       ecx
00477b51 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00477b57 e854a9f9ff               call       0x4124b0
00477b5c 90                       nop        
00477b5d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477b60 e88b82faff               call       0x41fdf0
00477b65 90                       nop        
00477b66 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00477b69 64890d00000000           mov        dword ptr fs:[0], ecx
00477b70 59                       pop        ecx
00477b71 8be5                     mov        esp, ebp
00477b73 5d                       pop        ebp
00477b74 c3                       ret        
00477b75 cc                       int3       
00477b76 cc                       int3       
00477b77 cc                       int3       
00477b78 cc                       int3       
00477b79 cc                       int3       
00477b7a cc                       int3       
00477b7b cc                       int3       
00477b7c cc                       int3       
00477b7d cc                       int3       
00477b7e cc                       int3       
00477b7f cc                       int3       
