004250c0 55                       push       ebp
004250c1 8bec                     mov        ebp, esp
004250c3 6a00                     push       0
004250c5 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004250c8 50                       push       eax
004250c9 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
004250cc 51                       push       ecx
004250cd 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004250d0 52                       push       edx
004250d1 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004250d4 50                       push       eax
004250d5 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004250d8 51                       push       ecx
004250d9 8b5508                   mov        edx, dword ptr [ebp + 8]
004250dc 52                       push       edx
004250dd e80e000000               call       0x4250f0
004250e2 83c41c                   add        esp, 0x1c
004250e5 5d                       pop        ebp
004250e6 c3                       ret        
004250e7 cc                       int3       
004250e8 cc                       int3       
004250e9 cc                       int3       
004250ea cc                       int3       
004250eb cc                       int3       
004250ec cc                       int3       
004250ed cc                       int3       
004250ee cc                       int3       
004250ef cc                       int3       
