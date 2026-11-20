# Player-contact and bomb split registration

Seed 4599481 / 966889228 first diverged in primary RNG at frame 26391 after moving every rectangular-cut child on the late manager pass. CPU inputs, player coordinates and bullet states still matched at that point.

Native laser 374137784 entered its first graze visit at position (143.672379, 454.564545), length32, angle1.3055048. Its graze clock advanced from0 to1 and consumed the draw group starting at primary seed1384973483. The overgeneralized Web movement put the player-contact child one step farther away and omitted that graze.

Player-contact callbacks identify a single laser with a nonzero laser_id. They run after its manager visit, so the newly linked child starts movement on the next visit. Bomb rectangle clears pass laser_id zero; their children remain eligible for the same tick's late manager pass. Contact registration remains independently guarded.

The corrected movement condition is beforeWorldTick && laser_id. It preserves the native player-contact delay while retaining the bomb-child movement needed by seed1000003.

Candidate ed2ddabb8acaad7fe2a6f84a70d3539b899b53136c38f20b148120d2b1fc63d2 passed all five critical seeds through36000 cumulative frames: 4599481,1000003,424242,65537,536870911. Complete same-build regression is recorded separately in the final aggregate result.

Paired frame26391 images: [before](screenshots/seed4599481-before/frame-26391.png), [after](screenshots/seed4599481-after/frame-26391.png).
