
// block 00473138
00473138  mov        edi, edi
0047313a  push       ebp
0047313b  mov        ebp, esp
0047313d  xor        eax, eax

// block 0047313f
0047313f  mov        ecx, dword ptr [ebp + 8]
00473142  cmp        ecx, dword ptr [eax*8 + 0x4ad3f8]
00473149  je         0x473155

// block 0047314b
0047314b  inc        eax
0047314c  cmp        eax, 0x17
0047314f  jb         0x47313f

// block 00473151
00473151  xor        eax, eax
00473153  pop        ebp
00473154  ret        

// block 00473155
00473155  mov        eax, dword ptr [eax*8 + 0x4ad3fc]
0047315c  pop        ebp
0047315d  ret        
