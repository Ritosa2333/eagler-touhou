004de8f0 55                       push       ebp
004de8f1 8bec                     mov        ebp, esp
004de8f3 83ec0c                   sub        esp, 0xc
004de8f6 894dfc                   mov        dword ptr [ebp - 4], ecx
004de8f9 8b4508                   mov        eax, dword ptr [ebp + 8]
004de8fc 50                       push       eax
004de8fd e80e50f3ff               call       0x413910
004de902 83c404                   add        esp, 4
004de905 0fb6c8                   movzx      ecx, al
004de908 85c9                     test       ecx, ecx
004de90a 7502                     jne        0x4de90e
004de90c eb36                     jmp        0x4de944
004de90e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004de911 52                       push       edx
004de912 6a08                     push       8
004de914 e8e7fdf2ff               call       0x40e700
004de919 83c408                   add        esp, 8
004de91c 8945f4                   mov        dword ptr [ebp - 0xc], eax
004de91f 8b4508                   mov        eax, dword ptr [ebp + 8]
004de922 50                       push       eax
004de923 e858c9f2ff               call       0x40b280
004de928 83c404                   add        esp, 4
004de92b 8945f8                   mov        dword ptr [ebp - 8], eax
004de92e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004de931 51                       push       ecx
004de932 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de935 e836fcffff               call       0x4de570
004de93a 50                       push       eax
004de93b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de93e e81d63f3ff               call       0x414c60
004de943 90                       nop        
004de944 8be5                     mov        esp, ebp
004de946 5d                       pop        ebp
004de947 c20400                   ret        4
004de94a cc                       int3       
004de94b cc                       int3       
004de94c cc                       int3       
004de94d cc                       int3       
004de94e cc                       int3       
004de94f cc                       int3       
