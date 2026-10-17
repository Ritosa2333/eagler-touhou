0047a4c0 55                       push       ebp
0047a4c1 8bec                     mov        ebp, esp
0047a4c3 51                       push       ecx
0047a4c4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a4c7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a4ca 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047a4cd 83e1f0                   and        ecx, 0xfffffff0
0047a4d0 83c902                   or         ecx, 2
0047a4d3 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a4d6 894a44                   mov        dword ptr [edx + 0x44], ecx
0047a4d9 8be5                     mov        esp, ebp
0047a4db 5d                       pop        ebp
0047a4dc c3                       ret        
0047a4dd cc                       int3       
0047a4de cc                       int3       
0047a4df cc                       int3       
