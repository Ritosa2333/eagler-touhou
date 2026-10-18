00414b60 55                       push       ebp
00414b61 8bec                     mov        ebp, esp
00414b63 51                       push       ecx
00414b64 894dfc                   mov        dword ptr [ebp - 4], ecx
00414b67 8be5                     mov        esp, ebp
00414b69 5d                       pop        ebp
00414b6a c20800                   ret        8
00414b6d cc                       int3       
00414b6e cc                       int3       
00414b6f cc                       int3       
