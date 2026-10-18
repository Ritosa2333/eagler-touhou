004de6e0 55                       push       ebp
004de6e1 8bec                     mov        ebp, esp
004de6e3 6aff                     push       -1
004de6e5 688d7c5600               push       0x567c8d
004de6ea 64a100000000             mov        eax, dword ptr fs:[0]
004de6f0 50                       push       eax
004de6f1 51                       push       ecx
004de6f2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004de6f7 33c5                     xor        eax, ebp
004de6f9 50                       push       eax
004de6fa 8d45f4                   lea        eax, [ebp - 0xc]
004de6fd 64a300000000             mov        dword ptr fs:[0], eax
004de703 894df0                   mov        dword ptr [ebp - 0x10], ecx
004de706 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de709 e87258f3ff               call       0x413f80
004de70e c745fc00000000           mov        dword ptr [ebp - 4], 0
004de715 8b4508                   mov        eax, dword ptr [ebp + 8]
004de718 50                       push       eax
004de719 e862cbf2ff               call       0x40b280
004de71e 83c404                   add        esp, 4
004de721 50                       push       eax
004de722 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de725 e8c6010000               call       0x4de8f0
004de72a 90                       nop        
004de72b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004de732 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004de735 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de738 64890d00000000           mov        dword ptr fs:[0], ecx
004de73f 59                       pop        ecx
004de740 8be5                     mov        esp, ebp
004de742 5d                       pop        ebp
004de743 c20400                   ret        4
004de746 cc                       int3       
004de747 cc                       int3       
004de748 cc                       int3       
004de749 cc                       int3       
004de74a cc                       int3       
004de74b cc                       int3       
004de74c cc                       int3       
004de74d cc                       int3       
004de74e cc                       int3       
004de74f cc                       int3       
