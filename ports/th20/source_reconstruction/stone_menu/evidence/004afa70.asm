004afa70 55                       push       ebp
004afa71 8bec                     mov        ebp, esp
004afa73 51                       push       ecx
004afa74 894dfc                   mov        dword ptr [ebp - 4], ecx
004afa77 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004afa7a 83c134                   add        ecx, 0x34
004afa7d e8ceffffff               call       0x4afa50
004afa82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004afa85 83c120                   add        ecx, 0x20
004afa88 e8c3ffffff               call       0x4afa50
004afa8d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004afa90 83c110                   add        ecx, 0x10
004afa93 e898bdfdff               call       0x48b830
004afa98 90                       nop        
004afa99 8be5                     mov        esp, ebp
004afa9b 5d                       pop        ebp
004afa9c c3                       ret        
004afa9d cc                       int3       
004afa9e cc                       int3       
004afa9f cc                       int3       
