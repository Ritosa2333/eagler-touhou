
// block 00460610
00460610  lea        eax, [eax + eax*8]
00460613  push       esi
00460614  add        eax, eax
00460616  push       edi
00460617  mov        edi, dword ptr [edx + 0x118]
0046061d  add        eax, eax
0046061f  add        eax, eax
00460621  add        edi, eax
00460623  mov        ecx, 0x12
00460628  mov        esi, ebx

// block 0046062a
0046062a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0046062c
0046062c  mov        ecx, dword ptr [edx + 0x118]
00460632  add        ecx, eax
00460634  pop        edi
00460635  pop        esi
00460636  fld        dword ptr [ecx + 0xc]
00460639  fdiv       dword ptr [ecx + 0x20]
0046063c  fstp       dword ptr [ecx + 0x24]
0046063f  mov        ecx, dword ptr [edx + 0x118]
00460645  fld        dword ptr [ecx + eax + 0x14]
00460649  fdiv       dword ptr [ecx + eax + 0x20]
0046064d  add        ecx, eax
0046064f  fstp       dword ptr [ecx + 0x2c]
00460652  mov        ecx, dword ptr [edx + 0x118]
00460658  fld        dword ptr [ecx + eax + 0x10]
0046065c  fdiv       dword ptr [ecx + eax + 0x1c]
00460660  add        ecx, eax
00460662  fstp       dword ptr [ecx + 0x28]
00460665  mov        ecx, dword ptr [edx + 0x118]
0046066b  fld        dword ptr [ecx + eax + 0x18]
0046066f  fdiv       dword ptr [ecx + eax + 0x1c]
00460673  add        ecx, eax
00460675  fstp       dword ptr [ecx + 0x30]
00460678  mov        ecx, dword ptr [edx + 0x118]
0046067e  fld        dword ptr [ecx + eax + 0x14]
00460682  fsub       dword ptr [ecx + eax + 0xc]
00460686  add        ecx, eax
00460688  fdiv       dword ptr [ebx + 0x3c]
0046068b  fstp       dword ptr [ecx + 0x38]
0046068e  mov        edx, dword ptr [edx + 0x118]
00460694  fld        dword ptr [eax + edx + 0x18]
00460698  fsub       dword ptr [eax + edx + 0x10]
0046069c  add        eax, edx
0046069e  fdiv       dword ptr [ebx + 0x40]
004606a1  fstp       dword ptr [eax + 0x34]
004606a4  ret        
