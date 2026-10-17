0052d040 55                       push       ebp
0052d041 8bec                     mov        ebp, esp
0052d043 51                       push       ecx
0052d044 894dfc                   mov        dword ptr [ebp - 4], ecx
0052d047 33c0                     xor        eax, eax
0052d049 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052d04c 8901                     mov        dword ptr [ecx], eax
0052d04e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052d051 e84a71eeff               call       0x4141a0
0052d056 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052d059 83c104                   add        ecx, 4
0052d05c e8af1cf8ff               call       0x4aed10
0052d061 90                       nop        
0052d062 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052d065 8be5                     mov        esp, ebp
0052d067 5d                       pop        ebp
0052d068 c20400                   ret        4
0052d06b cc                       int3       
0052d06c cc                       int3       
0052d06d cc                       int3       
0052d06e cc                       int3       
0052d06f cc                       int3       
