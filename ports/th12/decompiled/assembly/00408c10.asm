
// block 00408c10
00408c10  fldz       
00408c12  xor        ecx, ecx
00408c14  fcomp      dword ptr [eax + 4]
00408c17  fnstsw     ax
00408c19  test       ah, 0x41
00408c1c  lea        eax, [ecx + 0xe]
00408c1f  jnp        0x408c23

// block 00408c21
00408c21  mov        eax, ecx

// block 00408c23
00408c23  ret        
