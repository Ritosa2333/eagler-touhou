0047a4a0 55                       push       ebp
0047a4a1 8bec                     mov        ebp, esp
0047a4a3 51                       push       ecx
0047a4a4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a4a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a4aa 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0047a4ad 83c910                   or         ecx, 0x10
0047a4b0 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a4b3 894a14                   mov        dword ptr [edx + 0x14], ecx
0047a4b6 8be5                     mov        esp, ebp
0047a4b8 5d                       pop        ebp
0047a4b9 c3                       ret        
0047a4ba cc                       int3       
0047a4bb cc                       int3       
0047a4bc cc                       int3       
0047a4bd cc                       int3       
0047a4be cc                       int3       
0047a4bf cc                       int3       
