004e6600 55                       push       ebp
004e6601 8bec                     mov        ebp, esp
004e6603 83ec24                   sub        esp, 0x24
004e6606 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004e6609 8d4d08                   lea        ecx, [ebp + 8]
004e660c e8bf68f6ff               call       0x44ced0
004e6611 8945f8                   mov        dword ptr [ebp - 8], eax
004e6614 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e6617 e8b416f5ff               call       0x437cd0
004e661c 8945fc                   mov        dword ptr [ebp - 4], eax
004e661f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6622 f30f2c4848               cvttss2si  ecx, dword ptr [eax + 0x48]
004e6627 894df4                   mov        dword ptr [ebp - 0xc], ecx
004e662a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e662d f30f2c424c               cvttss2si  eax, dword ptr [edx + 0x4c]
004e6632 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e6635 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6638 f30f2c5110               cvttss2si  edx, dword ptr [ecx + 0x10]
004e663d 8955ec                   mov        dword ptr [ebp - 0x14], edx
004e6640 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6643 f30f2c480c               cvttss2si  ecx, dword ptr [eax + 0xc]
004e6648 894de8                   mov        dword ptr [ebp - 0x18], ecx
004e664b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e664e e8ed000000               call       0x4e6740
004e6653 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004e6656 8b55f8                   mov        edx, dword ptr [ebp - 8]
004e6659 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004e665c 8945e0                   mov        dword ptr [ebp - 0x20], eax
004e665f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e6662 51                       push       ecx
004e6663 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e6666 52                       push       edx
004e6667 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e666a 50                       push       eax
004e666b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004e666e 51                       push       ecx
004e666f 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004e6672 52                       push       edx
004e6673 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004e6676 50                       push       eax
004e6677 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004e667a 51                       push       ecx
004e667b 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004e667e 52                       push       edx
004e667f 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004e6682 50                       push       eax
004e6683 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e6686 51                       push       ecx
004e6687 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004e668a e8a1feffff               call       0x4e6530
004e668f 8be5                     mov        esp, ebp
004e6691 5d                       pop        ebp
004e6692 c21400                   ret        0x14
004e6695 cc                       int3       
004e6696 cc                       int3       
004e6697 cc                       int3       
004e6698 cc                       int3       
004e6699 cc                       int3       
004e669a cc                       int3       
004e669b cc                       int3       
004e669c cc                       int3       
004e669d cc                       int3       
004e669e cc                       int3       
004e669f cc                       int3       
