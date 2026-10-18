0047c620 55                       push       ebp
0047c621 8bec                     mov        ebp, esp
0047c623 83ec08                   sub        esp, 8
0047c626 894df8                   mov        dword ptr [ebp - 8], ecx
0047c629 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047c62c 8b4804                   mov        ecx, dword ptr [eax + 4]
0047c62f 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
0047c632 7409                     je         0x47c63d
0047c634 c745fc01000000           mov        dword ptr [ebp - 4], 1
0047c63b eb07                     jmp        0x47c644
0047c63d c745fc00000000           mov        dword ptr [ebp - 4], 0
0047c644 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0047c648 8be5                     mov        esp, ebp
0047c64a 5d                       pop        ebp
0047c64b c20400                   ret        4
0047c64e cc                       int3       
0047c64f cc                       int3       
