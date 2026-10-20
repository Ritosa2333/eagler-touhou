
// block 00477550
00477550  push       ebx
00477551  push       edi
00477552  xor        edi, edi
00477554  mov        eax, dword ptr [esp + 0x10]
00477558  or         eax, eax
0047755a  jge        0x477570

// block 0047755c
0047755c  inc        edi
0047755d  mov        edx, dword ptr [esp + 0xc]
00477561  neg        eax
00477563  neg        edx
00477565  sbb        eax, 0
00477568  mov        dword ptr [esp + 0x10], eax
0047756c  mov        dword ptr [esp + 0xc], edx

// block 00477570
00477570  mov        eax, dword ptr [esp + 0x18]
00477574  or         eax, eax
00477576  jge        0x47758b

// block 00477578
00477578  mov        edx, dword ptr [esp + 0x14]
0047757c  neg        eax
0047757e  neg        edx
00477580  sbb        eax, 0
00477583  mov        dword ptr [esp + 0x18], eax
00477587  mov        dword ptr [esp + 0x14], edx

// block 0047758b
0047758b  or         eax, eax
0047758d  jne        0x4775aa

// block 0047758f
0047758f  mov        ecx, dword ptr [esp + 0x14]
00477593  mov        eax, dword ptr [esp + 0x10]
00477597  xor        edx, edx
00477599  div        ecx
0047759b  mov        eax, dword ptr [esp + 0xc]
0047759f  div        ecx
004775a1  mov        eax, edx
004775a3  xor        edx, edx
004775a5  dec        edi
004775a6  jns        0x4775f6

// block 004775a8
004775a8  jmp        0x4775fd

// block 004775aa
004775aa  mov        ebx, eax
004775ac  mov        ecx, dword ptr [esp + 0x14]
004775b0  mov        edx, dword ptr [esp + 0x10]
004775b4  mov        eax, dword ptr [esp + 0xc]

// block 004775b8
004775b8  shr        ebx, 1
004775ba  rcr        ecx, 1
004775bc  shr        edx, 1
004775be  rcr        eax, 1
004775c0  or         ebx, ebx
004775c2  jne        0x4775b8

// block 004775c4
004775c4  div        ecx
004775c6  mov        ecx, eax
004775c8  mul        dword ptr [esp + 0x18]
004775cc  xchg       ecx, eax
004775cd  mul        dword ptr [esp + 0x14]
004775d1  add        edx, ecx
004775d3  jb         0x4775e3

// block 004775d5
004775d5  cmp        edx, dword ptr [esp + 0x10]
004775d9  ja         0x4775e3

// block 004775db
004775db  jb         0x4775eb

// block 004775dd
004775dd  cmp        eax, dword ptr [esp + 0xc]
004775e1  jbe        0x4775eb

// block 004775e3
004775e3  sub        eax, dword ptr [esp + 0x14]
004775e7  sbb        edx, dword ptr [esp + 0x18]

// block 004775eb
004775eb  sub        eax, dword ptr [esp + 0xc]
004775ef  sbb        edx, dword ptr [esp + 0x10]
004775f3  dec        edi
004775f4  jns        0x4775fd

// block 004775f6
004775f6  neg        edx
004775f8  neg        eax
004775fa  sbb        edx, 0

// block 004775fd
004775fd  pop        edi
004775fe  pop        ebx
004775ff  ret        0x10
