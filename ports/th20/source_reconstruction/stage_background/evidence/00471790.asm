00471790 55                       push       ebp
00471791 8bec                     mov        ebp, esp
00471793 51                       push       ecx
00471794 894dfc                   mov        dword ptr [ebp - 4], ecx
00471797 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047179a e87116fbff               call       0x422e10
0047179f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717a2 83c10c                   add        ecx, 0xc
004717a5 e86616fbff               call       0x422e10
004717aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717ad 83c118                   add        ecx, 0x18
004717b0 e85b16fbff               call       0x422e10
004717b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717b8 83c124                   add        ecx, 0x24
004717bb e85016fbff               call       0x422e10
004717c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717c3 83c130                   add        ecx, 0x30
004717c6 e84516fbff               call       0x422e10
004717cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717ce 83c13c                   add        ecx, 0x3c
004717d1 e83a16fbff               call       0x422e10
004717d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717d9 83c148                   add        ecx, 0x48
004717dc e82f16fbff               call       0x422e10
004717e1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004717e4 0f57c0                   xorps      xmm0, xmm0
004717e7 f30f114054               movss      dword ptr [eax + 0x54], xmm0
004717ec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004717ef c7415800000000           mov        dword ptr [ecx + 0x58], 0
004717f6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004717f9 c7425c00000000           mov        dword ptr [edx + 0x5c], 0
00471800 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471803 83c160                   add        ecx, 0x60
00471806 e895a5f9ff               call       0x40bda0
0047180b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047180e 81c1a0000000             add        ecx, 0xa0
00471814 e887a5f9ff               call       0x40bda0
00471819 33c0                     xor        eax, eax
0047181b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047181e 81c1e0000000             add        ecx, 0xe0
00471824 8901                     mov        dword ptr [ecx], eax
00471826 894104                   mov        dword ptr [ecx + 4], eax
00471829 894108                   mov        dword ptr [ecx + 8], eax
0047182c 89410c                   mov        dword ptr [ecx + 0xc], eax
0047182f 894110                   mov        dword ptr [ecx + 0x10], eax
00471832 894114                   mov        dword ptr [ecx + 0x14], eax
00471835 8b55fc                   mov        edx, dword ptr [ebp - 4]
00471838 c782f800000000000000     mov        dword ptr [edx + 0xf8], 0
00471842 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471845 c780fc00000000000000     mov        dword ptr [eax + 0xfc], 0
0047184f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471852 c7810001000000000000     mov        dword ptr [ecx + 0x100], 0
0047185c 33d2                     xor        edx, edx
0047185e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471861 0504010000               add        eax, 0x104
00471866 8910                     mov        dword ptr [eax], edx
00471868 895004                   mov        dword ptr [eax + 4], edx
0047186b 895008                   mov        dword ptr [eax + 8], edx
0047186e 89500c                   mov        dword ptr [eax + 0xc], edx
00471871 895010                   mov        dword ptr [eax + 0x10], edx
00471874 895014                   mov        dword ptr [eax + 0x14], edx
00471877 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047187a 81c11c010000             add        ecx, 0x11c
00471880 e81b80fcff               call       0x4398a0
00471885 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471888 81c124010000             add        ecx, 0x124
0047188e e80d80fcff               call       0x4398a0
00471893 68a0984300               push       0x4398a0
00471898 6a03                     push       3
0047189a 6a08                     push       8
0047189c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047189f 81c12c010000             add        ecx, 0x12c
004718a5 51                       push       ecx
004718a6 e875a3f9ff               call       0x40bc20
004718ab 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004718ae 81c144010000             add        ecx, 0x144
004718b4 e85715fbff               call       0x422e10
004718b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004718bc 81c150010000             add        ecx, 0x150
004718c2 e879000000               call       0x471940
004718c7 90                       nop        
004718c8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004718cb 8be5                     mov        esp, ebp
004718cd 5d                       pop        ebp
004718ce c3                       ret        
004718cf cc                       int3       
