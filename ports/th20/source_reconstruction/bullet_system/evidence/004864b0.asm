004864b0 55                       push       ebp
004864b1 8bec                     mov        ebp, esp
004864b3 51                       push       ecx
004864b4 894dfc                   mov        dword ptr [ebp - 4], ecx
004864b7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004864ba 8a4d08                   mov        cl, byte ptr [ebp + 8]
004864bd 888896040000             mov        byte ptr [eax + 0x496], cl
004864c3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004864c6 0fb6450c                 movzx      eax, byte ptr [ebp + 0xc]
004864ca 888295040000             mov        byte ptr [edx + 0x495], al
004864d0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004864d3 8a5510                   mov        dl, byte ptr [ebp + 0x10]
004864d6 889194040000             mov        byte ptr [ecx + 0x494], dl
004864dc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004864df 8a4d14                   mov        cl, byte ptr [ebp + 0x14]
004864e2 888897040000             mov        byte ptr [eax + 0x497], cl
004864e8 8be5                     mov        esp, ebp
004864ea 5d                       pop        ebp
004864eb c21000                   ret        0x10
004864ee cc                       int3       
004864ef cc                       int3       
