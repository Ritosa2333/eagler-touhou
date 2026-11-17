# TH19 projectile impacts

Original EXE SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

The hit table at 0x5a03d0 selects callbacks 1=0x54da60, 2=0x54cd70, 3=0x54c5f0, 4=0x54c2f0, 5=0x54bf50, 6=0x54bd90, 7=0x54b370, 8=0x54c7c0, 9=0x54a7b0. The null/default handler is 0x54fba0; callback7 calls it directly.

Default impact interrupts the existing character ANM at label1, divides speed by8 (float at0x5aa2f0), enters state2 and unregisters damage. Persistent shots skip this transition. Homing update0x54dc40 and charge update0x54b380 skip their motion update in state2.

Callbacks1/5/6 additionally create effect.anm142. Callback1 reverses the perturbed angle and applies native color/alpha ranges; callback6 applies its color ranges and1-to3 scale over20 frames. The angle range is20 degrees (constants0x5aa3d4=180 and0x5aa314=20). Reimu focused-shot groups6..9 use callback1; her charge group10 uses callback7 and retains its persistent talisman behavior. The persistence flag suppresses the default retirement transition, not callback dispatch.

The runtime retains original character-specific ANM impact segments, waits for descendant animation lifetimes, and invokes extra effects independently of persistent projectile damage. Laser, frog, flame and trap collision behavior remains in its dedicated branch. Both packages consume the shared TH19 runtime.

Verification uses actual SHT emission and collision against a durable test enemy, records impact state/damage retirement for19 characters, and captures ordinary and charged attack frames. These are EXE/asset-based checks and browser screenshots, not synchronized retail video comparison. The existing deterministic web random stream is retained; exact retail random sequence parity is not established.
