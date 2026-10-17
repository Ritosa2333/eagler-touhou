004bf8d0 55                       push       ebp
004bf8d1 8bec                     mov        ebp, esp
004bf8d3 51                       push       ecx
004bf8d4 6a00                     push       0
004bf8d6 a1244d5c00               mov        eax, dword ptr [0x5c4d24]
004bf8db 053c010000               add        eax, 0x13c
004bf8e0 50                       push       eax
004bf8e1 8b0d244d5c00             mov        ecx, dword ptr [0x5c4d24]
004bf8e7 81c1bc000000             add        ecx, 0xbc
004bf8ed 51                       push       ecx
004bf8ee e8ad11f5ff               call       0x410aa0
004bf8f3 83c40c                   add        esp, 0xc
004bf8f6 8945fc                   mov        dword ptr [ebp - 4], eax
004bf8f9 8b15244d5c00             mov        edx, dword ptr [0x5c4d24]
004bf8ff 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bf902 8982b4000000             mov        dword ptr [edx + 0xb4], eax
004bf908 8b0d244d5c00             mov        ecx, dword ptr [0x5c4d24]
004bf90e c781b800000003000000     mov        dword ptr [ecx + 0xb8], 3
004bf918 b9d05a5c00               mov        ecx, 0x5c5ad0
004bf91d e88eafffff               call       0x4ba8b0
004bf922 90                       nop        
004bf923 8be5                     mov        esp, ebp
004bf925 5d                       pop        ebp
004bf926 c3                       ret        
004bf927 cc                       int3       
004bf928 cc                       int3       
004bf929 cc                       int3       
004bf92a cc                       int3       
004bf92b cc                       int3       
004bf92c cc                       int3       
004bf92d cc                       int3       
004bf92e cc                       int3       
004bf92f cc                       int3       
