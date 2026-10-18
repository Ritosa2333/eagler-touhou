004b64a0 55                       push       ebp
004b64a1 8bec                     mov        ebp, esp
004b64a3 51                       push       ecx
004b64a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004b64a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b64aa e8b11cfcff               call       0x478160
004b64af 85c0                     test       eax, eax
004b64b1 742f                     je         0x4b64e2
004b64b3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b64b6 8b88bc010000             mov        ecx, dword ptr [eax + 0x1bc]
004b64bc 51                       push       ecx
004b64bd 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b64c3 e86883ffff               call       0x4ae830
004b64c8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b64cb c782bc01000000000000     mov        dword ptr [edx + 0x1bc], 0
004b64d5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b64d8 c780bc01000000000000     mov        dword ptr [eax + 0x1bc], 0
004b64e2 b968a55b00               mov        ecx, 0x5ba568
004b64e7 e814e9fbff               call       0x474e00
004b64ec 85c0                     test       eax, eax
004b64ee 7563                     jne        0x4b6553
004b64f0 b968a55b00               mov        ecx, 0x5ba568
004b64f5 e8761e0000               call       0x4b8370
004b64fa 85c0                     test       eax, eax
004b64fc 7555                     jne        0x4b6553
004b64fe 6a06                     push       6
004b6500 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004b6506 e8255ff9ff               call       0x44c430
004b650b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b650e c7817001000000000000     mov        dword ptr [ecx + 0x170], 0
004b6518 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b651b 83bac001000000           cmp        dword ptr [edx + 0x1c0], 0
004b6522 7422                     je         0x4b6546
004b6524 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6527 8b88c0010000             mov        ecx, dword ptr [eax + 0x1c0]
004b652d 51                       push       ecx
004b652e 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b6534 e83791f6ff               call       0x41f670
004b6539 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b653c c782c001000000000000     mov        dword ptr [edx + 0x1c0], 0
004b6546 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6549 c780c001000000000000     mov        dword ptr [eax + 0x1c0], 0
004b6553 8be5                     mov        esp, ebp
004b6555 5d                       pop        ebp
004b6556 c3                       ret        
004b6557 cc                       int3       
004b6558 cc                       int3       
004b6559 cc                       int3       
004b655a cc                       int3       
004b655b cc                       int3       
004b655c cc                       int3       
004b655d cc                       int3       
004b655e cc                       int3       
004b655f cc                       int3       
