
// block 004d9967
004d9967  scasb      al, byte ptr es:[edi]
004d9968  sbb        eax, 0x97f2825a
004d996d  xor        al, bl
004d996f  or         ebx, dword ptr [ebp - 0x10]
004d9972  loope      0x4d997b

// block 004d9974
004d9974  scasd      eax, dword ptr es:[edi]
004d9975  js         0x4d99f1
