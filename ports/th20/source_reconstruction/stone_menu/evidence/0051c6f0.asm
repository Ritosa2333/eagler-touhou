0051c6f0 55                       push       ebp
0051c6f1 8bec                     mov        ebp, esp
0051c6f3 83ec24                   sub        esp, 0x24
0051c6f6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051c6fb 33c5                     xor        eax, ebp
0051c6fd 8945fc                   mov        dword ptr [ebp - 4], eax
0051c700 894de0                   mov        dword ptr [ebp - 0x20], ecx
0051c703 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0051c706 25ff000000               and        eax, 0xff
0051c70b 0fb6c8                   movzx      ecx, al
0051c70e 51                       push       ecx
0051c70f 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0051c712 c1ea08                   shr        edx, 8
0051c715 81e2ff000000             and        edx, 0xff
0051c71b 0fb6c2                   movzx      eax, dl
0051c71e 50                       push       eax
0051c71f 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0051c722 c1e910                   shr        ecx, 0x10
0051c725 81e1ff000000             and        ecx, 0xff
0051c72b 0fb6d1                   movzx      edx, cl
0051c72e 52                       push       edx
0051c72f 8d4de4                   lea        ecx, [ebp - 0x1c]
0051c732 e8a9caf0ff               call       0x4291e0
0051c737 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0051c73a 25ff000000               and        eax, 0xff
0051c73f 0fb6c8                   movzx      ecx, al
0051c742 51                       push       ecx
0051c743 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0051c746 c1ea08                   shr        edx, 8
0051c749 81e2ff000000             and        edx, 0xff
0051c74f 0fb6c2                   movzx      eax, dl
0051c752 50                       push       eax
0051c753 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0051c756 c1e910                   shr        ecx, 0x10
0051c759 81e1ff000000             and        ecx, 0xff
0051c75f 0fb6d1                   movzx      edx, cl
0051c762 52                       push       edx
0051c763 8d4df0                   lea        ecx, [ebp - 0x10]
0051c766 e875caf0ff               call       0x4291e0
0051c76b 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0051c76e 05e0000000               add        eax, 0xe0
0051c773 8945dc                   mov        dword ptr [ebp - 0x24], eax
0051c776 8d4df0                   lea        ecx, [ebp - 0x10]
0051c779 51                       push       ecx
0051c77a 8d55e4                   lea        edx, [ebp - 0x1c]
0051c77d 52                       push       edx
0051c77e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0051c781 50                       push       eax
0051c782 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051c785 51                       push       ecx
0051c786 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0051c789 e842cef1ff               call       0x4395d0
0051c78e 90                       nop        
0051c78f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c792 33cd                     xor        ecx, ebp
0051c794 e855620200               call       0x5429ee
0051c799 8be5                     mov        esp, ebp
0051c79b 5d                       pop        ebp
0051c79c c21000                   ret        0x10
0051c79f cc                       int3       
