# Composed player-contact diagnostic host

This composes the frozen421-case PlayerHitRules and607-case PlayerDeathRules with
actual ANM hosts. It is not full original player initialization, shooting, focus,
freeze, death/respawn or native-frame acceptance. The public context is explicit:
state/flags/protection timer, control/system fields, initial raw position and
perfect-freeze charge are caller inputs, never inferred from a synthetic zero.

The browser currently supplies state1/flags0/protection120, initial raw(0,51200),
charge0 and explicit zero-valued diagnostic controls. The120 value is a proved
native constructor write, not proof of every constructor-to-first-game-frame
handoff. Normal-init state1 clears the low3 flags but does not reset protection.
The runtime labels this limitation and stops required unsupported operations.

## Before/after the contact

PlayerHitHost first evaluates native player/control suppression gates. A positive
protection timer yields the native hit-without-hit-call result; suppressed states
yield0. A required first-hit call writes state4 **before** its mandatory effects:
script83 then32 script84 objects from the original bullet ANM. All33 actual
factory operations must succeed before timer suffixes. State timer resets0,
conditional sound2 is retained as a request, protection becomes6, then actual
pl00.anm0 is rebound and the native tail state is applied. Any failure is sticky;
earlier prefix writes remain but later writes/false success do not occur.

BulletUpdateHost then follows the hit caller: state3, primary IRQ1 queued (not
immediately ticked), mandatory original type/color cancellation script357, native
drift request for the full-speed collision site, then the independent known tail
and exactly one embedded ANM tick. State3 thereafter moves half-speed and does not
repeat collision. The84 original tick prefixes,17 no-effect resumes and6 independent
after-hit suffixes distinguish actual helper execution from caller-authorized
continuation. No native callback return is patched or invented in the probe.

Graze, hit83/84, cancellation357 and child356 objects now share **one** primary
tail Scene and shared game RNG. They are not grouped by effect kind into separate
tick services, which would change random-call chronology. Enemy roots register at
the head of the same logical primary domain; the supported host ticks them before
tail effects at native primary priority30. Other external families and complete
native startup/activation are not established by this partial scheduler.

## Window timing and stops

With the known unit clock and initialized integral timer, state4 first evaluates
the native8-count deathbomb window, then protection decrement, actual player ANM,
and the ordinary state-timer increment. At timer8 the next step stops **before**
DeathCommit0x43cdd0; it does not advance to9 or silently respawn. Perfect-freeze
deathbomb and held-freeze branches are explicit required-system stops.

NativePlayerWindow executes original43bea1..43bf75 over four state/focus timer
pairs including65535, the actual player constructor clear0x43a94 bytes through
the original scalar memset, and the actual null-handle detach/lookup return.
These are bounded independent original-byte prefixes, not a complete constructor
or Windows game frame. The last valid ANM remains readable at logic-only stops;
an ANM fault instead hides invalid presentation state.

The new reusable PlayerDeathHost composes explicit motivation/charge rules,
mandatory charge/control/UI/ANM call requests and eight supplied cleanup records.
Opaque live cleanup handles reject; suffixes are not permission to assume zero
handles. It is not yet installed as a successful ordinary death manager. At the
live host's current boundary, explicit state4/window8 stops without skipping it.

## Ordinary movement

Normal-state raw movement uses the independent677-case PlayerRules at128 units
per pixel, real speed4.5px/axis tick and canonical bounds. Horizontal sign changes
request original pl00.anm scripts1/2/3/4, followed by the normal explicit ANM tick.
The native43b1b7..43b230 prefix is separately executed for seven transition cases,
stopping at the real bind when one is required. No age-based sprite substitution
is used. Keyboard blur/visibility clears directions to avoid stuck movement.

This initial browser binding accepts only ordinary directional inputs. Focus,
shoot and freeze paths require additional native factories/managers and are
explicitly reported as not integrated rather than accepted as no-ops. Movement
helpers may be tested with explicit FocusContext, but the browser does not install
an invented always-active focus manager.

## Verification and error ownership

The contact host has UBSan tests for all33 effects/order, conditional sound,
timers/ANM, suppression, partial callback rejection/throws/reentry and sticky
retry behavior. The bullet caller includes HALF/FULL zero/null/invalid/one results,
missing/failing cancellation, both continuation classes and no-IRQ animation
baselines. The malformed camera-input mutant first passed with implicit zero
context, then correctly rejected when absent camera state became explicit.

World presentation uses9-digit float32 round-trip JSON, validates composed fields,
and returns a valid empty draw list plus an observable error for nonfinite input.
No invalid value is silently clamped or replaced. Runtime source snapshots are
captured before and after compilation; changed source rejects publication and
preserves the last good immutable JS/WASM pair. These engineering checks are not
substitutes for native gameplay comparisons or completion of the full port.
