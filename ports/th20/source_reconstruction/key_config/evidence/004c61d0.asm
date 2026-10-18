004c61d0 55                       push       ebp
004c61d1 8bec                     mov        ebp, esp
004c61d3 51                       push       ecx
004c61d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c61d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c61da 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c61dd 8988fc000000             mov        dword ptr [eax + 0xfc], ecx
004c61e3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c61e6 c7820001000000000000     mov        dword ptr [edx + 0x100], 0
004c61f0 6a00                     push       0
004c61f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c61f5 81c1c0000000             add        ecx, 0xc0
004c61fb e820d3f5ff               call       0x423520
004c6200 90                       nop        
004c6201 8be5                     mov        esp, ebp
004c6203 5d                       pop        ebp
004c6204 c20400                   ret        4
004c6207 cc                       int3       
004c6208 cc                       int3       
004c6209 cc                       int3       
004c620a cc                       int3       
004c620b cc                       int3       
004c620c cc                       int3       
004c620d cc                       int3       
004c620e cc                       int3       
004c620f cc                       int3       
