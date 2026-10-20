
// block 00410845
00410845  push       esi
00410846  mov        esi, dword ptr [ecx + 0x478]
0041084c  push       edi
0041084d  push       ecx
0041084e  call       0x459a60

// block 00410853
00410853  fldz       
00410855  mov        eax, dword ptr [esi + 0x28]
00410858  mov        edi, dword ptr [0x4ce8cc]
0041085e  push       eax
0041085f  mov        eax, dword ptr [esi + 0x24]
00410862  push       0x20
00410864  sub        esp, 0x10
00410867  fstp       dword ptr [esp + 0xc]
0041086b  fld        dword ptr [esi + 0x20]
0041086e  fstp       dword ptr [esp + 8]
00410872  fld        dword ptr [esi + 4]
00410875  fstp       dword ptr [esp + 4]
00410879  fld        dword ptr [esi]
0041087b  fstp       dword ptr [esp]
0041087e  call       0x45d430

// block 00410883
00410883  pop        edi
00410884  xor        eax, eax
00410886  pop        esi
00410887  ret        
