004c87c0 55                       push       ebp
004c87c1 8bec                     mov        ebp, esp
004c87c3 6aff                     push       -1
004c87c5 6800765600               push       0x567600
004c87ca 64a100000000             mov        eax, dword ptr fs:[0]
004c87d0 50                       push       eax
004c87d1 51                       push       ecx
004c87d2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c87d7 33c5                     xor        eax, ebp
004c87d9 50                       push       eax
004c87da 8d45f4                   lea        eax, [ebp - 0xc]
004c87dd 64a300000000             mov        dword ptr fs:[0], eax
004c87e3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c87e6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c87e9 e83275f5ff               call       0x41fd20
004c87ee 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c87f1 c70000145700             mov        dword ptr [eax], 0x571400
004c87f7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c87fa 83c110                   add        ecx, 0x10
004c87fd e84e02f5ff               call       0x418a50
004c8802 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8805 c7412800000000           mov        dword ptr [ecx + 0x28], 0
004c880c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c880f c7422c00000100           mov        dword ptr [edx + 0x2c], 0x10000
004c8816 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8819 83c130                   add        ecx, 0x30
004c881c e8efa5f5ff               call       0x422e10
004c8821 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8824 83c13c                   add        ecx, 0x3c
004c8827 e8e4a5f5ff               call       0x422e10
004c882c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c882f c7404800000000           mov        dword ptr [eax + 0x48], 0
004c8836 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8839 c7414c00000000           mov        dword ptr [ecx + 0x4c], 0
004c8840 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8843 c7425000000000           mov        dword ptr [edx + 0x50], 0
004c884a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c884d c7405400000000           mov        dword ptr [eax + 0x54], 0
004c8854 680c145700               push       0x57140c
004c8859 e8523ef4ff               call       0x40c6b0
004c885e 83c404                   add        esp, 4
004c8861 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8864 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c8867 64890d00000000           mov        dword ptr fs:[0], ecx
004c886e 59                       pop        ecx
004c886f 8be5                     mov        esp, ebp
004c8871 5d                       pop        ebp
004c8872 c3                       ret        
004c8873 cc                       int3       
004c8874 cc                       int3       
004c8875 cc                       int3       
004c8876 cc                       int3       
004c8877 cc                       int3       
004c8878 cc                       int3       
004c8879 cc                       int3       
004c887a cc                       int3       
004c887b cc                       int3       
004c887c cc                       int3       
004c887d cc                       int3       
004c887e cc                       int3       
004c887f cc                       int3       
