004bd8f0 55                       push       ebp
004bd8f1 8bec                     mov        ebp, esp
004bd8f3 83ec08                   sub        esp, 8
004bd8f6 894dfc                   mov        dword ptr [ebp - 4], ecx
004bd8f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd8fc 05bc000000               add        eax, 0xbc
004bd901 8945f8                   mov        dword ptr [ebp - 8], eax
004bd904 6a00                     push       0
004bd906 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bd909 e8726cf5ff               call       0x414580
004bd90e 8bc8                     mov        ecx, eax
004bd910 e82bb7fbff               call       0x479040
004bd915 90                       nop        
004bd916 8be5                     mov        esp, ebp
004bd918 5d                       pop        ebp
004bd919 c3                       ret        
004bd91a cc                       int3       
004bd91b cc                       int3       
004bd91c cc                       int3       
004bd91d cc                       int3       
004bd91e cc                       int3       
004bd91f cc                       int3       
