
// block 004774a0
004774a0  push       edi
004774a1  push       esi
004774a2  push       ebx
004774a3  xor        edi, edi
004774a5  mov        eax, dword ptr [esp + 0x14]
004774a9  or         eax, eax
004774ab  jge        0x4774c1

// block 004774ad
004774ad  inc        edi
004774ae  mov        edx, dword ptr [esp + 0x10]
004774b2  neg        eax
004774b4  neg        edx
004774b6  sbb        eax, 0
004774b9  mov        dword ptr [esp + 0x14], eax
004774bd  mov        dword ptr [esp + 0x10], edx

// block 004774c1
004774c1  mov        eax, dword ptr [esp + 0x1c]
004774c5  or         eax, eax
004774c7  jge        0x4774dd

// block 004774c9
004774c9  inc        edi
004774ca  mov        edx, dword ptr [esp + 0x18]
004774ce  neg        eax
004774d0  neg        edx
004774d2  sbb        eax, 0
004774d5  mov        dword ptr [esp + 0x1c], eax
004774d9  mov        dword ptr [esp + 0x18], edx

// block 004774dd
004774dd  or         eax, eax
004774df  jne        0x4774f9

// block 004774e1
004774e1  mov        ecx, dword ptr [esp + 0x18]
004774e5  mov        eax, dword ptr [esp + 0x14]
004774e9  xor        edx, edx
004774eb  div        ecx
004774ed  mov        ebx, eax
004774ef  mov        eax, dword ptr [esp + 0x10]
004774f3  div        ecx
004774f5  mov        edx, ebx
004774f7  jmp        0x47753a

// block 004774f9
004774f9  mov        ebx, eax
004774fb  mov        ecx, dword ptr [esp + 0x18]
004774ff  mov        edx, dword ptr [esp + 0x14]
00477503  mov        eax, dword ptr [esp + 0x10]

// block 00477507
00477507  shr        ebx, 1
00477509  rcr        ecx, 1
0047750b  shr        edx, 1
0047750d  rcr        eax, 1
0047750f  or         ebx, ebx
00477511  jne        0x477507

// block 00477513
00477513  div        ecx
00477515  mov        esi, eax
00477517  mul        dword ptr [esp + 0x1c]
0047751b  mov        ecx, eax
0047751d  mov        eax, dword ptr [esp + 0x18]
00477521  mul        esi
00477523  add        edx, ecx
00477525  jb         0x477535

// block 00477527
00477527  cmp        edx, dword ptr [esp + 0x14]
0047752b  ja         0x477535

// block 0047752d
0047752d  jb         0x477536

// block 0047752f
0047752f  cmp        eax, dword ptr [esp + 0x10]
00477533  jbe        0x477536

// block 00477535
00477535  dec        esi

// block 00477536
00477536  xor        edx, edx
00477538  mov        eax, esi

// block 0047753a
0047753a  dec        edi
0047753b  jne        0x477544

// block 0047753d
0047753d  neg        edx
0047753f  neg        eax
00477541  sbb        edx, 0

// block 00477544
00477544  pop        ebx
00477545  pop        esi
00477546  pop        edi
00477547  ret        0x10
