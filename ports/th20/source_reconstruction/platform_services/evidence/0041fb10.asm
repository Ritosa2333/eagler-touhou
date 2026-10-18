0041fb10 55                       push       ebp
0041fb11 8bec                     mov        ebp, esp
0041fb13 51                       push       ecx
0041fb14 894dfc                   mov        dword ptr [ebp - 4], ecx
0041fb17 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb1a e831010000               call       0x41fc50
0041fb1f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb22 83c110                   add        ecx, 0x10
0041fb25 e826010000               call       0x41fc50
0041fb2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb2d 83c120                   add        ecx, 0x20
0041fb30 e81b010000               call       0x41fc50
0041fb35 33c0                     xor        eax, eax
0041fb37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb3a 668901                   mov        word ptr [ecx], ax
0041fb3d ba01000000               mov        edx, 1
0041fb42 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fb45 66895002                 mov        word ptr [eax + 2], dx
0041fb49 b902000000               mov        ecx, 2
0041fb4e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fb51 66894a04                 mov        word ptr [edx + 4], cx
0041fb55 b803000000               mov        eax, 3
0041fb5a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb5d 66894106                 mov        word ptr [ecx + 6], ax
0041fb61 83caff                   or         edx, 0xffffffff
0041fb64 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fb67 66895008                 mov        word ptr [eax + 8], dx
0041fb6b 83c9ff                   or         ecx, 0xffffffff
0041fb6e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fb71 66894a0a                 mov        word ptr [edx + 0xa], cx
0041fb75 83c8ff                   or         eax, 0xffffffff
0041fb78 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb7b 6689410c                 mov        word ptr [ecx + 0xc], ax
0041fb7f 83caff                   or         edx, 0xffffffff
0041fb82 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fb85 6689500e                 mov        word ptr [eax + 0xe], dx
0041fb89 33c9                     xor        ecx, ecx
0041fb8b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fb8e 66894a10                 mov        word ptr [edx + 0x10], cx
0041fb92 b801000000               mov        eax, 1
0041fb97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fb9a 66894112                 mov        word ptr [ecx + 0x12], ax
0041fb9e ba05000000               mov        edx, 5
0041fba3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fba6 66895014                 mov        word ptr [eax + 0x14], dx
0041fbaa b90a000000               mov        ecx, 0xa
0041fbaf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fbb2 66894a16                 mov        word ptr [edx + 0x16], cx
0041fbb6 83c8ff                   or         eax, 0xffffffff
0041fbb9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fbbc 66894118                 mov        word ptr [ecx + 0x18], ax
0041fbc0 83caff                   or         edx, 0xffffffff
0041fbc3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fbc6 6689501a                 mov        word ptr [eax + 0x1a], dx
0041fbca 83c9ff                   or         ecx, 0xffffffff
0041fbcd 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fbd0 66894a1c                 mov        word ptr [edx + 0x1c], cx
0041fbd4 83c8ff                   or         eax, 0xffffffff
0041fbd7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fbda 6689411e                 mov        word ptr [ecx + 0x1e], ax
0041fbde ba5a000000               mov        edx, 0x5a
0041fbe3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fbe6 66895020                 mov        word ptr [eax + 0x20], dx
0041fbea b958000000               mov        ecx, 0x58
0041fbef 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fbf2 66894a22                 mov        word ptr [edx + 0x22], cx
0041fbf6 b810000000               mov        eax, 0x10
0041fbfb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fbfe 66894124                 mov        word ptr [ecx + 0x24], ax
0041fc02 ba1b000000               mov        edx, 0x1b
0041fc07 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fc0a 66895026                 mov        word ptr [eax + 0x26], dx
0041fc0e b926000000               mov        ecx, 0x26
0041fc13 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fc16 66894a28                 mov        word ptr [edx + 0x28], cx
0041fc1a b828000000               mov        eax, 0x28
0041fc1f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041fc22 6689412a                 mov        word ptr [ecx + 0x2a], ax
0041fc26 ba25000000               mov        edx, 0x25
0041fc2b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fc2e 6689502c                 mov        word ptr [eax + 0x2c], dx
0041fc32 b927000000               mov        ecx, 0x27
0041fc37 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041fc3a 66894a2e                 mov        word ptr [edx + 0x2e], cx
0041fc3e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041fc41 8be5                     mov        esp, ebp
0041fc43 5d                       pop        ebp
0041fc44 c3                       ret        
0041fc45 cc                       int3       
0041fc46 cc                       int3       
0041fc47 cc                       int3       
0041fc48 cc                       int3       
0041fc49 cc                       int3       
0041fc4a cc                       int3       
0041fc4b cc                       int3       
0041fc4c cc                       int3       
