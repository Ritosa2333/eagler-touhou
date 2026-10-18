005345d0 55                       push       ebp
005345d1 8bec                     mov        ebp, esp
005345d3 51                       push       ecx
005345d4 894dfc                   mov        dword ptr [ebp - 4], ecx
005345d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
005345da 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005345dd 89487c                   mov        dword ptr [eax + 0x7c], ecx
005345e0 8b55fc                   mov        edx, dword ptr [ebp - 4]
005345e3 8b427c                   mov        eax, dword ptr [edx + 0x7c]
005345e6 50                       push       eax
005345e7 b968a55b00               mov        ecx, 0x5ba568
005345ec e8cf75edff               call       0x40bbc0
005345f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005345f4 898180000000             mov        dword ptr [ecx + 0x80], eax
005345fa 8be5                     mov        esp, ebp
005345fc 5d                       pop        ebp
005345fd c20400                   ret        4
