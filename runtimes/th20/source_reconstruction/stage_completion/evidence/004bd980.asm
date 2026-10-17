004bd980 55                       push       ebp
004bd981 8bec                     mov        ebp, esp
004bd983 51                       push       ecx
004bd984 894dfc                   mov        dword ptr [ebp - 4], ecx
004bd987 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd98a 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004bd990 81c980000000             or         ecx, 0x80
004bd996 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bd999 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004bd99f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd9a2 c7800c01000000000000     mov        dword ptr [eax + 0x10c], 0
004bd9ac 8be5                     mov        esp, ebp
004bd9ae 5d                       pop        ebp
004bd9af c3                       ret        
