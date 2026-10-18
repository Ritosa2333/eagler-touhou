00414b50 55                       push       ebp
00414b51 8bec                     mov        ebp, esp
00414b53 51                       push       ecx
00414b54 894dfc                   mov        dword ptr [ebp - 4], ecx
00414b57 33c0                     xor        eax, eax
00414b59 8be5                     mov        esp, ebp
00414b5b 5d                       pop        ebp
00414b5c c20400                   ret        4
00414b5f cc                       int3       
