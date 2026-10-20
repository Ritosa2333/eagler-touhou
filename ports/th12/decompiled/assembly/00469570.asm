
// block 00469570
00469570  push       esi
00469571  push       0x103c
00469576  call       0x46c9ea

// block 0046957b
0046957b  xor        esi, esi
0046957d  add        esp, 4
00469580  cmp        eax, esi
00469582  je         0x469598

// block 00469584
00469584  mov        dword ptr [eax], 0x49fc34
0046958a  mov        dword ptr [eax + 0x1010], esi
00469590  mov        dword ptr [eax + 0x1014], esi
00469596  mov        esi, eax

// block 00469598
00469598  mov        eax, dword ptr [esp + 8]
0046959c  push       eax
0046959d  mov        eax, edi
0046959f  mov        dword ptr [esi + 0x102c], edi
004695a5  call       0x4694f0

// block 004695aa
004695aa  fldz       
004695ac  mov        ecx, dword ptr [esi + 4]
004695af  mov        dword ptr [ecx + 4], eax
004695b2  mov        edx, dword ptr [esi + 4]
004695b5  mov        eax, esi
004695b7  fstp       dword ptr [edx]
004695b9  pop        esi
004695ba  ret        4
