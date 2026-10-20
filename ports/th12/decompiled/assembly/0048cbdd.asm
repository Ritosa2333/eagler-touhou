
// block 0048cbdd
0048cbdd  mov        edi, edi
0048cbdf  push       ebp
0048cbe0  mov        ebp, esp
0048cbe2  mov        eax, dword ptr [ebp + 8]
0048cbe5  mov        ecx, eax
0048cbe7  and        eax, 0x1f
0048cbea  sar        ecx, 5
0048cbed  mov        ecx, dword ptr [ecx*4 + 0x4d6320]
0048cbf4  shl        eax, 6
0048cbf7  lea        eax, [ecx + eax + 0xc]
0048cbfb  push       eax
0048cbfc  call       dword ptr [0x49808c]

// block 0048cc02
0048cc02  pop        ebp
0048cc03  ret        
