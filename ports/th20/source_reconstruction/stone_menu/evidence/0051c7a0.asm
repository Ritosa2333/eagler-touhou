0051c7a0 55                       push       ebp
0051c7a1 8bec                     mov        ebp, esp
0051c7a3 51                       push       ecx
0051c7a4 894dfc                   mov        dword ptr [ebp - 4], ecx
0051c7a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051c7aa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051c7ad 8988fc100200             mov        dword ptr [eax + 0x210fc], ecx
0051c7b3 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051c7b6 8b82fc100200             mov        eax, dword ptr [edx + 0x210fc]
0051c7bc 50                       push       eax
0051c7bd b968a55b00               mov        ecx, 0x5ba568
0051c7c2 e8f9f3eeff               call       0x40bbc0
0051c7c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c7ca 898100110200             mov        dword ptr [ecx + 0x21100], eax
0051c7d0 8be5                     mov        esp, ebp
0051c7d2 5d                       pop        ebp
0051c7d3 c20400                   ret        4
0051c7d6 cc                       int3       
0051c7d7 cc                       int3       
0051c7d8 cc                       int3       
0051c7d9 cc                       int3       
0051c7da cc                       int3       
0051c7db cc                       int3       
0051c7dc cc                       int3       
0051c7dd cc                       int3       
0051c7de cc                       int3       
0051c7df cc                       int3       
