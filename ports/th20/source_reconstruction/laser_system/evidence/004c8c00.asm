004c8c00 55                       push       ebp
004c8c01 8bec                     mov        ebp, esp
004c8c03 51                       push       ecx
004c8c04 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8c07 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c0a 81c1540d0000             add        ecx, 0xd54
004c8c10 e85b07f8ff               call       0x449370
004c8c15 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c18 81c170070000             add        ecx, 0x770
004c8c1e e84d07f8ff               call       0x449370
004c8c23 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c26 81c1f8060000             add        ecx, 0x6f8
004c8c2c e85f38fbff               call       0x47c490
004c8c31 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8c34 e8b7feffff               call       0x4c8af0
004c8c39 90                       nop        
004c8c3a 8be5                     mov        esp, ebp
004c8c3c 5d                       pop        ebp
004c8c3d c3                       ret        
004c8c3e cc                       int3       
004c8c3f cc                       int3       
