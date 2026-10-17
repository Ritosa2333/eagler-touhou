004bc480 55                       push       ebp
004bc481 8bec                     mov        ebp, esp
004bc483 6aff                     push       -1
004bc485 688d835600               push       0x56838d
004bc48a 64a100000000             mov        eax, dword ptr fs:[0]
004bc490 50                       push       eax
004bc491 83ec20                   sub        esp, 0x20
004bc494 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bc499 33c5                     xor        eax, ebp
004bc49b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bc49e 50                       push       eax
004bc49f 8d45f4                   lea        eax, [ebp - 0xc]
004bc4a2 64a300000000             mov        dword ptr fs:[0], eax
004bc4a8 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004bc4ab 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004bc4ae 83c010                   add        eax, 0x10
004bc4b1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bc4b4 6a08                     push       8
004bc4b6 8d4de8                   lea        ecx, [ebp - 0x18]
004bc4b9 e8c2fbf4ff               call       0x40c080
004bc4be 8d4de8                   lea        ecx, [ebp - 0x18]
004bc4c1 51                       push       ecx
004bc4c2 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bc4c5 e8b65df5ff               call       0x412280
004bc4ca c745fc00000000           mov        dword ptr [ebp - 4], 0
004bc4d1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bc4d4 e86760f5ff               call       0x412540
004bc4d9 8945d8                   mov        dword ptr [ebp - 0x28], eax
004bc4dc eb09                     jmp        0x4bc4e7
004bc4de 8d4de8                   lea        ecx, [ebp - 0x18]
004bc4e1 e84a57f5ff               call       0x411c30
004bc4e6 90                       nop        
004bc4e7 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004bc4ea 52                       push       edx
004bc4eb 8d4de8                   lea        ecx, [ebp - 0x18]
004bc4ee e8bd56f5ff               call       0x411bb0
004bc4f3 0fb6c0                   movzx      eax, al
004bc4f6 85c0                     test       eax, eax
004bc4f8 743a                     je         0x4bc534
004bc4fa 8d4de8                   lea        ecx, [ebp - 0x18]
004bc4fd e8fefdf4ff               call       0x40c300
004bc502 8945e0                   mov        dword ptr [ebp - 0x20], eax
004bc505 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bc508 e8f3fdf4ff               call       0x40c300
004bc50d 83c008                   add        eax, 8
004bc510 8bc8                     mov        ecx, eax
004bc512 e8c957f5ff               call       0x411ce0
004bc517 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bc51a e8e1fdf4ff               call       0x40c300
004bc51f 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004bc522 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004bc525 51                       push       ecx
004bc526 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004bc52c e86fd5ffff               call       0x4b9aa0
004bc531 90                       nop        
004bc532 ebaa                     jmp        0x4bc4de
004bc534 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004bc53b 8d4de8                   lea        ecx, [ebp - 0x18]
004bc53e e8bd55f5ff               call       0x411b00
004bc543 90                       nop        
004bc544 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004bc547 c7422800000000           mov        dword ptr [edx + 0x28], 0
004bc54e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bc551 64890d00000000           mov        dword ptr fs:[0], ecx
004bc558 59                       pop        ecx
004bc559 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bc55c 33cd                     xor        ecx, ebp
004bc55e e88b640800               call       0x5429ee
004bc563 8be5                     mov        esp, ebp
004bc565 5d                       pop        ebp
004bc566 c3                       ret        
004bc567 cc                       int3       
004bc568 cc                       int3       
004bc569 cc                       int3       
004bc56a cc                       int3       
004bc56b cc                       int3       
004bc56c cc                       int3       
004bc56d cc                       int3       
004bc56e cc                       int3       
004bc56f cc                       int3       
