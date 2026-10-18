004b8100 55                       push       ebp
004b8101 8bec                     mov        ebp, esp
004b8103 83ec0c                   sub        esp, 0xc
004b8106 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8109 c745f807000000           mov        dword ptr [ebp - 8], 7
004b8110 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b8117 8d45f8                   lea        eax, [ebp - 8]
004b811a 50                       push       eax
004b811b 8d4df4                   lea        ecx, [ebp - 0xc]
004b811e 51                       push       ecx
004b811f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8122 81c2bc000000             add        edx, 0xbc
004b8128 52                       push       edx
004b8129 e84290fbff               call       0x471170
004b812e 83c40c                   add        esp, 0xc
004b8131 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8134 8b10                     mov        edx, dword ptr [eax]
004b8136 8991bc000000             mov        dword ptr [ecx + 0xbc], edx
004b813c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b813f 8b80bc000000             mov        eax, dword ptr [eax + 0xbc]
004b8145 8be5                     mov        esp, ebp
004b8147 5d                       pop        ebp
004b8148 c3                       ret        
004b8149 cc                       int3       
004b814a cc                       int3       
004b814b cc                       int3       
004b814c cc                       int3       
004b814d cc                       int3       
004b814e cc                       int3       
004b814f cc                       int3       
