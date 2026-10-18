004bfb80 55                       push       ebp
004bfb81 8bec                     mov        ebp, esp
004bfb83 51                       push       ecx
004bfb84 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfb87 6a03                     push       3
004bfb89 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfb8c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfb8f 8b548170                 mov        edx, dword ptr [ecx + eax*4 + 0x70]
004bfb93 52                       push       edx
004bfb94 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004bfb9a e821f4f8ff               call       0x44efc0
004bfb9f 90                       nop        
004bfba0 8be5                     mov        esp, ebp
004bfba2 5d                       pop        ebp
004bfba3 c20400                   ret        4
004bfba6 cc                       int3       
004bfba7 cc                       int3       
004bfba8 cc                       int3       
004bfba9 cc                       int3       
004bfbaa cc                       int3       
004bfbab cc                       int3       
004bfbac cc                       int3       
004bfbad cc                       int3       
004bfbae cc                       int3       
004bfbaf cc                       int3       
