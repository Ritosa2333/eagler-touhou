004c5e00 55                       push       ebp
004c5e01 8bec                     mov        ebp, esp
004c5e03 51                       push       ecx
004c5e04 894dfc                   mov        dword ptr [ebp - 4], ecx
004c5e07 b801000000               mov        eax, 1
004c5e0c 6bc800                   imul       ecx, eax, 0
004c5e0f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c5e12 8d840ad0020000           lea        eax, [edx + ecx + 0x2d0]
004c5e19 8be5                     mov        esp, ebp
004c5e1b 5d                       pop        ebp
004c5e1c c3                       ret        
004c5e1d cc                       int3       
004c5e1e cc                       int3       
004c5e1f cc                       int3       
