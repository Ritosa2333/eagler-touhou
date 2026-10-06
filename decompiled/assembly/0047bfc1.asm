
// block 0047bfc1
0047bfc1  mov        edi, edi
0047bfc3  push       ebp
0047bfc4  mov        ebp, esp
0047bfc6  mov        eax, dword ptr [ebp + 8]
0047bfc9  cmp        eax, -2
0047bfcc  jne        0x47bfdd

// block 0047bfce
0047bfce  call       0x46e96f

// block 0047bfd3
0047bfd3  mov        dword ptr [eax], 9
0047bfd9  xor        eax, eax
0047bfdb  pop        ebp
0047bfdc  ret        

// block 0047bfdd
0047bfdd  push       esi
0047bfde  xor        esi, esi
0047bfe0  cmp        eax, esi
0047bfe2  jl         0x47bfec

// block 0047bfe4
0047bfe4  cmp        eax, dword ptr [0x4d6308]
0047bfea  jb         0x47c008

// block 0047bfec
0047bfec  call       0x46e96f

// block 0047bff1
0047bff1  push       esi
0047bff2  push       esi
0047bff3  push       esi
0047bff4  push       esi
0047bff5  push       esi
0047bff6  mov        dword ptr [eax], 9
0047bffc  call       0x470f2d

// block 0047c001
0047c001  add        esp, 0x14
0047c004  xor        eax, eax
0047c006  jmp        0x47c022

// block 0047c008
0047c008  mov        ecx, eax
0047c00a  and        eax, 0x1f
0047c00d  sar        ecx, 5
0047c010  mov        ecx, dword ptr [ecx*4 + 0x4d6320]
0047c017  shl        eax, 6
0047c01a  movsx      eax, byte ptr [ecx + eax + 4]
0047c01f  and        eax, 0x40

// block 0047c022
0047c022  pop        esi
0047c023  pop        ebp
0047c024  ret        
