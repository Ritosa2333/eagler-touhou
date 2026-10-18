004ce510 55                       push       ebp
004ce511 8bec                     mov        ebp, esp
004ce513 6aff                     push       -1
004ce515 688d9b5600               push       0x569b8d
004ce51a 64a100000000             mov        eax, dword ptr fs:[0]
004ce520 50                       push       eax
004ce521 81eca8000000             sub        esp, 0xa8
004ce527 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004ce52c 33c5                     xor        eax, ebp
004ce52e 8945f0                   mov        dword ptr [ebp - 0x10], eax
004ce531 50                       push       eax
004ce532 8d45f4                   lea        eax, [ebp - 0xc]
004ce535 64a300000000             mov        dword ptr fs:[0], eax
004ce53b 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
004ce541 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce547 83c058                   add        eax, 0x58
004ce54a 8b08                     mov        ecx, dword ptr [eax]
004ce54c 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004ce54f 8b5004                   mov        edx, dword ptr [eax + 4]
004ce552 8955dc                   mov        dword ptr [ebp - 0x24], edx
004ce555 8b4008                   mov        eax, dword ptr [eax + 8]
004ce558 8945e0                   mov        dword ptr [ebp - 0x20], eax
004ce55b c7458000000000           mov        dword ptr [ebp - 0x80], 0
004ce562 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce565 e8a648f5ff               call       0x422e10
004ce56a f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ce572 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ce57a 51                       push       ecx
004ce57b f30f110424               movss      dword ptr [esp], xmm0
004ce580 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce586 51                       push       ecx
004ce587 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004ce58c f30f110424               movss      dword ptr [esp], xmm0
004ce591 8d55e4                   lea        edx, [ebp - 0x1c]
004ce594 52                       push       edx
004ce595 e896adf6ff               call       0x439330
004ce59a 83c40c                   add        esp, 0xc
004ce59d 0f57c0                   xorps      xmm0, xmm0
004ce5a0 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004ce5a5 8d45e4                   lea        eax, [ebp - 0x1c]
004ce5a8 50                       push       eax
004ce5a9 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce5ac e82fb1f5ff               call       0x4296e0
004ce5b1 90                       nop        
004ce5b2 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce5b5 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce5b8 7d18                     jge        0x4ce5d2
004ce5ba 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce5bd 035580                   add        edx, dword ptr [ebp - 0x80]
004ce5c0 0fb602                   movzx      eax, byte ptr [edx]
004ce5c3 85c0                     test       eax, eax
004ce5c5 740b                     je         0x4ce5d2
004ce5c7 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce5ca 83c101                   add        ecx, 1
004ce5cd 894d80                   mov        dword ptr [ebp - 0x80], ecx
004ce5d0 ebe0                     jmp        0x4ce5b2
004ce5d2 837d8000                 cmp        dword ptr [ebp - 0x80], 0
004ce5d6 0f84c5000000             je         0x4ce6a1
004ce5dc f30f2a4580               cvtsi2ss   xmm0, dword ptr [ebp - 0x80]
004ce5e1 51                       push       ecx
004ce5e2 f30f110424               movss      dword ptr [esp], xmm0
004ce5e7 8d9564ffffff             lea        edx, [ebp - 0x9c]
004ce5ed 52                       push       edx
004ce5ee 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce5f1 e8faacf5ff               call       0x4292f0
004ce5f6 50                       push       eax
004ce5f7 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce5fd 83c158                   add        ecx, 0x58
004ce600 e8dbb0f5ff               call       0x4296e0
004ce605 90                       nop        
004ce606 f30f2a4580               cvtsi2ss   xmm0, dword ptr [ebp - 0x80]
004ce60b f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce613 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce619 f30f104874               movss      xmm1, dword ptr [eax + 0x74]
004ce61e f30f5cc8                 subss      xmm1, xmm0
004ce622 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce628 f30f114974               movss      dword ptr [ecx + 0x74], xmm1
004ce62d f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ce635 f30f590568cd5600         mulss      xmm0, dword ptr [0x56cd68]
004ce63d 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004ce643 f30f104a74               movss      xmm1, dword ptr [edx + 0x74]
004ce648 0f2fc8                   comiss     xmm1, xmm0
004ce64b 7632                     jbe        0x4ce67f
004ce64d 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce653 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce659 8b5174                   mov        edx, dword ptr [ecx + 0x74]
004ce65c 899008070000             mov        dword ptr [eax + 0x708], edx
004ce662 f30f2a4580               cvtsi2ss   xmm0, dword ptr [ebp - 0x80]
004ce667 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce66f 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce675 f30f118080000000         movss      dword ptr [eax + 0x80], xmm0
004ce67d eb22                     jmp        0x4ce6a1
004ce67f 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce685 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004ce688 83e2f9                   and        edx, 0xfffffff9
004ce68b 83ca02                   or         edx, 2
004ce68e 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce694 89501c                   mov        dword ptr [eax + 0x1c], edx
004ce697 b801000000               mov        eax, 1
004ce69c e920020000               jmp        0x4ce8c1
004ce6a1 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
004ce6ab 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce6ae 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce6b1 7d27                     jge        0x4ce6da
004ce6b3 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce6b6 035580                   add        edx, dword ptr [ebp - 0x80]
004ce6b9 0fb602                   movzx      eax, byte ptr [edx]
004ce6bc 85c0                     test       eax, eax
004ce6be 751a                     jne        0x4ce6da
004ce6c0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce6c3 83c101                   add        ecx, 1
004ce6c6 894d80                   mov        dword ptr [ebp - 0x80], ecx
004ce6c9 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
004ce6cf 83c201                   add        edx, 1
004ce6d2 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004ce6d8 ebd1                     jmp        0x4ce6ab
004ce6da 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004ce6dd 3b450c                   cmp        eax, dword ptr [ebp + 0xc]
004ce6e0 0f8dd9010000             jge        0x4ce8bf
004ce6e6 f30f2a8578ffffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x88]
004ce6ee f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce6f6 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce6fc f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004ce701 f30f5cc8                 subss      xmm1, xmm0
004ce705 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004ce70b f30f108208070000         movss      xmm0, dword ptr [edx + 0x708]
004ce713 f30f5cc1                 subss      xmm0, xmm1
004ce717 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce71d f30f118008070000         movss      dword ptr [eax + 0x708], xmm0
004ce725 f30f2a8578ffffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x88]
004ce72d f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce735 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce73b f30f114174               movss      dword ptr [ecx + 0x74], xmm0
004ce740 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ce748 f30f590568cd5600         mulss      xmm0, dword ptr [0x56cd68]
004ce750 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004ce756 0f2f4274                 comiss     xmm0, dword ptr [edx + 0x74]
004ce75a 7618                     jbe        0x4ce774
004ce75c 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce762 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004ce765 83e1f9                   and        ecx, 0xfffffff9
004ce768 83c902                   or         ecx, 2
004ce76b 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004ce771 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004ce774 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004ce777 3b450c                   cmp        eax, dword ptr [ebp + 0xc]
004ce77a 0f8d3f010000             jge        0x4ce8bf
004ce780 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce783 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce786 7d18                     jge        0x4ce7a0
004ce788 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce78b 035580                   add        edx, dword ptr [ebp - 0x80]
004ce78e 0fb602                   movzx      eax, byte ptr [edx]
004ce791 85c0                     test       eax, eax
004ce793 740b                     je         0x4ce7a0
004ce795 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce798 83c101                   add        ecx, 1
004ce79b 894d80                   mov        dword ptr [ebp - 0x80], ecx
004ce79e ebe0                     jmp        0x4ce780
004ce7a0 8b5580                   mov        edx, dword ptr [ebp - 0x80]
004ce7a3 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
004ce7a6 7c05                     jl         0x4ce7ad
004ce7a8 e912010000               jmp        0x4ce8bf
004ce7ad c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
004ce7b7 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004ce7ba 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004ce7c0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce7c3 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce7c6 7d27                     jge        0x4ce7ef
004ce7c8 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce7cb 035580                   add        edx, dword ptr [ebp - 0x80]
004ce7ce 0fb602                   movzx      eax, byte ptr [edx]
004ce7d1 85c0                     test       eax, eax
004ce7d3 751a                     jne        0x4ce7ef
004ce7d5 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004ce7d8 83c101                   add        ecx, 1
004ce7db 894d80                   mov        dword ptr [ebp - 0x80], ecx
004ce7de 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
004ce7e4 83c201                   add        edx, 1
004ce7e7 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004ce7ed ebd1                     jmp        0x4ce7c0
004ce7ef 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
004ce7f5 05f8060000               add        eax, 0x6f8
004ce7fa 50                       push       eax
004ce7fb 8d4d84                   lea        ecx, [ebp - 0x7c]
004ce7fe e83da1ffff               call       0x4c8940
004ce803 c745fc00000000           mov        dword ptr [ebp - 4], 0
004ce80a f30f2a8578ffffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x88]
004ce812 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce81a f30f114598               movss      dword ptr [ebp - 0x68], xmm0
004ce81f f30f104598               movss      xmm0, dword ptr [ebp - 0x68]
004ce824 f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
004ce829 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ce831 f30f590568cd5600         mulss      xmm0, dword ptr [0x56cd68]
004ce839 f30f104d94               movss      xmm1, dword ptr [ebp - 0x6c]
004ce83e 0f2fc8                   comiss     xmm1, xmm0
004ce841 7667                     jbe        0x4ce8aa
004ce843 f30f2a8574ffffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x8c]
004ce84b 51                       push       ecx
004ce84c f30f110424               movss      dword ptr [esp], xmm0
004ce851 8d8d58ffffff             lea        ecx, [ebp - 0xa8]
004ce857 51                       push       ecx
004ce858 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce85b e890aaf5ff               call       0x4292f0
004ce860 50                       push       eax
004ce861 8d954cffffff             lea        edx, [ebp - 0xb4]
004ce867 52                       push       edx
004ce868 8d4dd8                   lea        ecx, [ebp - 0x28]
004ce86b e800adf5ff               call       0x429570
004ce870 8b08                     mov        ecx, dword ptr [eax]
004ce872 894d84                   mov        dword ptr [ebp - 0x7c], ecx
004ce875 8b5004                   mov        edx, dword ptr [eax + 4]
004ce878 895588                   mov        dword ptr [ebp - 0x78], edx
004ce87b 8b4008                   mov        eax, dword ptr [eax + 8]
004ce87e 89458c                   mov        dword ptr [ebp - 0x74], eax
004ce881 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce887 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ce88d e8fee1f4ff               call       0x41ca90
004ce892 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004ce898 8d5584                   lea        edx, [ebp - 0x7c]
004ce89b 52                       push       edx
004ce89c 6a00                     push       0
004ce89e 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004ce8a4 e8475f0000               call       0x4d47f0
004ce8a9 90                       nop        
004ce8aa c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004ce8b1 8d4d84                   lea        ecx, [ebp - 0x7c]
004ce8b4 e8f7dbfaff               call       0x47c4b0
004ce8b9 90                       nop        
004ce8ba e9b5feffff               jmp        0x4ce774
004ce8bf 33c0                     xor        eax, eax
004ce8c1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ce8c4 64890d00000000           mov        dword ptr fs:[0], ecx
004ce8cb 59                       pop        ecx
004ce8cc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ce8cf 33cd                     xor        ecx, ebp
004ce8d1 e818410700               call       0x5429ee
004ce8d6 8be5                     mov        esp, ebp
004ce8d8 5d                       pop        ebp
004ce8d9 c20800                   ret        8
004ce8dc cc                       int3       
004ce8dd cc                       int3       
004ce8de cc                       int3       
004ce8df cc                       int3       
