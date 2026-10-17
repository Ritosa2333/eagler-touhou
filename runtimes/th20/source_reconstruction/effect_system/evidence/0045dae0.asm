0045dae0 55                       push       ebp
0045dae1 8bec                     mov        ebp, esp
0045dae3 83ec10                   sub        esp, 0x10
0045dae6 894dfc                   mov        dword ptr [ebp - 4], ecx
0045dae9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045daec 05e0010000               add        eax, 0x1e0
0045daf1 8945f8                   mov        dword ptr [ebp - 8], eax
0045daf4 6a00                     push       0
0045daf6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045daf9 e8826afbff               call       0x414580
0045dafe 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045db01 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045db04 e897e2faff               call       0x40bda0
0045db09 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045db0c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045db0f 51                       push       ecx
0045db10 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0045db13 52                       push       edx
0045db14 8b4508                   mov        eax, dword ptr [ebp + 8]
0045db17 50                       push       eax
0045db18 6a3c                     push       0x3c
0045db1a 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0045db20 e84bf0fdff               call       0x43cb70
0045db25 90                       nop        
0045db26 8be5                     mov        esp, ebp
0045db28 5d                       pop        ebp
0045db29 c20400                   ret        4
0045db2c cc                       int3       
0045db2d cc                       int3       
0045db2e cc                       int3       
0045db2f cc                       int3       
