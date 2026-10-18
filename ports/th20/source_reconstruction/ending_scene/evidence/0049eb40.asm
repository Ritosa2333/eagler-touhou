0049eb40 55                       push       ebp
0049eb41 8bec                     mov        ebp, esp
0049eb43 83ec08                   sub        esp, 8
0049eb46 894dfc                   mov        dword ptr [ebp - 4], ecx
0049eb49 8b45fc                   mov        eax, dword ptr [ebp - 4]
0049eb4c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0049eb4f 8908                     mov        dword ptr [eax], ecx
0049eb51 8b55fc                   mov        edx, dword ptr [ebp - 4]
0049eb54 83c204                   add        edx, 4
0049eb57 8955f8                   mov        dword ptr [ebp - 8], edx
0049eb5a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0049eb5d 50                       push       eax
0049eb5e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0049eb61 e81abcfcff               call       0x46a780
0049eb66 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0049eb69 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0049eb6c 8b02                     mov        eax, dword ptr [edx]
0049eb6e 894120                   mov        dword ptr [ecx + 0x20], eax
0049eb71 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0049eb74 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0049eb77 8b02                     mov        eax, dword ptr [edx]
0049eb79 894124                   mov        dword ptr [ecx + 0x24], eax
0049eb7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0049eb7f 8b5518                   mov        edx, dword ptr [ebp + 0x18]
0049eb82 895128                   mov        dword ptr [ecx + 0x28], edx
0049eb85 8b45fc                   mov        eax, dword ptr [ebp - 4]
0049eb88 8be5                     mov        esp, ebp
0049eb8a 5d                       pop        ebp
0049eb8b c21400                   ret        0x14
0049eb8e cc                       int3       
0049eb8f cc                       int3       
