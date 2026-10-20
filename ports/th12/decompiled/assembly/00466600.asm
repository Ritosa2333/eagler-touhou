
// block 00466600
00466600  push       ecx
00466601  push       ebx
00466602  xor        ebx, ebx
00466604  cmp        dword ptr [edi + 4], ebx
00466607  jne        0x46660e

// block 00466609
00466609  xor        eax, eax
0046660b  pop        ebx
0046660c  pop        ecx
0046660d  ret        

// block 0046660e
0046660e  push       esi
0046660f  xor        esi, esi
00466611  cmp        dword ptr [edi + 0x10], ebx
00466614  jbe        0x466648

// block 00466616
00466616  mov        eax, dword ptr [edi + 4]
00466619  cmp        dword ptr [eax + esi*4], 0
0046661d  je         0x466642

// block 0046661f
0046661f  mov        ecx, eax
00466621  mov        dword ptr [esp + 8], 0
00466629  mov        eax, dword ptr [ecx + esi*4]
0046662c  mov        edx, dword ptr [eax]
0046662e  mov        edx, dword ptr [edx + 0x24]
00466631  lea        ecx, [esp + 8]
00466635  push       ecx
00466636  push       eax
00466637  call       edx

// block 00466639
00466639  mov        eax, dword ptr [esp + 8]
0046663d  and        eax, 1
00466640  or         ebx, eax

// block 00466642
00466642  inc        esi
00466643  cmp        esi, dword ptr [edi + 0x10]
00466646  jb         0x466616

// block 00466648
00466648  pop        esi
00466649  mov        eax, ebx
0046664b  pop        ebx
0046664c  pop        ecx
0046664d  ret        
