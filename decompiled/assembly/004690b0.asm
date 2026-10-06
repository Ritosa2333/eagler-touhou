
// block 004690b0
004690b0  mov        eax, dword ptr [edx + 4]
004690b3  push       esi
004690b4  movzx      esi, word ptr [eax + 8]
004690b8  push       edi
004690b9  mov        edi, 1
004690be  shl        edi, cl
004690c0  test       edi, esi
004690c2  je         0x469104

// block 004690c4
004690c4  fldz       
004690c6  lea        ecx, [eax + ecx*4 + 0x10]
004690ca  fcomp      dword ptr [ecx]
004690cc  fnstsw     ax
004690ce  fld        dword ptr [ecx]
004690d0  test       ah, 0x41
004690d3  jp         0x4690ec

// block 004690d5
004690d5  lea        esi, [edx + 8]
004690d8  call       0x4931e0

// block 004690dd
004690dd  mov        ecx, eax
004690df  mov        eax, dword ptr [esi + 0x1004]
004690e5  add        eax, ecx
004690e7  pop        edi
004690e8  add        eax, esi
004690ea  pop        esi
004690eb  ret        

// block 004690ec
004690ec  mov        esi, dword ptr [edx + 0x1014]
004690f2  mov        edi, dword ptr [esi]
004690f4  call       0x4931e0

// block 004690f9
004690f9  mov        edx, dword ptr [edi + 0x10]
004690fc  push       eax
004690fd  mov        ecx, esi
004690ff  call       edx

// block 00469101
00469101  pop        edi
00469102  pop        esi
00469103  ret        

// block 00469104
00469104  pop        edi
00469105  xor        eax, eax
00469107  pop        esi
00469108  ret        
