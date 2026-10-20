
// block 0047dfd9
0047dfd9  mov        edi, edi
0047dfdb  push       ebp
0047dfdc  mov        ebp, esp
0047dfde  push       esi
0047dfdf  push       dword ptr [ebp + 0xc]
0047dfe2  mov        esi, ecx
0047dfe4  mov        ecx, dword ptr [esi + 4]
0047dfe7  push       dword ptr [ebp + 8]
0047dfea  mov        eax, dword ptr [ecx]
0047dfec  call       dword ptr [eax + 8]

// block 0047dfef
0047dfef  cmp        eax, dword ptr [ebp + 0xc]
0047dff2  jae        0x47e000

// block 0047dff4
0047dff4  push       dword ptr [ebp + 0xc]
0047dff7  mov        ecx, dword ptr [esi + 8]
0047dffa  mov        edx, dword ptr [ecx]
0047dffc  push       eax
0047dffd  call       dword ptr [edx + 8]

// block 0047e000
0047e000  pop        esi
0047e001  pop        ebp
0047e002  ret        8
