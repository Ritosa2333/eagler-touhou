
// block 0048ce8c
0048ce8c  cmp        dword ptr [ebp - 0x1c], edi
0048ce8f  jne        0x48ceac

// block 0048ce91
0048ce91  mov        eax, esi
0048ce93  sar        eax, 5
0048ce96  mov        ecx, esi
0048ce98  and        ecx, 0x1f
0048ce9b  shl        ecx, 6
0048ce9e  mov        eax, dword ptr [eax*4 + 0x4d6320]
0048cea5  lea        eax, [eax + ecx + 4]
0048cea9  and        byte ptr [eax], 0xfe

// block 0048ceac
0048ceac  push       esi
0048cead  call       0x48cbdd

// block 0048ceb2
0048ceb2  pop        ecx
0048ceb3  ret        
