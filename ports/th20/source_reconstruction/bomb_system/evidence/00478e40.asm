00478e40 55                       push       ebp
00478e41 8bec                     mov        ebp, esp
00478e43 51                       push       ecx
00478e44 894dfc                   mov        dword ptr [ebp - 4], ecx
00478e47 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478e4a 83c174                   add        ecx, 0x74
00478e4d e8ee010000               call       0x479040
00478e52 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478e55 81c1c8000000             add        ecx, 0xc8
00478e5b e8e0010000               call       0x479040
00478e60 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478e63 50                       push       eax
00478e64 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00478e6a e85169faff               call       0x41f7c0
00478e6f 33c0                     xor        eax, eax
00478e71 8be5                     mov        esp, ebp
00478e73 5d                       pop        ebp
00478e74 c3                       ret        
00478e75 cc                       int3       
00478e76 cc                       int3       
00478e77 cc                       int3       
00478e78 cc                       int3       
00478e79 cc                       int3       
00478e7a cc                       int3       
00478e7b cc                       int3       
00478e7c cc                       int3       
00478e7d cc                       int3       
00478e7e cc                       int3       
00478e7f cc                       int3       
