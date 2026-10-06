
// block 0043cf90
0043cf90  push       ecx
0043cf91  push       esi
0043cf92  push       edi
0043cf93  push       0x1edfc
0043cf98  push       0
0043cf9a  push       ebx
0043cf9b  call       0x477420

// block 0043cfa0
0043cfa0  add        esp, 0xc
0043cfa3  push       1
0043cfa5  lea        eax, [esp + 0xc]
0043cfa9  push       eax
0043cfaa  mov        eax, 0x4a14b0
0043cfaf  call       0x463c10

// block 0043cfb4
0043cfb4  mov        dword ptr [ebx], eax
0043cfb6  lea        eax, [ebx + 0x1e9b4]
0043cfbc  call       0x43ceb0

// block 0043cfc1
0043cfc1  lea        esi, [ebx + 8]
0043cfc4  mov        ecx, esi
0043cfc6  sub        ecx, ebx
0043cfc8  lea        edi, [ecx - 8]
0043cfcb  mov        eax, 0xea36c79
0043cfd0  imul       edi
0043cfd2  sar        edx, 0xa
0043cfd5  mov        eax, edx
0043cfd7  shr        eax, 0x1f
0043cfda  add        eax, edx
0043cfdc  cmp        eax, 7
0043cfdf  jae        0x43d008

// block 0043cfe1
0043cfe1  call       0x43ce00

// block 0043cfe6
0043cfe6  add        edi, 0x45f4
0043cfec  mov        eax, 0xea36c79
0043cff1  imul       edi
0043cff3  sar        edx, 0xa
0043cff6  mov        ecx, edx
0043cff8  shr        ecx, 0x1f
0043cffb  add        ecx, edx
0043cffd  add        esi, 0x45f4
0043d003  cmp        ecx, 7
0043d006  jb         0x43cfe1

// block 0043d008
0043d008  push       ebx
0043d009  call       0x43d140

// block 0043d00e
0043d00e  pop        edi
0043d00f  mov        eax, ebx
0043d011  pop        esi
0043d012  pop        ecx
0043d013  ret        
