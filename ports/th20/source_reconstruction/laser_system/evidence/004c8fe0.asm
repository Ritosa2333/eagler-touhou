004c8fe0 55                       push       ebp
004c8fe1 8bec                     mov        ebp, esp
004c8fe3 51                       push       ecx
004c8fe4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8fe7 8b4508                   mov        eax, dword ptr [ebp + 8]
004c8fea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8fed 8b10                     mov        edx, dword ptr [eax]
004c8fef 8911                     mov        dword ptr [ecx], edx
004c8ff1 8b5004                   mov        edx, dword ptr [eax + 4]
004c8ff4 895104                   mov        dword ptr [ecx + 4], edx
004c8ff7 8b4008                   mov        eax, dword ptr [eax + 8]
004c8ffa 894108                   mov        dword ptr [ecx + 8], eax
004c8ffd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9000 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9003 8b420c                   mov        eax, dword ptr [edx + 0xc]
004c9006 89410c                   mov        dword ptr [ecx + 0xc], eax
004c9009 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c900c 8b5508                   mov        edx, dword ptr [ebp + 8]
004c900f 8b4210                   mov        eax, dword ptr [edx + 0x10]
004c9012 894110                   mov        dword ptr [ecx + 0x10], eax
004c9015 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9018 8b5508                   mov        edx, dword ptr [ebp + 8]
004c901b 8b4214                   mov        eax, dword ptr [edx + 0x14]
004c901e 894114                   mov        dword ptr [ecx + 0x14], eax
004c9021 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9024 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9027 8b4218                   mov        eax, dword ptr [edx + 0x18]
004c902a 894118                   mov        dword ptr [ecx + 0x18], eax
004c902d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9030 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9033 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004c9036 89411c                   mov        dword ptr [ecx + 0x1c], eax
004c9039 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c903c 8b5508                   mov        edx, dword ptr [ebp + 8]
004c903f 8b4220                   mov        eax, dword ptr [edx + 0x20]
004c9042 894120                   mov        dword ptr [ecx + 0x20], eax
004c9045 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9048 8b5508                   mov        edx, dword ptr [ebp + 8]
004c904b 8b4224                   mov        eax, dword ptr [edx + 0x24]
004c904e 894124                   mov        dword ptr [ecx + 0x24], eax
004c9051 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9054 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9057 8b4228                   mov        eax, dword ptr [edx + 0x28]
004c905a 894128                   mov        dword ptr [ecx + 0x28], eax
004c905d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9060 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9063 8b422c                   mov        eax, dword ptr [edx + 0x2c]
004c9066 89412c                   mov        dword ptr [ecx + 0x2c], eax
004c9069 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c906c 8b5508                   mov        edx, dword ptr [ebp + 8]
004c906f 8b4230                   mov        eax, dword ptr [edx + 0x30]
004c9072 894130                   mov        dword ptr [ecx + 0x30], eax
004c9075 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9078 8b5134                   mov        edx, dword ptr [ecx + 0x34]
004c907b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c907e 895034                   mov        dword ptr [eax + 0x34], edx
004c9081 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9084 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9087 8b4234                   mov        eax, dword ptr [edx + 0x34]
004c908a 894134                   mov        dword ptr [ecx + 0x34], eax
004c908d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9090 83c138                   add        ecx, 0x38
004c9093 51                       push       ecx
004c9094 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9097 83c138                   add        ecx, 0x38
004c909a e8d134fbff               call       0x47c570
004c909f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c90a2 8b4508                   mov        eax, dword ptr [ebp + 8]
004c90a5 8b4848                   mov        ecx, dword ptr [eax + 0x48]
004c90a8 894a48                   mov        dword ptr [edx + 0x48], ecx
004c90ab 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c90ae 8b4508                   mov        eax, dword ptr [ebp + 8]
004c90b1 8b484c                   mov        ecx, dword ptr [eax + 0x4c]
004c90b4 894a4c                   mov        dword ptr [edx + 0x4c], ecx
004c90b7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c90ba 8b4508                   mov        eax, dword ptr [ebp + 8]
004c90bd 8b4850                   mov        ecx, dword ptr [eax + 0x50]
004c90c0 894a50                   mov        dword ptr [edx + 0x50], ecx
004c90c3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c90c6 8be5                     mov        esp, ebp
004c90c8 5d                       pop        ebp
004c90c9 c20400                   ret        4
004c90cc cc                       int3       
004c90cd cc                       int3       
004c90ce cc                       int3       
004c90cf cc                       int3       
