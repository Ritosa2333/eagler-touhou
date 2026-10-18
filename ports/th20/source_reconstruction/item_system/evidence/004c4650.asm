004c4650 55                       push       ebp
004c4651 8bec                     mov        ebp, esp
004c4653 51                       push       ecx
004c4654 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4657 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c465a 83780800                 cmp        dword ptr [eax + 8], 0
004c465e 740c                     je         0x4c466c
004c4660 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4663 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c4666 e815e7f4ff               call       0x412d80
004c466b 90                       nop        
004c466c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c466f 837a0c00                 cmp        dword ptr [edx + 0xc], 0
004c4673 740c                     je         0x4c4681
004c4675 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4678 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004c467b e800e7f4ff               call       0x412d80
004c4680 90                       nop        
004c4681 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4684 83791400                 cmp        dword ptr [ecx + 0x14], 0
004c4688 740c                     je         0x4c4696
004c468a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c468d 8b4a14                   mov        ecx, dword ptr [edx + 0x14]
004c4690 e8ebe6f4ff               call       0x412d80
004c4695 90                       nop        
004c4696 8be5                     mov        esp, ebp
004c4698 5d                       pop        ebp
004c4699 c3                       ret        
004c469a cc                       int3       
004c469b cc                       int3       
004c469c cc                       int3       
004c469d cc                       int3       
004c469e cc                       int3       
004c469f cc                       int3       
