004c9300 55                       push       ebp
004c9301 8bec                     mov        ebp, esp
004c9303 83ec0c                   sub        esp, 0xc
004c9306 894dfc                   mov        dword ptr [ebp - 4], ecx
004c9309 8b4508                   mov        eax, dword ptr [ebp + 8]
004c930c 83c008                   add        eax, 8
004c930f 8945f8                   mov        dword ptr [ebp - 8], eax
004c9312 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9315 51                       push       ecx
004c9316 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c9319 e85213f8ff               call       0x44a670
004c931e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c9321 83c210                   add        edx, 0x10
004c9324 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c9327 8b4508                   mov        eax, dword ptr [ebp + 8]
004c932a 83c008                   add        eax, 8
004c932d 50                       push       eax
004c932e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c9331 e87a13f8ff               call       0x44a6b0
004c9336 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9339 8b5128                   mov        edx, dword ptr [ecx + 0x28]
004c933c 83c201                   add        edx, 1
004c933f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c9342 895028                   mov        dword ptr [eax + 0x28], edx
004c9345 8be5                     mov        esp, ebp
004c9347 5d                       pop        ebp
004c9348 c20400                   ret        4
004c934b cc                       int3       
004c934c cc                       int3       
004c934d cc                       int3       
004c934e cc                       int3       
004c934f cc                       int3       
