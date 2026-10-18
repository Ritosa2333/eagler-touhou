004a05d0 55                       push       ebp
004a05d1 8bec                     mov        ebp, esp
004a05d3 51                       push       ecx
004a05d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a05d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a05da 81c14ca20100             add        ecx, 0x1a24c
004a05e0 e85b8afdff               call       0x479040
004a05e5 6a00                     push       0
004a05e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a05ea 81c14ca20100             add        ecx, 0x1a24c
004a05f0 e8ab11f7ff               call       0x4117a0
004a05f5 90                       nop        
004a05f6 8be5                     mov        esp, ebp
004a05f8 5d                       pop        ebp
004a05f9 c3                       ret        
004a05fa cc                       int3       
004a05fb cc                       int3       
004a05fc cc                       int3       
004a05fd cc                       int3       
004a05fe cc                       int3       
004a05ff cc                       int3       
