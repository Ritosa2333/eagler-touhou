004de7c0 55                       push       ebp
004de7c1 8bec                     mov        ebp, esp
004de7c3 6aff                     push       -1
004de7c5 688d7c5600               push       0x567c8d
004de7ca 64a100000000             mov        eax, dword ptr fs:[0]
004de7d0 50                       push       eax
004de7d1 51                       push       ecx
004de7d2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004de7d7 33c5                     xor        eax, ebp
004de7d9 50                       push       eax
004de7da 8d45f4                   lea        eax, [ebp - 0xc]
004de7dd 64a300000000             mov        dword ptr fs:[0], eax
004de7e3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004de7e6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de7e9 e89257f3ff               call       0x413f80
004de7ee c745fc00000000           mov        dword ptr [ebp - 4], 0
004de7f5 8b4508                   mov        eax, dword ptr [ebp + 8]
004de7f8 50                       push       eax
004de7f9 e882caf2ff               call       0x40b280
004de7fe 83c404                   add        esp, 4
004de801 50                       push       eax
004de802 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de805 e8a6010000               call       0x4de9b0
004de80a 90                       nop        
004de80b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004de812 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004de815 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de818 64890d00000000           mov        dword ptr fs:[0], ecx
004de81f 59                       pop        ecx
004de820 8be5                     mov        esp, ebp
004de822 5d                       pop        ebp
004de823 c20400                   ret        4
004de826 cc                       int3       
004de827 cc                       int3       
004de828 cc                       int3       
004de829 cc                       int3       
004de82a cc                       int3       
004de82b cc                       int3       
004de82c cc                       int3       
004de82d cc                       int3       
004de82e cc                       int3       
004de82f cc                       int3       
