
// block 004767d0
004767d0  push       ebx
004767d1  push       esi
004767d2  mov        eax, dword ptr [esp + 0x18]
004767d6  or         eax, eax
004767d8  jne        0x4767f2

// block 004767da
004767da  mov        ecx, dword ptr [esp + 0x14]
004767de  mov        eax, dword ptr [esp + 0x10]
004767e2  xor        edx, edx
004767e4  div        ecx
004767e6  mov        ebx, eax
004767e8  mov        eax, dword ptr [esp + 0xc]
004767ec  div        ecx
004767ee  mov        edx, ebx
004767f0  jmp        0x476833

// block 004767f2
004767f2  mov        ecx, eax
004767f4  mov        ebx, dword ptr [esp + 0x14]
004767f8  mov        edx, dword ptr [esp + 0x10]
004767fc  mov        eax, dword ptr [esp + 0xc]

// block 00476800
00476800  shr        ecx, 1
00476802  rcr        ebx, 1
00476804  shr        edx, 1
00476806  rcr        eax, 1
00476808  or         ecx, ecx
0047680a  jne        0x476800

// block 0047680c
0047680c  div        ebx
0047680e  mov        esi, eax
00476810  mul        dword ptr [esp + 0x18]
00476814  mov        ecx, eax
00476816  mov        eax, dword ptr [esp + 0x14]
0047681a  mul        esi
0047681c  add        edx, ecx
0047681e  jb         0x47682e

// block 00476820
00476820  cmp        edx, dword ptr [esp + 0x10]
00476824  ja         0x47682e

// block 00476826
00476826  jb         0x47682f

// block 00476828
00476828  cmp        eax, dword ptr [esp + 0xc]
0047682c  jbe        0x47682f

// block 0047682e
0047682e  dec        esi

// block 0047682f
0047682f  xor        edx, edx
00476831  mov        eax, esi

// block 00476833
00476833  pop        esi
00476834  pop        ebx
00476835  ret        0x10
