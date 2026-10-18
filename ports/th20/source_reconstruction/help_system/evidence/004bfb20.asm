004bfb20 55                       push       ebp
004bfb21 8bec                     mov        ebp, esp
004bfb23 51                       push       ecx
004bfb24 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfb27 6a02                     push       2
004bfb29 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfb2c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfb2f 8b548170                 mov        edx, dword ptr [ecx + eax*4 + 0x70]
004bfb33 52                       push       edx
004bfb34 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004bfb3a e881f4f8ff               call       0x44efc0
004bfb3f 90                       nop        
004bfb40 8be5                     mov        esp, ebp
004bfb42 5d                       pop        ebp
004bfb43 c20400                   ret        4
004bfb46 cc                       int3       
004bfb47 cc                       int3       
004bfb48 cc                       int3       
004bfb49 cc                       int3       
004bfb4a cc                       int3       
004bfb4b cc                       int3       
004bfb4c cc                       int3       
004bfb4d cc                       int3       
004bfb4e cc                       int3       
004bfb4f cc                       int3       
