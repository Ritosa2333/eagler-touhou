
// block 00495600
00495600  push       ebp
00495601  mov        ebp, esp
00495603  add        esp, -0x20
00495606  mov        dword ptr [ebp - 0x20], eax
00495609  mov        eax, dword ptr [ebp + 0x18]
0049560c  mov        dword ptr [ebp - 0x10], eax
0049560f  mov        eax, dword ptr [ebp + 0x1c]
00495612  mov        dword ptr [ebp - 0xc], eax
00495615  jmp        0x495620

// block 00495620
00495620  fstp       qword ptr [ebp - 8]
00495623  mov        dword ptr [ebp - 0x1c], ecx
00495626  mov        eax, dword ptr [ebp + 0x10]
00495629  mov        ecx, dword ptr [ebp + 0x14]
0049562c  mov        dword ptr [ebp - 0x18], eax
0049562f  mov        dword ptr [ebp - 0x14], ecx
00495632  lea        eax, [ebp + 8]
00495635  lea        ecx, [ebp - 0x20]
00495638  push       eax
00495639  push       ecx
0049563a  push       edx
0049563b  call       0x496bb6

// block 00495640
00495640  add        esp, 0xc
00495643  fld        qword ptr [ebp - 8]
00495646  cmp        word ptr [ebp + 8], 0x27f
0049564c  je         0x495651

// block 0049564e
0049564e  fldcw      word ptr [ebp + 8]

// block 00495651
00495651  leave      
00495652  ret        
