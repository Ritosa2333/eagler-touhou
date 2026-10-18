0052ca30 55                       push       ebp
0052ca31 8bec                     mov        ebp, esp
0052ca33 83ec08                   sub        esp, 8
0052ca36 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ca39 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ca3c 0fbe4809                 movsx      ecx, byte ptr [eax + 9]
0052ca40 85c9                     test       ecx, ecx
0052ca42 7514                     jne        0x52ca58
0052ca44 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052ca47 0fbe4208                 movsx      eax, byte ptr [edx + 8]
0052ca4b 85c0                     test       eax, eax
0052ca4d 7509                     jne        0x52ca58
0052ca4f c745f800000000           mov        dword ptr [ebp - 8], 0
0052ca56 eb07                     jmp        0x52ca5f
0052ca58 c745f801000000           mov        dword ptr [ebp - 8], 1
0052ca5f 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
0052ca63 8be5                     mov        esp, ebp
0052ca65 5d                       pop        ebp
0052ca66 c3                       ret        
0052ca67 cc                       int3       
0052ca68 cc                       int3       
0052ca69 cc                       int3       
0052ca6a cc                       int3       
0052ca6b cc                       int3       
0052ca6c cc                       int3       
0052ca6d cc                       int3       
0052ca6e cc                       int3       
0052ca6f cc                       int3       
