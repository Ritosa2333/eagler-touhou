004bd780 55                       push       ebp
004bd781 8bec                     mov        ebp, esp
004bd783 6aff                     push       -1
004bd785 688d805600               push       0x56808d
004bd78a 64a100000000             mov        eax, dword ptr fs:[0]
004bd790 50                       push       eax
004bd791 83ec14                   sub        esp, 0x14
004bd794 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bd799 33c5                     xor        eax, ebp
004bd79b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bd79e 50                       push       eax
004bd79f 8d45f4                   lea        eax, [ebp - 0xc]
004bd7a2 64a300000000             mov        dword ptr fs:[0], eax
004bd7a8 894de8                   mov        dword ptr [ebp - 0x18], ecx
004bd7ab 6a14                     push       0x14
004bd7ad b940025c00               mov        ecx, 0x5c0240
004bd7b2 e859ebf4ff               call       0x40c310
004bd7b7 50                       push       eax
004bd7b8 8d4dec                   lea        ecx, [ebp - 0x14]
004bd7bb e870dff4ff               call       0x40b730
004bd7c0 c745fc00000000           mov        dword ptr [ebp - 4], 0
004bd7c7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bd7ca e8e167faff               call       0x463fb0
004bd7cf 90                       nop        
004bd7d0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bd7d3 e80869faff               call       0x4640e0
004bd7d8 05e8000000               add        eax, 0xe8
004bd7dd 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bd7e0 8b4508                   mov        eax, dword ptr [ebp + 8]
004bd7e3 50                       push       eax
004bd7e4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bd7e7 e874f8ffff               call       0x4bd060
004bd7ec 0fb608                   movzx      ecx, byte ptr [eax]
004bd7ef 894de0                   mov        dword ptr [ebp - 0x20], ecx
004bd7f2 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004bd7f9 8d4dec                   lea        ecx, [ebp - 0x14]
004bd7fc e8ffe0f4ff               call       0x40b900
004bd801 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004bd804 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bd807 64890d00000000           mov        dword ptr fs:[0], ecx
004bd80e 59                       pop        ecx
004bd80f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bd812 33cd                     xor        ecx, ebp
004bd814 e8d5510800               call       0x5429ee
004bd819 8be5                     mov        esp, ebp
004bd81b 5d                       pop        ebp
004bd81c c20400                   ret        4
004bd81f cc                       int3       
