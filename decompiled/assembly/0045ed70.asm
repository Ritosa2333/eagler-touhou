
// block 0045ed70
0045ed70  movzx      eax, word ptr [eax]
0045ed73  test       eax, 0x8000
0045ed78  je         0x45ed9c

// block 0045ed7a
0045ed7a  push       esi
0045ed7b  movzx      eax, ax
0045ed7e  mov        esi, eax
0045ed80  shr        esi, 0xa
0045ed83  and        esi, 0x1f
0045ed86  add        dword ptr [ecx], esi
0045ed88  mov        esi, eax
0045ed8a  shr        esi, 5
0045ed8d  and        esi, 0x1f
0045ed90  add        dword ptr [ecx + 4], esi
0045ed93  and        eax, 0x1f
0045ed96  add        dword ptr [ecx + 8], eax
0045ed99  inc        dword ptr [edx]
0045ed9b  pop        esi

// block 0045ed9c
0045ed9c  ret        
