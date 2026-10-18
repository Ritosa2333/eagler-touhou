004bfa00 55                       push       ebp
004bfa01 8bec                     mov        ebp, esp
004bfa03 51                       push       ecx
004bfa04 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfa07 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa0a 83780800                 cmp        dword ptr [eax + 8], 0
004bfa0e 7f0a                     jg         0x4bfa1a
004bfa10 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa13 8b01                     mov        eax, dword ptr [ecx]
004bfa15 e9a6000000               jmp        0x4bfac0
004bfa1a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa1d 8b02                     mov        eax, dword ptr [edx]
004bfa1f 034508                   add        eax, dword ptr [ebp + 8]
004bfa22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa25 8901                     mov        dword ptr [ecx], eax
004bfa27 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa2a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa2d 8b0a                     mov        ecx, dword ptr [edx]
004bfa2f 3b4808                   cmp        ecx, dword ptr [eax + 8]
004bfa32 7c33                     jl         0x4bfa67
004bfa34 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa37 837a4800                 cmp        dword ptr [edx + 0x48], 0
004bfa3b 741a                     je         0x4bfa57
004bfa3d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa43 8b5008                   mov        edx, dword ptr [eax + 8]
004bfa46 2b510c                   sub        edx, dword ptr [ecx + 0xc]
004bfa49 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa4c 8b08                     mov        ecx, dword ptr [eax]
004bfa4e 2bca                     sub        ecx, edx
004bfa50 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa53 890a                     mov        dword ptr [edx], ecx
004bfa55 eb0e                     jmp        0x4bfa65
004bfa57 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa5a 8b4808                   mov        ecx, dword ptr [eax + 8]
004bfa5d 83e901                   sub        ecx, 1
004bfa60 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa63 890a                     mov        dword ptr [edx], ecx
004bfa65 ebc0                     jmp        0x4bfa27
004bfa67 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa6a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa6d 8b10                     mov        edx, dword ptr [eax]
004bfa6f 3b510c                   cmp        edx, dword ptr [ecx + 0xc]
004bfa72 7d2e                     jge        0x4bfaa2
004bfa74 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa77 83784800                 cmp        dword ptr [eax + 0x48], 0
004bfa7b 7418                     je         0x4bfa95
004bfa7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa80 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa83 8b4108                   mov        eax, dword ptr [ecx + 8]
004bfa86 2b420c                   sub        eax, dword ptr [edx + 0xc]
004bfa89 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa8c 0301                     add        eax, dword ptr [ecx]
004bfa8e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bfa91 8902                     mov        dword ptr [edx], eax
004bfa93 eb0b                     jmp        0x4bfaa0
004bfa95 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfa98 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfa9b 8b510c                   mov        edx, dword ptr [ecx + 0xc]
004bfa9e 8910                     mov        dword ptr [eax], edx
004bfaa0 ebc5                     jmp        0x4bfa67
004bfaa2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfaa5 8b08                     mov        ecx, dword ptr [eax]
004bfaa7 51                       push       ecx
004bfaa8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfaab e820aafaff               call       0x46a4d0
004bfab0 0fb6d0                   movzx      edx, al
004bfab3 85d2                     test       edx, edx
004bfab5 0f855fffffff             jne        0x4bfa1a
004bfabb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfabe 8b00                     mov        eax, dword ptr [eax]
004bfac0 8be5                     mov        esp, ebp
004bfac2 5d                       pop        ebp
004bfac3 c20400                   ret        4
004bfac6 cc                       int3       
004bfac7 cc                       int3       
004bfac8 cc                       int3       
004bfac9 cc                       int3       
004bfaca cc                       int3       
004bfacb cc                       int3       
004bfacc cc                       int3       
004bfacd cc                       int3       
004bface cc                       int3       
004bfacf cc                       int3       
