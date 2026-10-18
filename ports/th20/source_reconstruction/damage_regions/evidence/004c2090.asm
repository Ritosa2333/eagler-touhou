004c2090 55                       push       ebp
004c2091 8bec                     mov        ebp, esp
004c2093 51                       push       ecx
004c2094 894dfc                   mov        dword ptr [ebp - 4], ecx
004c2097 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c209a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c209d 898858c40000             mov        dword ptr [eax + 0xc458], ecx
004c20a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c20a6 8b8258c40000             mov        eax, dword ptr [edx + 0xc458]
004c20ac 50                       push       eax
004c20ad b968a55b00               mov        ecx, 0x5ba568
004c20b2 e8099bf4ff               call       0x40bbc0
004c20b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c20ba 89815cc40000             mov        dword ptr [ecx + 0xc45c], eax
004c20c0 8be5                     mov        esp, ebp
004c20c2 5d                       pop        ebp
004c20c3 c20400                   ret        4
004c20c6 cc                       int3       
004c20c7 cc                       int3       
004c20c8 cc                       int3       
004c20c9 cc                       int3       
004c20ca cc                       int3       
004c20cb cc                       int3       
004c20cc cc                       int3       
004c20cd cc                       int3       
004c20ce cc                       int3       
004c20cf cc                       int3       
