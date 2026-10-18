004bfb50 55                       push       ebp
004bfb51 8bec                     mov        ebp, esp
004bfb53 51                       push       ecx
004bfb54 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfb57 6a01                     push       1
004bfb59 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfb5c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfb5f 8b548170                 mov        edx, dword ptr [ecx + eax*4 + 0x70]
004bfb63 52                       push       edx
004bfb64 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004bfb6a e821f3f8ff               call       0x44ee90
004bfb6f 90                       nop        
004bfb70 8be5                     mov        esp, ebp
004bfb72 5d                       pop        ebp
004bfb73 c20400                   ret        4
004bfb76 cc                       int3       
004bfb77 cc                       int3       
004bfb78 cc                       int3       
004bfb79 cc                       int3       
004bfb7a cc                       int3       
004bfb7b cc                       int3       
004bfb7c cc                       int3       
004bfb7d cc                       int3       
004bfb7e cc                       int3       
004bfb7f cc                       int3       
