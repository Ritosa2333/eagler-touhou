004c8b70 55                       push       ebp
004c8b71 8bec                     mov        ebp, esp
004c8b73 6aff                     push       -1
004c8b75 6800765600               push       0x567600
004c8b7a 64a100000000             mov        eax, dword ptr fs:[0]
004c8b80 50                       push       eax
004c8b81 51                       push       ecx
004c8b82 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c8b87 33c5                     xor        eax, ebp
004c8b89 50                       push       eax
004c8b8a 8d45f4                   lea        eax, [ebp - 0xc]
004c8b8d 64a300000000             mov        dword ptr fs:[0], eax
004c8b93 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c8b96 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8b99 c70000145700             mov        dword ptr [eax], 0x571400
004c8b9f 6824145700               push       0x571424
004c8ba4 e8073bf4ff               call       0x40c6b0
004c8ba9 83c404                   add        esp, 4
004c8bac 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8baf 8b5108                   mov        edx, dword ptr [ecx + 8]
004c8bb2 52                       push       edx
004c8bb3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c8bb9 e8f298f4ff               call       0x4124b0
004c8bbe 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8bc1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004c8bc4 51                       push       ecx
004c8bc5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c8bcb e8e098f4ff               call       0x4124b0
004c8bd0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8bd3 e8a838ffff               call       0x4bc480
004c8bd8 90                       nop        
004c8bd9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8bdc e80f72f5ff               call       0x41fdf0
004c8be1 90                       nop        
004c8be2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c8be5 64890d00000000           mov        dword ptr fs:[0], ecx
004c8bec 59                       pop        ecx
004c8bed 8be5                     mov        esp, ebp
004c8bef 5d                       pop        ebp
004c8bf0 c3                       ret        
004c8bf1 cc                       int3       
004c8bf2 cc                       int3       
004c8bf3 cc                       int3       
004c8bf4 cc                       int3       
004c8bf5 cc                       int3       
004c8bf6 cc                       int3       
004c8bf7 cc                       int3       
004c8bf8 cc                       int3       
004c8bf9 cc                       int3       
004c8bfa cc                       int3       
004c8bfb cc                       int3       
004c8bfc cc                       int3       
004c8bfd cc                       int3       
004c8bfe cc                       int3       
004c8bff cc                       int3       
