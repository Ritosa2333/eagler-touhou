004c5530 55                       push       ebp
004c5531 8bec                     mov        ebp, esp
004c5533 6aff                     push       -1
004c5535 6800765600               push       0x567600
004c553a 64a100000000             mov        eax, dword ptr fs:[0]
004c5540 50                       push       eax
004c5541 51                       push       ecx
004c5542 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c5547 33c5                     xor        eax, ebp
004c5549 50                       push       eax
004c554a 8d45f4                   lea        eax, [ebp - 0xc]
004c554d 64a300000000             mov        dword ptr fs:[0], eax
004c5553 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c5556 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5559 e8c2a7f5ff               call       0x41fd20
004c555e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5561 c700d00d5700             mov        dword ptr [eax], 0x570dd0
004c5567 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c556a c7411000000000           mov        dword ptr [ecx + 0x10], 0
004c5571 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5574 83c114                   add        ecx, 0x14
004c5577 e88499feff               call       0x4aef00
004c557c 33d2                     xor        edx, edx
004c557e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5581 83c060                   add        eax, 0x60
004c5584 8910                     mov        dword ptr [eax], edx
004c5586 895004                   mov        dword ptr [eax + 4], edx
004c5589 895008                   mov        dword ptr [eax + 8], edx
004c558c 89500c                   mov        dword ptr [eax + 0xc], edx
004c558f 895010                   mov        dword ptr [eax + 0x10], edx
004c5592 895014                   mov        dword ptr [eax + 0x14], edx
004c5595 895018                   mov        dword ptr [eax + 0x18], edx
004c5598 89501c                   mov        dword ptr [eax + 0x1c], edx
004c559b 33c9                     xor        ecx, ecx
004c559d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c55a0 81c280000000             add        edx, 0x80
004c55a6 890a                     mov        dword ptr [edx], ecx
004c55a8 894a04                   mov        dword ptr [edx + 4], ecx
004c55ab 894a08                   mov        dword ptr [edx + 8], ecx
004c55ae 894a0c                   mov        dword ptr [edx + 0xc], ecx
004c55b1 894a10                   mov        dword ptr [edx + 0x10], ecx
004c55b4 894a14                   mov        dword ptr [edx + 0x14], ecx
004c55b7 894a18                   mov        dword ptr [edx + 0x18], ecx
004c55ba 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004c55bd 33c0                     xor        eax, eax
004c55bf 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c55c2 81c1a0000000             add        ecx, 0xa0
004c55c8 8901                     mov        dword ptr [ecx], eax
004c55ca 894104                   mov        dword ptr [ecx + 4], eax
004c55cd 894108                   mov        dword ptr [ecx + 8], eax
004c55d0 89410c                   mov        dword ptr [ecx + 0xc], eax
004c55d3 894110                   mov        dword ptr [ecx + 0x10], eax
004c55d6 894114                   mov        dword ptr [ecx + 0x14], eax
004c55d9 894118                   mov        dword ptr [ecx + 0x18], eax
004c55dc 89411c                   mov        dword ptr [ecx + 0x1c], eax
004c55df 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c55e2 81c1c0000000             add        ecx, 0xc0
004c55e8 e8a3d7f5ff               call       0x422d90
004c55ed 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c55f0 81c1d0000000             add        ecx, 0xd0
004c55f6 e895d7f5ff               call       0x422d90
004c55fb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c55fe 81c1e0000000             add        ecx, 0xe0
004c5604 e807d8f5ff               call       0x422e10
004c5609 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c560c 81c1ec000000             add        ecx, 0xec
004c5612 e879d7f5ff               call       0x422d90
004c5617 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c561a c782fc00000000000000     mov        dword ptr [edx + 0xfc], 0
004c5624 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5627 c7800001000000000000     mov        dword ptr [eax + 0x100], 0
004c5631 33c9                     xor        ecx, ecx
004c5633 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c5636 81c204010000             add        edx, 0x104
004c563c 890a                     mov        dword ptr [edx], ecx
004c563e 894a04                   mov        dword ptr [edx + 4], ecx
004c5641 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5644 c7800c01000000000000     mov        dword ptr [eax + 0x10c], 0
004c564e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5651 c7811001000000000000     mov        dword ptr [ecx + 0x110], 0
004c565b 68dc0d5700               push       0x570ddc
004c5660 e84b70f4ff               call       0x40c6b0
004c5665 83c404                   add        esp, 4
004c5668 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c566b 8915284d5c00             mov        dword ptr [0x5c4d28], edx
004c5671 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5674 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c5677 64890d00000000           mov        dword ptr fs:[0], ecx
004c567e 59                       pop        ecx
004c567f 8be5                     mov        esp, ebp
004c5681 5d                       pop        ebp
004c5682 c3                       ret        
004c5683 cc                       int3       
004c5684 cc                       int3       
004c5685 cc                       int3       
004c5686 cc                       int3       
004c5687 cc                       int3       
004c5688 cc                       int3       
004c5689 cc                       int3       
004c568a cc                       int3       
004c568b cc                       int3       
004c568c cc                       int3       
004c568d cc                       int3       
004c568e cc                       int3       
004c568f cc                       int3       
