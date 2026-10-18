004bcf60 55                       push       ebp
004bcf61 8bec                     mov        ebp, esp
004bcf63 6aff                     push       -1
004bcf65 68cd795600               push       0x5679cd
004bcf6a 64a100000000             mov        eax, dword ptr fs:[0]
004bcf70 50                       push       eax
004bcf71 83ec10                   sub        esp, 0x10
004bcf74 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bcf79 33c5                     xor        eax, ebp
004bcf7b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bcf7e 50                       push       eax
004bcf7f 8d45f4                   lea        eax, [ebp - 0xc]
004bcf82 64a300000000             mov        dword ptr fs:[0], eax
004bcf88 894de8                   mov        dword ptr [ebp - 0x18], ecx
004bcf8b 6a14                     push       0x14
004bcf8d b940025c00               mov        ecx, 0x5c0240
004bcf92 e879f3f4ff               call       0x40c310
004bcf97 50                       push       eax
004bcf98 8d4dec                   lea        ecx, [ebp - 0x14]
004bcf9b e890e7f4ff               call       0x40b730
004bcfa0 c745fc00000000           mov        dword ptr [ebp - 4], 0
004bcfa7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bcfaa e83171faff               call       0x4640e0
004bcfaf 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bcfb2 8b4508                   mov        eax, dword ptr [ebp + 8]
004bcfb5 50                       push       eax
004bcfb6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bcfb9 e842000000               call       0x4bd000
004bcfbe 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bcfc1 e87a1b0000               call       0x4beb40
004bcfc6 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004bcfcd 8d4dec                   lea        ecx, [ebp - 0x14]
004bcfd0 e82be9f4ff               call       0x40b900
004bcfd5 90                       nop        
004bcfd6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bcfd9 64890d00000000           mov        dword ptr fs:[0], ecx
004bcfe0 59                       pop        ecx
004bcfe1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bcfe4 33cd                     xor        ecx, ebp
004bcfe6 e8035a0800               call       0x5429ee
004bcfeb 8be5                     mov        esp, ebp
004bcfed 5d                       pop        ebp
004bcfee c20400                   ret        4
004bcff1 cc                       int3       
004bcff2 cc                       int3       
004bcff3 cc                       int3       
004bcff4 cc                       int3       
004bcff5 cc                       int3       
004bcff6 cc                       int3       
004bcff7 cc                       int3       
004bcff8 cc                       int3       
004bcff9 cc                       int3       
004bcffa cc                       int3       
004bcffb cc                       int3       
004bcffc cc                       int3       
004bcffd cc                       int3       
004bcffe cc                       int3       
004bcfff cc                       int3       
