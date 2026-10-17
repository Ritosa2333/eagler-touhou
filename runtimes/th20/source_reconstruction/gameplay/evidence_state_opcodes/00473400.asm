00473400 55                       push       ebp
00473401 8bec                     mov        ebp, esp
00473403 51                       push       ecx
00473404 894dfc                   mov        dword ptr [ebp - 4], ecx
00473407 b804000000               mov        eax, 4
0047340c 6bc800                   imul       ecx, eax, 0
0047340f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473412 f30f2c440a08             cvttss2si  eax, dword ptr [edx + ecx + 8]
00473418 b901000000               mov        ecx, 1
0047341d 6bd100                   imul       edx, ecx, 0
00473420 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473423 88441118                 mov        byte ptr [ecx + edx + 0x18], al
00473427 ba04000000               mov        edx, 4
0047342c c1e200                   shl        edx, 0
0047342f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00473432 f30f2c4c1008             cvttss2si  ecx, dword ptr [eax + edx + 8]
00473438 ba01000000               mov        edx, 1
0047343d c1e200                   shl        edx, 0
00473440 8b45fc                   mov        eax, dword ptr [ebp - 4]
00473443 884c1018                 mov        byte ptr [eax + edx + 0x18], cl
00473447 b904000000               mov        ecx, 4
0047344c d1e1                     shl        ecx, 1
0047344e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473451 f30f2c440a08             cvttss2si  eax, dword ptr [edx + ecx + 8]
00473457 b901000000               mov        ecx, 1
0047345c d1e1                     shl        ecx, 1
0047345e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473461 88440a18                 mov        byte ptr [edx + ecx + 0x18], al
00473465 b804000000               mov        eax, 4
0047346a 6bc803                   imul       ecx, eax, 3
0047346d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473470 f30f2c440a08             cvttss2si  eax, dword ptr [edx + ecx + 8]
00473476 b901000000               mov        ecx, 1
0047347b 6bd103                   imul       edx, ecx, 3
0047347e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473481 88441118                 mov        byte ptr [ecx + edx + 0x18], al
00473485 8be5                     mov        esp, ebp
00473487 5d                       pop        ebp
00473488 c3                       ret        
00473489 cc                       int3       
0047348a cc                       int3       
0047348b cc                       int3       
0047348c cc                       int3       
0047348d cc                       int3       
0047348e cc                       int3       
0047348f cc                       int3       
