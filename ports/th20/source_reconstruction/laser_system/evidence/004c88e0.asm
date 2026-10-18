004c88e0 55                       push       ebp
004c88e1 8bec                     mov        ebp, esp
004c88e3 51                       push       ecx
004c88e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c88e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c88ea e8c1fcffff               call       0x4c85b0
004c88ef 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c88f2 c70048115700             mov        dword ptr [eax], 0x571148
004c88f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c88fb 81c1f8060000             add        ecx, 0x6f8
004c8901 e82a38fbff               call       0x47c130
004c8906 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8909 81c14c070000             add        ecx, 0x74c
004c890f e82c02f8ff               call       0x448b40
004c8914 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8917 81c1300d0000             add        ecx, 0xd30
004c891d e81e02f8ff               call       0x448b40
004c8922 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8925 81c114130000             add        ecx, 0x1314
004c892b e81002f8ff               call       0x448b40
004c8930 90                       nop        
004c8931 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8934 8be5                     mov        esp, ebp
004c8936 5d                       pop        ebp
004c8937 c3                       ret        
004c8938 cc                       int3       
004c8939 cc                       int3       
004c893a cc                       int3       
004c893b cc                       int3       
004c893c cc                       int3       
004c893d cc                       int3       
004c893e cc                       int3       
004c893f cc                       int3       
