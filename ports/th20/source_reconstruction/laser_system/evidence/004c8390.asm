004c8390 55                       push       ebp
004c8391 8bec                     mov        ebp, esp
004c8393 51                       push       ecx
004c8394 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8397 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c839a e811020000               call       0x4c85b0
004c839f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c83a2 c70074135700             mov        dword ptr [eax], 0x571374
004c83a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c83ab 81c1f8060000             add        ecx, 0x6f8
004c83b1 e8fa31fcff               call       0x48b5b0
004c83b6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c83b9 81c148070000             add        ecx, 0x748
004c83bf e87c07f8ff               call       0x448b40
004c83c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c83c7 0f57c0                   xorps      xmm0, xmm0
004c83ca f30f11812c0d0000         movss      dword ptr [ecx + 0xd2c], xmm0
004c83d2 6800080000               push       0x800
004c83d7 6a00                     push       0
004c83d9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c83dc 81c2300d0000             add        edx, 0xd30
004c83e2 52                       push       edx
004c83e3 e878c30700               call       0x544760
004c83e8 83c40c                   add        esp, 0xc
004c83eb 6800080000               push       0x800
004c83f0 6a00                     push       0
004c83f2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c83f5 0530150000               add        eax, 0x1530
004c83fa 50                       push       eax
004c83fb e860c30700               call       0x544760
004c8400 83c40c                   add        esp, 0xc
004c8403 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8406 8be5                     mov        esp, ebp
004c8408 5d                       pop        ebp
004c8409 c3                       ret        
004c840a cc                       int3       
004c840b cc                       int3       
004c840c cc                       int3       
004c840d cc                       int3       
004c840e cc                       int3       
004c840f cc                       int3       
