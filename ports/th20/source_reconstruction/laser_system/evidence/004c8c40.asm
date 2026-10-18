004c8c40 55                       push       ebp
004c8c41 8bec                     mov        ebp, esp
004c8c43 51                       push       ecx
004c8c44 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8c47 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c4a 81c114130000             add        ecx, 0x1314
004c8c50 e81b07f8ff               call       0x449370
004c8c55 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c58 81c1300d0000             add        ecx, 0xd30
004c8c5e e80d07f8ff               call       0x449370
004c8c63 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c66 81c14c070000             add        ecx, 0x74c
004c8c6c e8ff06f8ff               call       0x449370
004c8c71 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c74 81c1f8060000             add        ecx, 0x6f8
004c8c7a e83138fbff               call       0x47c4b0
004c8c7f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c82 e869feffff               call       0x4c8af0
004c8c87 90                       nop        
004c8c88 8be5                     mov        esp, ebp
004c8c8a 5d                       pop        ebp
004c8c8b c3                       ret        
004c8c8c cc                       int3       
004c8c8d cc                       int3       
004c8c8e cc                       int3       
004c8c8f cc                       int3       
