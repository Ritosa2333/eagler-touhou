004c8410 55                       push       ebp
004c8411 8bec                     mov        ebp, esp
004c8413 51                       push       ecx
004c8414 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8417 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c841a e891010000               call       0x4c85b0
004c841f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8422 c70014125700             mov        dword ptr [eax], 0x571214
004c8428 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c842b 81c1f8060000             add        ecx, 0x6f8
004c8431 e81a32fcff               call       0x48b650
004c8436 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8439 81c154070000             add        ecx, 0x754
004c843f e8fc06f8ff               call       0x448b40
004c8444 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8447 81c1380d0000             add        ecx, 0xd38
004c844d e8ee06f8ff               call       0x448b40
004c8452 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8455 f30f100598f75600         movss      xmm0, dword ptr [0x56f798]
004c845d f30f11811c130000         movss      dword ptr [ecx + 0x131c], xmm0
004c8465 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c8468 c7822013000000000000     mov        dword ptr [edx + 0x1320], 0
004c8472 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8475 c7802413000000000000     mov        dword ptr [eax + 0x1324], 0
004c847f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8482 c7812813000000000000     mov        dword ptr [ecx + 0x1328], 0
004c848c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c848f c7822c13000000000000     mov        dword ptr [edx + 0x132c], 0
004c8499 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c849c 81c130130000             add        ecx, 0x1330
004c84a2 e819feffff               call       0x4c82c0
004c84a7 90                       nop        
004c84a8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c84ab 8be5                     mov        esp, ebp
004c84ad 5d                       pop        ebp
004c84ae c3                       ret        
004c84af cc                       int3       
