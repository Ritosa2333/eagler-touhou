0047a370 55                       push       ebp
0047a371 8bec                     mov        ebp, esp
0047a373 83ec08                   sub        esp, 8
0047a376 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a379 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a37c 8b88c8020000             mov        ecx, dword ptr [eax + 0x2c8]
0047a382 83e101                   and        ecx, 1
0047a385 753c                     jne        0x47a3c3
0047a387 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a38a 8b82c8020000             mov        eax, dword ptr [edx + 0x2c8]
0047a390 c1e805                   shr        eax, 5
0047a393 83e001                   and        eax, 1
0047a396 752b                     jne        0x47a3c3
0047a398 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a39b 8b91cc020000             mov        edx, dword ptr [ecx + 0x2cc]
0047a3a1 c1ea0b                   shr        edx, 0xb
0047a3a4 83e201                   and        edx, 1
0047a3a7 751a                     jne        0x47a3c3
0047a3a9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a3ac 8b88cc020000             mov        ecx, dword ptr [eax + 0x2cc]
0047a3b2 c1e90a                   shr        ecx, 0xa
0047a3b5 83e101                   and        ecx, 1
0047a3b8 7509                     jne        0x47a3c3
0047a3ba c745f800000000           mov        dword ptr [ebp - 8], 0
0047a3c1 eb07                     jmp        0x47a3ca
0047a3c3 c745f801000000           mov        dword ptr [ebp - 8], 1
0047a3ca 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047a3cd 8be5                     mov        esp, ebp
0047a3cf 5d                       pop        ebp
0047a3d0 c3                       ret        
0047a3d1 cc                       int3       
0047a3d2 cc                       int3       
0047a3d3 cc                       int3       
0047a3d4 cc                       int3       
0047a3d5 cc                       int3       
0047a3d6 cc                       int3       
0047a3d7 cc                       int3       
0047a3d8 cc                       int3       
0047a3d9 cc                       int3       
0047a3da cc                       int3       
0047a3db cc                       int3       
0047a3dc cc                       int3       
0047a3dd cc                       int3       
0047a3de cc                       int3       
0047a3df cc                       int3       
