004de890 55                       push       ebp
004de891 8bec                     mov        ebp, esp
004de893 83ec0c                   sub        esp, 0xc
004de896 894dfc                   mov        dword ptr [ebp - 4], ecx
004de899 8b4508                   mov        eax, dword ptr [ebp + 8]
004de89c 50                       push       eax
004de89d e86e50f3ff               call       0x413910
004de8a2 83c404                   add        esp, 4
004de8a5 0fb6c8                   movzx      ecx, al
004de8a8 85c9                     test       ecx, ecx
004de8aa 7502                     jne        0x4de8ae
004de8ac eb36                     jmp        0x4de8e4
004de8ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
004de8b1 52                       push       edx
004de8b2 6a08                     push       8
004de8b4 e847fef2ff               call       0x40e700
004de8b9 83c408                   add        esp, 8
004de8bc 8945f4                   mov        dword ptr [ebp - 0xc], eax
004de8bf 8b4508                   mov        eax, dword ptr [ebp + 8]
004de8c2 50                       push       eax
004de8c3 e8b8c9f2ff               call       0x40b280
004de8c8 83c404                   add        esp, 4
004de8cb 8945f8                   mov        dword ptr [ebp - 8], eax
004de8ce 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004de8d1 51                       push       ecx
004de8d2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de8d5 e866fcffff               call       0x4de540
004de8da 50                       push       eax
004de8db 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de8de e87d63f3ff               call       0x414c60
004de8e3 90                       nop        
004de8e4 8be5                     mov        esp, ebp
004de8e6 5d                       pop        ebp
004de8e7 c20400                   ret        4
004de8ea cc                       int3       
004de8eb cc                       int3       
004de8ec cc                       int3       
004de8ed cc                       int3       
004de8ee cc                       int3       
004de8ef cc                       int3       
