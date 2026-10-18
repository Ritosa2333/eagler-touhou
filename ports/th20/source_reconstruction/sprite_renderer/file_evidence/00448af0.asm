00448af0 55                       push       ebp
00448af1 8bec                     mov        ebp, esp
00448af3 51                       push       ecx
00448af4 894dfc                   mov        dword ptr [ebp - 4], ecx
00448af7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448afa c70000000000             mov        dword ptr [eax], 0
00448b00 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448b03 c7410400000000           mov        dword ptr [ecx + 4], 0
00448b0a 8b55fc                   mov        edx, dword ptr [ebp - 4]
00448b0d c7420800000000           mov        dword ptr [edx + 8], 0
00448b14 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448b17 c7400c00000000           mov        dword ptr [eax + 0xc], 0
00448b1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448b21 c7411000000000           mov        dword ptr [ecx + 0x10], 0
00448b28 33d2                     xor        edx, edx
00448b2a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448b2d 83c014                   add        eax, 0x14
00448b30 8910                     mov        dword ptr [eax], edx
00448b32 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448b35 8be5                     mov        esp, ebp
00448b37 5d                       pop        ebp
00448b38 c3                       ret        
00448b39 cc                       int3       
00448b3a cc                       int3       
00448b3b cc                       int3       
00448b3c cc                       int3       
00448b3d cc                       int3       
00448b3e cc                       int3       
00448b3f cc                       int3       
