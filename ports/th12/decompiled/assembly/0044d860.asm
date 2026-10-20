
// block 0044d860
0044d860  cmp        byte ptr [ecx + 3], 0
0044d864  je         0x44d87d

// block 0044d866
0044d866  push       esi
0044d867  movzx      esi, byte ptr [ecx + 2]
0044d86b  add        dword ptr [eax], esi
0044d86d  movzx      esi, byte ptr [ecx + 1]
0044d871  movzx      ecx, byte ptr [ecx]
0044d874  add        dword ptr [eax + 4], esi
0044d877  add        dword ptr [eax + 8], ecx
0044d87a  inc        dword ptr [edx]
0044d87c  pop        esi

// block 0044d87d
0044d87d  ret        
