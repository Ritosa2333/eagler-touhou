00510890 55                       push       ebp
00510891 8bec                     mov        ebp, esp
00510893 51                       push       ecx
00510894 894dfc                   mov        dword ptr [ebp - 4], ecx
00510897 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051089a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051089d 8988900f0000             mov        dword ptr [eax + 0xf90], ecx
005108a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
005108a6 8b82900f0000             mov        eax, dword ptr [edx + 0xf90]
005108ac 50                       push       eax
005108ad b968a55b00               mov        ecx, 0x5ba568
005108b2 e809b3efff               call       0x40bbc0
005108b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005108ba 8981940f0000             mov        dword ptr [ecx + 0xf94], eax
005108c0 8be5                     mov        esp, ebp
005108c2 5d                       pop        ebp
005108c3 c20400                   ret        4
005108c6 cc                       int3       
005108c7 cc                       int3       
005108c8 cc                       int3       
005108c9 cc                       int3       
005108ca cc                       int3       
005108cb cc                       int3       
005108cc cc                       int3       
005108cd cc                       int3       
005108ce cc                       int3       
005108cf cc                       int3       
