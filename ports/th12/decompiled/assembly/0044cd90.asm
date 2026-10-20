
// block 0044cd90
0044cd90  push       ebx
0044cd91  mov        ebx, dword ptr [esp + 0xc]
0044cd95  push       esi
0044cd96  push       edi
0044cd97  mov        edi, ecx
0044cd99  mov        esi, dword ptr [edi + 0xc]
0044cd9c  add        esi, dword ptr [edi + 4]
0044cd9f  mov        eax, dword ptr [edi + 8]
0044cda2  sub        esi, eax
0044cda4  cmp        esi, ebx
0044cda6  jb         0x44cdc2

// block 0044cda8
0044cda8  push       ebx
0044cda9  push       eax
0044cdaa  mov        eax, dword ptr [esp + 0x18]
0044cdae  push       eax
0044cdaf  call       0x47a330

// block 0044cdb4
0044cdb4  add        esp, 0xc
0044cdb7  add        dword ptr [edi + 8], ebx
0044cdba  pop        edi
0044cdbb  pop        esi
0044cdbc  mov        eax, ebx
0044cdbe  pop        ebx
0044cdbf  ret        8

// block 0044cdc2
0044cdc2  test       esi, esi
0044cdc4  je         0x44cde0

// block 0044cdc6
0044cdc6  mov        ecx, dword ptr [esp + 0x10]
0044cdca  push       esi
0044cdcb  push       eax
0044cdcc  push       ecx
0044cdcd  call       0x47a330

// block 0044cdd2
0044cdd2  add        esp, 0xc
0044cdd5  add        dword ptr [edi + 8], esi
0044cdd8  pop        edi
0044cdd9  mov        eax, esi
0044cddb  pop        esi
0044cddc  pop        ebx
0044cddd  ret        8

// block 0044cde0
0044cde0  pop        edi
0044cde1  pop        esi
0044cde2  xor        eax, eax
0044cde4  pop        ebx
0044cde5  ret        8
