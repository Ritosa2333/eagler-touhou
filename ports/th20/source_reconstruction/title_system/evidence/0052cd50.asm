0052cd50 55                       push       ebp
0052cd51 8bec                     mov        ebp, esp
0052cd53 83ec08                   sub        esp, 8
0052cd56 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cd59 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cd5c 83c008                   add        eax, 8
0052cd5f 8945f8                   mov        dword ptr [ebp - 8], eax
0052cd62 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cd65 51                       push       ecx
0052cd66 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cd69 e89223f2ff               call       0x44f100
0052cd6e 90                       nop        
0052cd6f 8be5                     mov        esp, ebp
0052cd71 5d                       pop        ebp
0052cd72 c20400                   ret        4
0052cd75 cc                       int3       
0052cd76 cc                       int3       
0052cd77 cc                       int3       
0052cd78 cc                       int3       
0052cd79 cc                       int3       
0052cd7a cc                       int3       
0052cd7b cc                       int3       
0052cd7c cc                       int3       
0052cd7d cc                       int3       
0052cd7e cc                       int3       
0052cd7f cc                       int3       
