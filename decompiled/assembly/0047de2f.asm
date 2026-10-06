
// block 0047de2f
0047de2f  mov        edi, edi
0047de31  push       ebp
0047de32  mov        ebp, esp
0047de34  mov        eax, dword ptr [ebp + 8]
0047de37  cmp        eax, dword ptr [ebp + 0xc]
0047de3a  jae        0x47de42

// block 0047de3c
0047de3c  mov        cl, byte ptr [ecx + 4]
0047de3f  mov        byte ptr [eax], cl
0047de41  inc        eax

// block 0047de42
0047de42  pop        ebp
0047de43  ret        8
