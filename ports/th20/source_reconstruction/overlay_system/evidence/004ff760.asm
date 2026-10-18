004ff760 55                       push       ebp
004ff761 8bec                     mov        ebp, esp
004ff763 83ec0c                   sub        esp, 0xc
004ff766 894dfc                   mov        dword ptr [ebp - 4], ecx
004ff769 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ff76c 8b4830                   mov        ecx, dword ptr [eax + 0x30]
004ff76f 894df8                   mov        dword ptr [ebp - 8], ecx
004ff772 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ff775 8b4230                   mov        eax, dword ptr [edx + 0x30]
004ff778 8b08                     mov        ecx, dword ptr [eax]
004ff77a 8b5118                   mov        edx, dword ptr [ecx + 0x18]
004ff77d 8955f4                   mov        dword ptr [ebp - 0xc], edx
004ff780 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004ff783 50                       push       eax
004ff784 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ff787 51                       push       ecx
004ff788 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004ff78b ff55f4                   call       dword ptr [ebp - 0xc]
004ff78e 8be5                     mov        esp, ebp
004ff790 5d                       pop        ebp
004ff791 c20800                   ret        8
004ff794 cc                       int3       
004ff795 cc                       int3       
004ff796 cc                       int3       
004ff797 cc                       int3       
004ff798 cc                       int3       
004ff799 cc                       int3       
004ff79a cc                       int3       
004ff79b cc                       int3       
004ff79c cc                       int3       
004ff79d cc                       int3       
004ff79e cc                       int3       
004ff79f cc                       int3       
