
// block 00462a20
00462a20  test       ax, ax
00462a23  jge        0x462a28

// block 00462a25
00462a25  xor        eax, eax
00462a27  ret        

// block 00462a28
00462a28  cwde       
00462a29  add        eax, edx
00462a2b  mov        dl, byte ptr [eax]
00462a2d  and        dl, 0x80
00462a30  movzx      edx, dl
00462a33  neg        edx
00462a35  sbb        edx, edx
00462a37  and        edx, ecx
00462a39  or         dword ptr [esi], edx
00462a3b  mov        al, byte ptr [eax]
00462a3d  and        al, 0x80
00462a3f  movzx      eax, al
00462a42  neg        eax
00462a44  sbb        eax, eax
00462a46  and        eax, ecx
00462a48  ret        
