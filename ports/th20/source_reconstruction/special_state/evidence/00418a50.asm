00418a50 55                       push       ebp
00418a51 8bec                     mov        ebp, esp
00418a53 51                       push       ecx
00418a54 894dfc                   mov        dword ptr [ebp - 4], ecx
00418a57 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00418a5a e8b1ffffff               call       0x418a10
00418a5f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00418a65 894114                   mov        dword ptr [ecx + 0x14], eax
00418a68 8b55fc                   mov        edx, dword ptr [ebp - 4]
00418a6b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a6e 895014                   mov        dword ptr [eax + 0x14], edx
00418a71 8b45fc                   mov        eax, dword ptr [ebp - 4]
00418a74 8be5                     mov        esp, ebp
00418a76 5d                       pop        ebp
00418a77 c3                       ret        
00418a78 cc                       int3       
00418a79 cc                       int3       
00418a7a cc                       int3       
00418a7b cc                       int3       
00418a7c cc                       int3       
00418a7d cc                       int3       
00418a7e cc                       int3       
00418a7f cc                       int3       
