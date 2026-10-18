004b8620 55                       push       ebp
004b8621 8bec                     mov        ebp, esp
004b8623 51                       push       ecx
004b8624 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8627 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b862a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004b862d 8988f4a10100             mov        dword ptr [eax + 0x1a1f4], ecx
004b8633 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8636 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004b8639 8982f8a10100             mov        dword ptr [edx + 0x1a1f8], eax
004b863f 8be5                     mov        esp, ebp
004b8641 5d                       pop        ebp
004b8642 c20800                   ret        8
004b8645 cc                       int3       
004b8646 cc                       int3       
004b8647 cc                       int3       
004b8648 cc                       int3       
004b8649 cc                       int3       
004b864a cc                       int3       
004b864b cc                       int3       
004b864c cc                       int3       
004b864d cc                       int3       
004b864e cc                       int3       
004b864f cc                       int3       
