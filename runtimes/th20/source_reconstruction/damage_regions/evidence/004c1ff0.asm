004c1ff0 55                       push       ebp
004c1ff1 8bec                     mov        ebp, esp
004c1ff3 83ec08                   sub        esp, 8
004c1ff6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1ff9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1ffc e80feeffff               call       0x4c0e10
004c2001 85c0                     test       eax, eax
004c2003 7418                     je         0x4c201d
004c2005 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2008 e803eeffff               call       0x4c0e10
004c200d 8945f8                   mov        dword ptr [ebp - 8], eax
004c2010 8b4508                   mov        eax, dword ptr [ebp + 8]
004c2013 50                       push       eax
004c2014 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c2017 e814000000               call       0x4c2030
004c201c 90                       nop        
004c201d 8be5                     mov        esp, ebp
004c201f 5d                       pop        ebp
004c2020 c20400                   ret        4
004c2023 cc                       int3       
004c2024 cc                       int3       
004c2025 cc                       int3       
004c2026 cc                       int3       
004c2027 cc                       int3       
004c2028 cc                       int3       
004c2029 cc                       int3       
004c202a cc                       int3       
004c202b cc                       int3       
004c202c cc                       int3       
004c202d cc                       int3       
004c202e cc                       int3       
004c202f cc                       int3       
