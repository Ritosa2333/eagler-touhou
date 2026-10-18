00478290 55                       push       ebp
00478291 8bec                     mov        ebp, esp
00478293 51                       push       ecx
00478294 894dfc                   mov        dword ptr [ebp - 4], ecx
00478297 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047829a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047829d 8988b0000000             mov        dword ptr [eax + 0xb0], ecx
004782a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004782a6 8b82b0000000             mov        eax, dword ptr [edx + 0xb0]
004782ac 50                       push       eax
004782ad b968a55b00               mov        ecx, 0x5ba568
004782b2 e80939f9ff               call       0x40bbc0
004782b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004782ba 8981b4000000             mov        dword ptr [ecx + 0xb4], eax
004782c0 8be5                     mov        esp, ebp
004782c2 5d                       pop        ebp
004782c3 c20400                   ret        4
004782c6 cc                       int3       
004782c7 cc                       int3       
004782c8 cc                       int3       
004782c9 cc                       int3       
004782ca cc                       int3       
004782cb cc                       int3       
004782cc cc                       int3       
004782cd cc                       int3       
004782ce cc                       int3       
004782cf cc                       int3       
