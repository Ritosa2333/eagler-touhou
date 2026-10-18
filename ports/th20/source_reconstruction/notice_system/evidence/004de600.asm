004de600 55                       push       ebp
004de601 8bec                     mov        ebp, esp
004de603 6aff                     push       -1
004de605 688d7c5600               push       0x567c8d
004de60a 64a100000000             mov        eax, dword ptr fs:[0]
004de610 50                       push       eax
004de611 51                       push       ecx
004de612 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004de617 33c5                     xor        eax, ebp
004de619 50                       push       eax
004de61a 8d45f4                   lea        eax, [ebp - 0xc]
004de61d 64a300000000             mov        dword ptr fs:[0], eax
004de623 894df0                   mov        dword ptr [ebp - 0x10], ecx
004de626 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de629 e85259f3ff               call       0x413f80
004de62e c745fc00000000           mov        dword ptr [ebp - 4], 0
004de635 8b4508                   mov        eax, dword ptr [ebp + 8]
004de638 50                       push       eax
004de639 e842ccf2ff               call       0x40b280
004de63e 83c404                   add        esp, 4
004de641 50                       push       eax
004de642 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de645 e8e6010000               call       0x4de830
004de64a 90                       nop        
004de64b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004de652 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004de655 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de658 64890d00000000           mov        dword ptr fs:[0], ecx
004de65f 59                       pop        ecx
004de660 8be5                     mov        esp, ebp
004de662 5d                       pop        ebp
004de663 c20400                   ret        4
004de666 cc                       int3       
004de667 cc                       int3       
004de668 cc                       int3       
004de669 cc                       int3       
004de66a cc                       int3       
004de66b cc                       int3       
004de66c cc                       int3       
004de66d cc                       int3       
004de66e cc                       int3       
004de66f cc                       int3       
