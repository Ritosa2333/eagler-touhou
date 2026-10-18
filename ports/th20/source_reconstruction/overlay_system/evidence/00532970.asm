00532970 55                       push       ebp
00532971 8bec                     mov        ebp, esp
00532973 6aff                     push       -1
00532975 6800765600               push       0x567600
0053297a 64a100000000             mov        eax, dword ptr fs:[0]
00532980 50                       push       eax
00532981 51                       push       ecx
00532982 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00532987 33c5                     xor        eax, ebp
00532989 50                       push       eax
0053298a 8d45f4                   lea        eax, [ebp - 0xc]
0053298d 64a300000000             mov        dword ptr fs:[0], eax
00532993 894df0                   mov        dword ptr [ebp - 0x10], ecx
00532996 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00532999 c70028625700             mov        dword ptr [eax], 0x576228
0053299f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005329a2 8b5128                   mov        edx, dword ptr [ecx + 0x28]
005329a5 52                       push       edx
005329a6 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005329ac e88ffdffff               call       0x532740
005329b1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005329b4 c7402800000000           mov        dword ptr [eax + 0x28], 0
005329bb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005329be 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
005329c1 52                       push       edx
005329c2 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005329c8 e873fdffff               call       0x532740
005329cd 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005329d0 c7402c00000000           mov        dword ptr [eax + 0x2c], 0
005329d7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005329da 8b5130                   mov        edx, dword ptr [ecx + 0x30]
005329dd 52                       push       edx
005329de 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005329e4 e857fdffff               call       0x532740
005329e9 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005329ec c7403000000000           mov        dword ptr [eax + 0x30], 0
005329f3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005329f6 8b5134                   mov        edx, dword ptr [ecx + 0x34]
005329f9 52                       push       edx
005329fa 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00532a00 e83bfdffff               call       0x532740
00532a05 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00532a08 c7403400000000           mov        dword ptr [eax + 0x34], 0
00532a0f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00532a12 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00532a15 52                       push       edx
00532a16 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00532a1c e8dffdf6ff               call       0x4a2800
00532a21 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00532a24 c7404000000000           mov        dword ptr [eax + 0x40], 0
00532a2b 685c625700               push       0x57625c
00532a30 e87b9cedff               call       0x40c6b0
00532a35 83c404                   add        esp, 4
00532a38 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00532a3b 8b5108                   mov        edx, dword ptr [ecx + 8]
00532a3e 52                       push       edx
00532a3f 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00532a45 e866faedff               call       0x4124b0
00532a4a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00532a4d 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00532a50 51                       push       ecx
00532a51 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00532a57 e854faedff               call       0x4124b0
00532a5c 6a17                     push       0x17
00532a5e 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00532a64 e8c799f1ff               call       0x44c430
00532a69 90                       nop        
00532a6a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00532a6d e87ed3eeff               call       0x41fdf0
00532a72 90                       nop        
00532a73 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00532a76 64890d00000000           mov        dword ptr fs:[0], ecx
00532a7d 59                       pop        ecx
00532a7e 8be5                     mov        esp, ebp
00532a80 5d                       pop        ebp
00532a81 c3                       ret        
00532a82 cc                       int3       
00532a83 cc                       int3       
00532a84 cc                       int3       
00532a85 cc                       int3       
00532a86 cc                       int3       
00532a87 cc                       int3       
00532a88 cc                       int3       
00532a89 cc                       int3       
00532a8a cc                       int3       
00532a8b cc                       int3       
00532a8c cc                       int3       
00532a8d cc                       int3       
00532a8e cc                       int3       
00532a8f cc                       int3       
