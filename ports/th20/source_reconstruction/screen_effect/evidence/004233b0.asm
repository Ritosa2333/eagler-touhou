004233b0 55                       push       ebp
004233b1 8bec                     mov        ebp, esp
004233b3 6aff                     push       -1
004233b5 6800765600               push       0x567600
004233ba 64a100000000             mov        eax, dword ptr fs:[0]
004233c0 50                       push       eax
004233c1 51                       push       ecx
004233c2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004233c7 33c5                     xor        eax, ebp
004233c9 50                       push       eax
004233ca 8d45f4                   lea        eax, [ebp - 0xc]
004233cd 64a300000000             mov        dword ptr fs:[0], eax
004233d3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004233d6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004233d9 e842c9ffff               call       0x41fd20
004233de 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004233e1 c70044d75600             mov        dword ptr [eax], 0x56d744
004233e7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004233ea c7411000000000           mov        dword ptr [ecx + 0x10], 0
004233f1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004233f4 c7421400000000           mov        dword ptr [edx + 0x14], 0
004233fb 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004233fe c7401800000000           mov        dword ptr [eax + 0x18], 0
00423405 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00423408 c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
0042340f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00423412 c7422000000000           mov        dword ptr [edx + 0x20], 0
00423419 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0042341c c7402400000000           mov        dword ptr [eax + 0x24], 0
00423423 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00423426 c7412800000000           mov        dword ptr [ecx + 0x28], 0
0042342d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00423430 c7422c00000000           mov        dword ptr [edx + 0x2c], 0
00423437 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0042343a 83c130                   add        ecx, 0x30
0042343d e84ef9ffff               call       0x422d90
00423442 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00423445 c7404000000000           mov        dword ptr [eax + 0x40], 0
0042344c 6850d75600               push       0x56d750
00423451 e85a92feff               call       0x40c6b0
00423456 83c404                   add        esp, 4
00423459 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0042345c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0042345f 64890d00000000           mov        dword ptr fs:[0], ecx
00423466 59                       pop        ecx
00423467 8be5                     mov        esp, ebp
00423469 5d                       pop        ebp
0042346a c3                       ret        
0042346b cc                       int3       
0042346c cc                       int3       
0042346d cc                       int3       
0042346e cc                       int3       
0042346f cc                       int3       
