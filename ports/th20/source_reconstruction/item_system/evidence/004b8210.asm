004b8210 55                       push       ebp
004b8211 8bec                     mov        ebp, esp
004b8213 83ec0c                   sub        esp, 0xc
004b8216 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8219 c745f890010000           mov        dword ptr [ebp - 8], 0x190
004b8220 c745f490010000           mov        dword ptr [ebp - 0xc], 0x190
004b8227 8d45f8                   lea        eax, [ebp - 8]
004b822a 50                       push       eax
004b822b 8d4df4                   lea        ecx, [ebp - 0xc]
004b822e 51                       push       ecx
004b822f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8232 83c234                   add        edx, 0x34
004b8235 52                       push       edx
004b8236 e8358ffbff               call       0x471170
004b823b 83c40c                   add        esp, 0xc
004b823e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8241 8b10                     mov        edx, dword ptr [eax]
004b8243 895134                   mov        dword ptr [ecx + 0x34], edx
004b8246 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b8249 8b4034                   mov        eax, dword ptr [eax + 0x34]
004b824c 8be5                     mov        esp, ebp
004b824e 5d                       pop        ebp
004b824f c3                       ret        
