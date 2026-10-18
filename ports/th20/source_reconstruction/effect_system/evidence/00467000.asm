00467000 55                       push       ebp
00467001 8bec                     mov        ebp, esp
00467003 83ec20                   sub        esp, 0x20
00467006 8d4508                   lea        eax, [ebp + 8]
00467009 50                       push       eax
0046700a e811e6fbff               call       0x425620
0046700f 83c404                   add        esp, 4
00467012 8945f4                   mov        dword ptr [ebp - 0xc], eax
00467015 8d4d0c                   lea        ecx, [ebp + 0xc]
00467018 51                       push       ecx
00467019 e802e6fbff               call       0x425620
0046701e 83c404                   add        esp, 4
00467021 8945f0                   mov        dword ptr [ebp - 0x10], eax
00467024 8d5510                   lea        edx, [ebp + 0x10]
00467027 52                       push       edx
00467028 e8f3e5fbff               call       0x425620
0046702d 83c404                   add        esp, 4
00467030 8945ec                   mov        dword ptr [ebp - 0x14], eax
00467033 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00467036 8945fc                   mov        dword ptr [ebp - 4], eax
00467039 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0046703c 894de8                   mov        dword ptr [ebp - 0x18], ecx
0046703f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00467042 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00467045 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00467048 2b45fc                   sub        eax, dword ptr [ebp - 4]
0046704b 8945f8                   mov        dword ptr [ebp - 8], eax
0046704e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00467051 51                       push       ecx
00467052 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467055 52                       push       edx
00467056 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00467059 2b45f8                   sub        eax, dword ptr [ebp - 8]
0046705c 50                       push       eax
0046705d e8becd0d00               call       0x543e20
00467062 83c40c                   add        esp, 0xc
00467065 8945e0                   mov        dword ptr [ebp - 0x20], eax
00467068 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0046706b 8be5                     mov        esp, ebp
0046706d 5d                       pop        ebp
0046706e c3                       ret        
0046706f cc                       int3       
