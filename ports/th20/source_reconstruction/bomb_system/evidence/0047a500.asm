0047a500 55                       push       ebp
0047a501 8bec                     mov        ebp, esp
0047a503 51                       push       ecx
0047a504 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a507 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a50a 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047a50d 83e1f0                   and        ecx, 0xfffffff0
0047a510 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a513 894a44                   mov        dword ptr [edx + 0x44], ecx
0047a516 8be5                     mov        esp, ebp
0047a518 5d                       pop        ebp
0047a519 c3                       ret        
0047a51a cc                       int3       
0047a51b cc                       int3       
0047a51c cc                       int3       
0047a51d cc                       int3       
0047a51e cc                       int3       
0047a51f cc                       int3       
