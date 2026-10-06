
// block 00421880
00421880  mov        ecx, dword ptr [eax + 0x38]
00421883  mov        eax, 0x51eb851f
00421888  imul       ecx
0042188a  sar        edx, 5
0042188d  mov        ecx, edx
0042188f  shr        ecx, 0x1f
00421892  add        ecx, edx
00421894  mov        eax, ecx
00421896  cdq        
00421897  push       esi
00421898  mov        esi, 0xa
0042189d  idiv       esi
0042189f  mov        eax, ecx
004218a1  pop        esi
004218a2  sub        eax, edx
004218a4  ret        
