004ff630 55                       push       ebp
004ff631 8bec                     mov        ebp, esp
004ff633 83ec0c                   sub        esp, 0xc
004ff636 894dfc                   mov        dword ptr [ebp - 4], ecx
004ff639 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ff63c 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
004ff63f 894df8                   mov        dword ptr [ebp - 8], ecx
004ff642 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ff645 8b422c                   mov        eax, dword ptr [edx + 0x2c]
004ff648 8b08                     mov        ecx, dword ptr [eax]
004ff64a 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004ff64d 8955f4                   mov        dword ptr [ebp - 0xc], edx
004ff650 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004ff653 50                       push       eax
004ff654 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ff657 51                       push       ecx
004ff658 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004ff65b ff55f4                   call       dword ptr [ebp - 0xc]
004ff65e 8be5                     mov        esp, ebp
004ff660 5d                       pop        ebp
004ff661 c20800                   ret        8
004ff664 cc                       int3       
004ff665 cc                       int3       
004ff666 cc                       int3       
004ff667 cc                       int3       
004ff668 cc                       int3       
004ff669 cc                       int3       
004ff66a cc                       int3       
004ff66b cc                       int3       
004ff66c cc                       int3       
004ff66d cc                       int3       
004ff66e cc                       int3       
004ff66f cc                       int3       
