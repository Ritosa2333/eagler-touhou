004de670 55                       push       ebp
004de671 8bec                     mov        ebp, esp
004de673 6aff                     push       -1
004de675 688d7c5600               push       0x567c8d
004de67a 64a100000000             mov        eax, dword ptr fs:[0]
004de680 50                       push       eax
004de681 51                       push       ecx
004de682 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004de687 33c5                     xor        eax, ebp
004de689 50                       push       eax
004de68a 8d45f4                   lea        eax, [ebp - 0xc]
004de68d 64a300000000             mov        dword ptr fs:[0], eax
004de693 894df0                   mov        dword ptr [ebp - 0x10], ecx
004de696 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de699 e8e258f3ff               call       0x413f80
004de69e c745fc00000000           mov        dword ptr [ebp - 4], 0
004de6a5 8b4508                   mov        eax, dword ptr [ebp + 8]
004de6a8 50                       push       eax
004de6a9 e8d2cbf2ff               call       0x40b280
004de6ae 83c404                   add        esp, 4
004de6b1 50                       push       eax
004de6b2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004de6b5 e8d6010000               call       0x4de890
004de6ba 90                       nop        
004de6bb c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004de6c2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004de6c5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de6c8 64890d00000000           mov        dword ptr fs:[0], ecx
004de6cf 59                       pop        ecx
004de6d0 8be5                     mov        esp, ebp
004de6d2 5d                       pop        ebp
004de6d3 c20400                   ret        4
004de6d6 cc                       int3       
004de6d7 cc                       int3       
004de6d8 cc                       int3       
004de6d9 cc                       int3       
004de6da cc                       int3       
004de6db cc                       int3       
004de6dc cc                       int3       
004de6dd cc                       int3       
004de6de cc                       int3       
004de6df cc                       int3       
