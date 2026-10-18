00460570 55                       push       ebp
00460571 8bec                     mov        ebp, esp
00460573 83ec28                   sub        esp, 0x28
00460576 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0046057b 33c5                     xor        eax, ebp
0046057d 8945fc                   mov        dword ptr [ebp - 4], eax
00460580 894dec                   mov        dword ptr [ebp - 0x14], ecx
00460583 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00460586 8b4804                   mov        ecx, dword ptr [eax + 4]
00460589 51                       push       ecx
0046058a 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00460590 e82b56feff               call       0x445bc0
00460595 8d4df0                   lea        ecx, [ebp - 0x10]
00460598 e87328fcff               call       0x422e10
0046059d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004605a0 8b4204                   mov        eax, dword ptr [edx + 4]
004605a3 8945e8                   mov        dword ptr [ebp - 0x18], eax
004605a6 8d4df0                   lea        ecx, [ebp - 0x10]
004605a9 51                       push       ecx
004605aa 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004605ad e82e71fdff               call       0x4376e0
004605b2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004605b5 81c288020000             add        edx, 0x288
004605bb 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004605be 6a00                     push       0
004605c0 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004605c3 e8b83ffbff               call       0x414580
004605c8 8945e0                   mov        dword ptr [ebp - 0x20], eax
004605cb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004605ce 83c108                   add        ecx, 8
004605d1 e8cab7faff               call       0x40bda0
004605d6 8945dc                   mov        dword ptr [ebp - 0x24], eax
004605d9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004605dc 81c1d0030000             add        ecx, 0x3d0
004605e2 e8a9f9faff               call       0x40ff90
004605e7 8945d8                   mov        dword ptr [ebp - 0x28], eax
004605ea 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004605ed 50                       push       eax
004605ee 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004605f1 51                       push       ecx
004605f2 8d55f0                   lea        edx, [ebp - 0x10]
004605f5 52                       push       edx
004605f6 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004605f9 50                       push       eax
004605fa 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00460600 e89bcbfdff               call       0x43d1a0
00460605 33c0                     xor        eax, eax
00460607 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046060a 33cd                     xor        ecx, ebp
0046060c e8dd230e00               call       0x5429ee
00460611 8be5                     mov        esp, ebp
00460613 5d                       pop        ebp
00460614 c3                       ret        
00460615 cc                       int3       
00460616 cc                       int3       
00460617 cc                       int3       
00460618 cc                       int3       
00460619 cc                       int3       
0046061a cc                       int3       
0046061b cc                       int3       
0046061c cc                       int3       
0046061d cc                       int3       
0046061e cc                       int3       
0046061f cc                       int3       
