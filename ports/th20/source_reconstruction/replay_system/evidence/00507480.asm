00507480 55                       push       ebp
00507481 8bec                     mov        ebp, esp
00507483 51                       push       ecx
00507484 894dfc                   mov        dword ptr [ebp - 4], ecx
00507487 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050748a c70074323072             mov        dword ptr [eax], 0x72303274
00507490 b901000000               mov        ecx, 1
00507495 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507498 66894a04                 mov        word ptr [edx + 4], cx
0050749c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050749f c6400600                 mov        byte ptr [eax + 6], 0
005074a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005074a6 c6410700                 mov        byte ptr [ecx + 7], 0
005074aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
005074ad c6420800                 mov        byte ptr [edx + 8], 0
005074b1 8b45fc                   mov        eax, dword ptr [ebp - 4]
005074b4 c7400c00000000           mov        dword ptr [eax + 0xc], 0
005074bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005074be c7411000010000           mov        dword ptr [ecx + 0x10], 0x100
005074c5 8b55fc                   mov        edx, dword ptr [ebp - 4]
005074c8 c7421400000000           mov        dword ptr [edx + 0x14], 0
005074cf 8b45fc                   mov        eax, dword ptr [ebp - 4]
005074d2 c6401800                 mov        byte ptr [eax + 0x18], 0
005074d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005074d9 c6411900                 mov        byte ptr [ecx + 0x19], 0
005074dd 33d2                     xor        edx, edx
005074df 8b45fc                   mov        eax, dword ptr [ebp - 4]
005074e2 83c01a                   add        eax, 0x1a
005074e5 668910                   mov        word ptr [eax], dx
005074e8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005074eb c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
005074f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
005074f5 c7422000000000           mov        dword ptr [edx + 0x20], 0
005074fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
005074ff c7402400000000           mov        dword ptr [eax + 0x24], 0
00507506 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507509 c7412800000000           mov        dword ptr [ecx + 0x28], 0
00507510 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507513 c7422c00000000           mov        dword ptr [edx + 0x2c], 0
0050751a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050751d 8be5                     mov        esp, ebp
0050751f 5d                       pop        ebp
00507520 c3                       ret        
00507521 cc                       int3       
00507522 cc                       int3       
00507523 cc                       int3       
00507524 cc                       int3       
00507525 cc                       int3       
00507526 cc                       int3       
00507527 cc                       int3       
00507528 cc                       int3       
00507529 cc                       int3       
0050752a cc                       int3       
0050752b cc                       int3       
0050752c cc                       int3       
0050752d cc                       int3       
0050752e cc                       int3       
0050752f cc                       int3       
