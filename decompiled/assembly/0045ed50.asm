
// block 0045ed50
0045ed50  cmp        byte ptr [ecx + 3], 0
0045ed54  je         0x45ed6d

// block 0045ed56
0045ed56  push       esi
0045ed57  movzx      esi, byte ptr [ecx + 2]
0045ed5b  add        dword ptr [eax], esi
0045ed5d  movzx      esi, byte ptr [ecx + 1]
0045ed61  movzx      ecx, byte ptr [ecx]
0045ed64  add        dword ptr [eax + 4], esi
0045ed67  add        dword ptr [eax + 8], ecx
0045ed6a  inc        dword ptr [edx]
0045ed6c  pop        esi

// block 0045ed6d
0045ed6d  ret        
