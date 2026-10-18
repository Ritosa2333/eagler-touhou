0044c590 55                       push       ebp
0044c591 8bec                     mov        ebp, esp
0044c593 83ec1c                   sub        esp, 0x1c
0044c596 894df8                   mov        dword ptr [ebp - 8], ecx
0044c599 c745fc00000000           mov        dword ptr [ebp - 4], 0
0044c5a0 8b45f8                   mov        eax, dword ptr [ebp - 8]
0044c5a3 8945f4                   mov        dword ptr [ebp - 0xc], eax
0044c5a6 6a00                     push       0
0044c5a8 8d4dec                   lea        ecx, [ebp - 0x14]
0044c5ab 51                       push       ecx
0044c5ac 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044c5af e80c22fcff               call       0x40e7c0
0044c5b4 8b5004                   mov        edx, dword ptr [eax + 4]
0044c5b7 52                       push       edx
0044c5b8 8b00                     mov        eax, dword ptr [eax]
0044c5ba 50                       push       eax
0044c5bb 8d4de4                   lea        ecx, [ebp - 0x1c]
0044c5be 51                       push       ecx
0044c5bf e86cf0ffff               call       0x44b630
0044c5c4 83c40c                   add        esp, 0xc
0044c5c7 50                       push       eax
0044c5c8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c5cb e8f0b7fcff               call       0x417dc0
0044c5d0 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044c5d3 83ca01                   or         edx, 1
0044c5d6 8955fc                   mov        dword ptr [ebp - 4], edx
0044c5d9 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c5dc 8be5                     mov        esp, ebp
0044c5de 5d                       pop        ebp
0044c5df c20400                   ret        4
0044c5e2 cc                       int3       
0044c5e3 cc                       int3       
0044c5e4 cc                       int3       
0044c5e5 cc                       int3       
0044c5e6 cc                       int3       
0044c5e7 cc                       int3       
0044c5e8 cc                       int3       
0044c5e9 cc                       int3       
0044c5ea cc                       int3       
0044c5eb cc                       int3       
0044c5ec cc                       int3       
0044c5ed cc                       int3       
0044c5ee cc                       int3       
0044c5ef cc                       int3       
