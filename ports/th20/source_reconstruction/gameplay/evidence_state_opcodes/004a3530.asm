004a3530 55                       push       ebp
004a3531 8bec                     mov        ebp, esp
004a3533 51                       push       ecx
004a3534 894dfc                   mov        dword ptr [ebp - 4], ecx
004a3537 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a353a c700ffffffff             mov        dword ptr [eax], 0xffffffff
004a3540 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a3543 c74104ffffffff           mov        dword ptr [ecx + 4], 0xffffffff
004a354a 6a40                     push       0x40
004a354c 6a00                     push       0
004a354e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a3551 83c208                   add        edx, 8
004a3554 52                       push       edx
004a3555 e806120a00               call       0x544760
004a355a 83c40c                   add        esp, 0xc
004a355d 6a40                     push       0x40
004a355f 6a00                     push       0
004a3561 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a3564 83c048                   add        eax, 0x48
004a3567 50                       push       eax
004a3568 e8f3110a00               call       0x544760
004a356d 83c40c                   add        esp, 0xc
004a3570 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a3573 8be5                     mov        esp, ebp
004a3575 5d                       pop        ebp
004a3576 c3                       ret        
004a3577 cc                       int3       
004a3578 cc                       int3       
004a3579 cc                       int3       
004a357a cc                       int3       
004a357b cc                       int3       
004a357c cc                       int3       
004a357d cc                       int3       
004a357e cc                       int3       
004a357f cc                       int3       
