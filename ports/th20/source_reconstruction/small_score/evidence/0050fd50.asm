0050fd50 55                       push       ebp
0050fd51 8bec                     mov        ebp, esp
0050fd53 51                       push       ecx
0050fd54 894dfc                   mov        dword ptr [ebp - 4], ecx
0050fd57 33c0                     xor        eax, eax
0050fd59 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd5c 8901                     mov        dword ptr [ecx], eax
0050fd5e 894104                   mov        dword ptr [ecx + 4], eax
0050fd61 894108                   mov        dword ptr [ecx + 8], eax
0050fd64 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd67 83c10c                   add        ecx, 0xc
0050fd6a e8a130f1ff               call       0x422e10
0050fd6f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050fd72 0f57c0                   xorps      xmm0, xmm0
0050fd75 f30f114218               movss      dword ptr [edx + 0x18], xmm0
0050fd7a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050fd7d c7401c00000000           mov        dword ptr [eax + 0x1c], 0
0050fd84 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd87 83c120                   add        ecx, 0x20
0050fd8a e80130f1ff               call       0x422d90
0050fd8f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd92 0f57c0                   xorps      xmm0, xmm0
0050fd95 f30f114130               movss      dword ptr [ecx + 0x30], xmm0
0050fd9a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050fd9d 0f57c0                   xorps      xmm0, xmm0
0050fda0 f30f114234               movss      dword ptr [edx + 0x34], xmm0
0050fda5 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050fda8 c6403800                 mov        byte ptr [eax + 0x38], 0
0050fdac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fdaf c6413900                 mov        byte ptr [ecx + 0x39], 0
0050fdb3 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050fdb6 c7423c00000000           mov        dword ptr [edx + 0x3c], 0
0050fdbd 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050fdc0 0f57c0                   xorps      xmm0, xmm0
0050fdc3 f30f114040               movss      dword ptr [eax + 0x40], xmm0
0050fdc8 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050fdcb 8be5                     mov        esp, ebp
0050fdcd 5d                       pop        ebp
0050fdce c3                       ret        
0050fdcf cc                       int3       
