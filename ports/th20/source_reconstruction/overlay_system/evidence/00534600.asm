00534600 55                       push       ebp
00534601 8bec                     mov        ebp, esp
00534603 83ec1c                   sub        esp, 0x1c
00534606 894dfc                   mov        dword ptr [ebp - 4], ecx
00534609 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053460c 8b4834                   mov        ecx, dword ptr [eax + 0x34]
0053460f 51                       push       ecx
00534610 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534616 e825e1ffff               call       0x532740
0053461b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053461e c7423400000000           mov        dword ptr [edx + 0x34], 0
00534625 6a00                     push       0
00534627 e854faf2ff               call       0x464080
0053462c 83c404                   add        esp, 4
0053462f 8945f8                   mov        dword ptr [ebp - 8], eax
00534632 6a00                     push       0
00534634 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00534637 e8a4000000               call       0x5346e0
0053463c 90                       nop        
0053463d 837d0c08                 cmp        dword ptr [ebp + 0xc], 8
00534641 7532                     jne        0x534675
00534643 6a00                     push       0
00534645 e836faf2ff               call       0x464080
0053464a 83c404                   add        esp, 4
0053464d 8945f4                   mov        dword ptr [ebp - 0xc], eax
00534650 6a00                     push       0
00534652 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00534655 e8265cfdff               call       0x50a280
0053465a 89450c                   mov        dword ptr [ebp + 0xc], eax
0053465d 6a00                     push       0
0053465f e81cfaf2ff               call       0x464080
00534664 83c404                   add        esp, 4
00534667 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053466a 6a01                     push       1
0053466c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053466f e86c000000               call       0x5346e0
00534674 90                       nop        
00534675 6a00                     push       0
00534677 e804faf2ff               call       0x464080
0053467c 83c404                   add        esp, 4
0053467f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00534682 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534685 50                       push       eax
00534686 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00534689 e80298eeff               call       0x41de90
0053468e 6b4d0824                 imul       ecx, dword ptr [ebp + 8], 0x24
00534692 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00534695 8b8491b00a5b00           mov        eax, dword ptr [ecx + edx*4 + 0x5b0ab0]
0053469c ffd0                     call       eax
0053469e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005346a1 894134                   mov        dword ptr [ecx + 0x34], eax
005346a4 8b55fc                   mov        edx, dword ptr [ebp - 4]
005346a7 8b4234                   mov        eax, dword ptr [edx + 0x34]
005346aa 8945e8                   mov        dword ptr [ebp - 0x18], eax
005346ad 6a03                     push       3
005346af 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005346b2 e809e7edff               call       0x412dc0
005346b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005346ba 8b5134                   mov        edx, dword ptr [ecx + 0x34]
005346bd 8955e4                   mov        dword ptr [ebp - 0x1c], edx
005346c0 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005346c3 50                       push       eax
005346c4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005346c7 e8a4ceedff               call       0x411570
005346cc 90                       nop        
005346cd 8be5                     mov        esp, ebp
005346cf 5d                       pop        ebp
005346d0 c20800                   ret        8
005346d3 cc                       int3       
005346d4 cc                       int3       
005346d5 cc                       int3       
005346d6 cc                       int3       
005346d7 cc                       int3       
005346d8 cc                       int3       
005346d9 cc                       int3       
005346da cc                       int3       
005346db cc                       int3       
005346dc cc                       int3       
005346dd cc                       int3       
005346de cc                       int3       
005346df cc                       int3       
