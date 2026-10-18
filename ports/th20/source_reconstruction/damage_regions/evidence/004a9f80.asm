004a9f80 55                       push       ebp
004a9f81 8bec                     mov        ebp, esp
004a9f83 83ec08                   sub        esp, 8
004a9f86 894dfc                   mov        dword ptr [ebp - 4], ecx
004a9f89 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a9f8c 83c038                   add        eax, 0x38
004a9f8f 8945f8                   mov        dword ptr [ebp - 8], eax
004a9f92 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004a9f95 51                       push       ecx
004a9f96 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004a9f99 52                       push       edx
004a9f9a 8b4508                   mov        eax, dword ptr [ebp + 8]
004a9f9d 50                       push       eax
004a9f9e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004a9fa1 e87a970800               call       0x533720
004a9fa6 90                       nop        
004a9fa7 8be5                     mov        esp, ebp
004a9fa9 5d                       pop        ebp
004a9faa c20c00                   ret        0xc
004a9fad cc                       int3       
004a9fae cc                       int3       
004a9faf cc                       int3       
