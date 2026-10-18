004de750 55                       push       ebp
004de751 8bec                     mov        ebp, esp
004de753 6aff                     push       -1
004de755 688d7c5600               push       0x567c8d
004de75a 64a100000000             mov        eax, dword ptr fs:[0]
004de760 50                       push       eax
004de761 51                       push       ecx
004de762 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004de767 33c5                     xor        eax, ebp
004de769 50                       push       eax
004de76a 8d45f4                   lea        eax, [ebp - 0xc]
004de76d 64a300000000             mov        dword ptr fs:[0], eax
004de773 894df0                   mov        dword ptr [ebp - 0x10], ecx
004de776 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de779 e80258f3ff               call       0x413f80
004de77e c745fc00000000           mov        dword ptr [ebp - 4], 0
004de785 8b4508                   mov        eax, dword ptr [ebp + 8]
004de788 50                       push       eax
004de789 e8f2caf2ff               call       0x40b280
004de78e 83c404                   add        esp, 4
004de791 50                       push       eax
004de792 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de795 e8b6010000               call       0x4de950
004de79a 90                       nop        
004de79b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004de7a2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004de7a5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de7a8 64890d00000000           mov        dword ptr fs:[0], ecx
004de7af 59                       pop        ecx
004de7b0 8be5                     mov        esp, ebp
004de7b2 5d                       pop        ebp
004de7b3 c20400                   ret        4
004de7b6 cc                       int3       
004de7b7 cc                       int3       
004de7b8 cc                       int3       
004de7b9 cc                       int3       
004de7ba cc                       int3       
004de7bb cc                       int3       
004de7bc cc                       int3       
004de7bd cc                       int3       
004de7be cc                       int3       
004de7bf cc                       int3       
