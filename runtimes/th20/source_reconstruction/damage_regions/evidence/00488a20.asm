00488a20 55                       push       ebp
00488a21 8bec                     mov        ebp, esp
00488a23 51                       push       ecx
00488a24 894dfc                   mov        dword ptr [ebp - 4], ecx
00488a27 8b45fc                   mov        eax, dword ptr [ebp - 4]
00488a2a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00488a2d 8988bc000000             mov        dword ptr [eax + 0xbc], ecx
00488a33 8b55fc                   mov        edx, dword ptr [ebp - 4]
00488a36 8b82bc000000             mov        eax, dword ptr [edx + 0xbc]
00488a3c 50                       push       eax
00488a3d b968a55b00               mov        ecx, 0x5ba568
00488a42 e87931f8ff               call       0x40bbc0
00488a47 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00488a4a 8981c0000000             mov        dword ptr [ecx + 0xc0], eax
00488a50 8be5                     mov        esp, ebp
00488a52 5d                       pop        ebp
00488a53 c20400                   ret        4
00488a56 cc                       int3       
00488a57 cc                       int3       
00488a58 cc                       int3       
00488a59 cc                       int3       
00488a5a cc                       int3       
00488a5b cc                       int3       
00488a5c cc                       int3       
00488a5d cc                       int3       
00488a5e cc                       int3       
00488a5f cc                       int3       
