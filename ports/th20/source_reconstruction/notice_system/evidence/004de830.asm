004de830 55                       push       ebp
004de831 8bec                     mov        ebp, esp
004de833 83ec0c                   sub        esp, 0xc
004de836 894dfc                   mov        dword ptr [ebp - 4], ecx
004de839 8b4508                   mov        eax, dword ptr [ebp + 8]
004de83c 50                       push       eax
004de83d e8ce50f3ff               call       0x413910
004de842 83c404                   add        esp, 4
004de845 0fb6c8                   movzx      ecx, al
004de848 85c9                     test       ecx, ecx
004de84a 7502                     jne        0x4de84e
004de84c eb36                     jmp        0x4de884
004de84e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004de851 52                       push       edx
004de852 6a08                     push       8
004de854 e8a7fef2ff               call       0x40e700
004de859 83c408                   add        esp, 8
004de85c 8945f4                   mov        dword ptr [ebp - 0xc], eax
004de85f 8b4508                   mov        eax, dword ptr [ebp + 8]
004de862 50                       push       eax
004de863 e818caf2ff               call       0x40b280
004de868 83c404                   add        esp, 4
004de86b 8945f8                   mov        dword ptr [ebp - 8], eax
004de86e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004de871 51                       push       ecx
004de872 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de875 e896fcffff               call       0x4de510
004de87a 50                       push       eax
004de87b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de87e e8dd63f3ff               call       0x414c60
004de883 90                       nop        
004de884 8be5                     mov        esp, ebp
004de886 5d                       pop        ebp
004de887 c20400                   ret        4
004de88a cc                       int3       
004de88b cc                       int3       
004de88c cc                       int3       
004de88d cc                       int3       
004de88e cc                       int3       
004de88f cc                       int3       
