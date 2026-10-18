00418a10 55                       push       ebp
00418a11 8bec                     mov        ebp, esp
00418a13 51                       push       ecx
00418a14 894dfc                   mov        dword ptr [ebp - 4], ecx
00418a17 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a1a c70000000000             mov        dword ptr [eax], 0
00418a20 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00418a23 c7410400000000           mov        dword ptr [ecx + 4], 0
00418a2a 8b55fc                   mov        edx, dword ptr [ebp - 4]
00418a2d c7420800000000           mov        dword ptr [edx + 8], 0
00418a34 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a37 c7400c00000000           mov        dword ptr [eax + 0xc], 0
00418a3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00418a41 c7411000000000           mov        dword ptr [ecx + 0x10], 0
00418a48 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a4b 8be5                     mov        esp, ebp
00418a4d 5d                       pop        ebp
00418a4e c3                       ret        
00418a4f cc                       int3       
