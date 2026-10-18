0045eac0 55                       push       ebp
0045eac1 8bec                     mov        ebp, esp
0045eac3 83ec10                   sub        esp, 0x10
0045eac6 894dfc                   mov        dword ptr [ebp - 4], ecx
0045eac9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045eacc 05e0010000               add        eax, 0x1e0
0045ead1 8945f8                   mov        dword ptr [ebp - 8], eax
0045ead4 6a00                     push       0
0045ead6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045ead9 e8a25afbff               call       0x414580
0045eade 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045eae1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045eae4 e8b7d2faff               call       0x40bda0
0045eae9 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045eaec 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045eaef 51                       push       ecx
0045eaf0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0045eaf3 52                       push       edx
0045eaf4 8b4508                   mov        eax, dword ptr [ebp + 8]
0045eaf7 50                       push       eax
0045eaf8 6a3c                     push       0x3c
0045eafa 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0045eb00 e89be6fdff               call       0x43d1a0
0045eb05 90                       nop        
0045eb06 8be5                     mov        esp, ebp
0045eb08 5d                       pop        ebp
0045eb09 c20400                   ret        4
0045eb0c cc                       int3       
0045eb0d cc                       int3       
0045eb0e cc                       int3       
0045eb0f cc                       int3       
