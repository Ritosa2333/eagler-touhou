004de950 55                       push       ebp
004de951 8bec                     mov        ebp, esp
004de953 83ec0c                   sub        esp, 0xc
004de956 894dfc                   mov        dword ptr [ebp - 4], ecx
004de959 8b4508                   mov        eax, dword ptr [ebp + 8]
004de95c 50                       push       eax
004de95d e8ae4ff3ff               call       0x413910
004de962 83c404                   add        esp, 4
004de965 0fb6c8                   movzx      ecx, al
004de968 85c9                     test       ecx, ecx
004de96a 7502                     jne        0x4de96e
004de96c eb36                     jmp        0x4de9a4
004de96e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004de971 52                       push       edx
004de972 6a08                     push       8
004de974 e887fdf2ff               call       0x40e700
004de979 83c408                   add        esp, 8
004de97c 8945f4                   mov        dword ptr [ebp - 0xc], eax
004de97f 8b4508                   mov        eax, dword ptr [ebp + 8]
004de982 50                       push       eax
004de983 e8f8c8f2ff               call       0x40b280
004de988 83c404                   add        esp, 4
004de98b 8945f8                   mov        dword ptr [ebp - 8], eax
004de98e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004de991 51                       push       ecx
004de992 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de995 e806fcffff               call       0x4de5a0
004de99a 50                       push       eax
004de99b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de99e e8bd62f3ff               call       0x414c60
004de9a3 90                       nop        
004de9a4 8be5                     mov        esp, ebp
004de9a6 5d                       pop        ebp
004de9a7 c20400                   ret        4
004de9aa cc                       int3       
004de9ab cc                       int3       
004de9ac cc                       int3       
004de9ad cc                       int3       
004de9ae cc                       int3       
004de9af cc                       int3       
