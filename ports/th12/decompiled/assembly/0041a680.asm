
// block 0041a680
0041a680  push       esi
0041a681  lea        esi, [eax + 0x271]
0041a687  shl        esi, 4
0041a68a  shl        eax, 4
0041a68d  add        eax, edx
0041a68f  mov        dword ptr [esi + edx], edi
0041a692  mov        dword ptr [eax + 0x2714], ecx
0041a698  mov        dword ptr [eax + 0x2718], ecx
0041a69e  pop        esi
0041a69f  ret        
