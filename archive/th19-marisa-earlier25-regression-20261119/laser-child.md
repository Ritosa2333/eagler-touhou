# Straight laser split controller

Seed 1000003 / 999983 diverged at cumulative frame 35570. Web blocked-mask 168 rejected the saved CPU direction 7; native continued with input 97. Both lists contained 114 obstacles.

The differing obstacle was a straight-laser tail created by a rectangular clear. At the comparison visit its Web rectangle center was (-179.941879, 434.231506), while native was (-183.821823, 438.886566). The difference equals one movement step at speed 6.06 and angle 2.26562691.

The child retained the contact-registration delay and also marked its movement as already completed. Native delays contact registration but visits the new controller in the late laser-manager pass. Removing that movement suppression aligns the tail without admitting early contacts.

Candidate WASM: `97f356f08f888d93d1982c346ebb40e6972595338c2858d704efaaba3ca30dde`. All 36000 cumulative frames passed strict gameplay validation: primary RNG, CPU controls, player positions, bullet counts and coordinates. There were 35999 comparable bullet frames; frame 27132 is a verified native round-transition gap. Maximum coordinate difference was 0.00006103515625 pixels.

This build also includes a provisional wolf registration-phase change. The laser correction is independently supported by the native obstacle trace. Whole-build acceptance still requires the wolf round-transition regressions and the complete seed regression.
