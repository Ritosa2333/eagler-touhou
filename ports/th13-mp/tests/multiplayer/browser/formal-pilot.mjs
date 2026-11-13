// Test pilot: observes diagnostics, changes only the same hosted keys as the
// production keyboard bridge. It never changes game state, rate or resources.
export function installPilot({ seat }) {
  const w = document.querySelector("#gameFrame").contentWindow,
    m = w.Module;
  const json = (f) => {
    const p = f();
    return p ? JSON.parse(m.UTF8ToString(p) || "{}") : {};
  };
  const keys = [
    [1, 44],
    [2, 45],
    [8, 42],
    [16, 200],
    [32, 208],
    [64, 203],
    [128, 205],
    [512, 29],
    [2048, 46],
    [4096, 28],
    [256, 1],
  ];
  const state = (w.__pilot = {
    events: [],
    stage: 0,
    lastFrame: -1,
    mask: 0,
    decisions: 0,
    movementDecisions: 0,
  });
  // Keep the SHT movement estimate inside installPilot
  // so Playwright page.evaluate's function serialization has no free imports.
  function createMovementModel(m, w) {
    const identities = [
      '41efe8effa3b0a591ed8bf3db646c2a8d3e8d37d35cd744d1687778068a93769',
      '083c815505f09bfc5cd26daa3caaa35a568d3d2f67f182aa076c08c48597fed3',
      'b851026d26d3522784025c863037cb66bb19bb031cbbbe9e720c07c835c0dc6e',
      '146b690e7b98850091e39778b3fb7481fb87453b8f0a92fb5491a2d3c202f7b7',
    ];
    const cache = new Map();
    function request(character) {
      if (!Number.isInteger(character) || character < 0 || character >= identities.length)
        return { status:'fallback', reason:'invalid-character' };
      if (cache.has(character)) return cache.get(character);
      const record = { status:'fallback', reason:'sht-loading', character };
      cache.set(character, record);
      try {
        const file = `/assets/pl0${character}a.sht`;
        const bytes = new Uint8Array(m.FS.readFile(file)); // immutable snapshot
        if (bytes.byteLength < 32) throw Error('sht-short');
        const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
        const speeds128 = Array.from({length:4}, (_, i) => {
          const v = view.getFloat32(16 + 4 * i, true);
          if (!Number.isFinite(v) || v <= 0 || v * 128 > 2147483647) throw Error('sht-speed-invalid');
          return Math.trunc(v * 128);
        });
        if (speeds128.some(x => x <= 0)) throw Error('sht-speed-invalid');
        if (!w.crypto?.subtle) throw Error('sha256-unavailable');
        w.crypto.subtle.digest('SHA-256', bytes).then(digest => {
          const sha = Array.from(new Uint8Array(digest), b => b.toString(16).padStart(2,'0')).join('');
          if (sha !== identities[character]) Object.assign(record, {reason:'sht-role-sha-mismatch', sha});
          else Object.assign(record, {status:'ready', reason:null, file, sha, speeds128});
        }).catch(() => { record.reason = 'sha256-failed'; });
      } catch (error) {
        record.reason = `sht-read:${error.message}`;
        // An asset may not yet be mounted when the pilot installs.
        // Retry on the next observation instead of caching an early read failure.
        cache.delete(character);
      }
      return record;
    }
    function path({character, s, hazards, team, shift, dx, dy, horizons}) {
      const record = request(character);
      if (record.status !== 'ready') return {mode:'fallback', reason:record.reason};
      // Browser x/y are printed to six decimals. Reconstruct the 1/128 grid;
      // refuse inconsistent or missing observations. phase_frame and the list of
      // live hazards are proxies, not the production startup/enemy gates.
      if (!s || s.character !== character || !Number.isFinite(s.x) || !Number.isFinite(s.y) ||
          s.phase !== 1 || s.ghost || s.dead || !Number.isInteger(s.phase_frame) || s.phase_frame < 0 ||
          !Array.isArray(hazards?.enemies) || !team || ![0,1,false,true].includes(team.active) || team.active ||
          ![true,false].includes(shift) ||
          ![-1,0,1].includes(dx) || ![-1,0,1].includes(dy) ||
          !Array.isArray(horizons) || horizons.some(t => !Number.isInteger(t) || t < 0))
        return {mode:'fallback',reason:'known-special-or-invalid-observation',sha:record.sha};
      const fixedX = Math.round(s.x * 128), fixedY = Math.round(s.y * 128);
      if (fixedX < -23552 || fixedX > 23552 || fixedY < 4096 || fixedY > 55296 ||
          Math.abs(s.x - fixedX / 128) > 0.00000051 || Math.abs(s.y - fixedY / 128) > 0.00000051)
        return {mode:'fallback',reason:'position-not-on-fixed-grid',sha:record.sha};
      const focus = shift && s.phase_frame >= 4 && hazards.enemies.length > 0;
      const speed = record.speeds128[(dx && dy ? 2 : 0) + (focus ? 1 : 0)];
      const clamp = (v, a, b) => Math.max(a, Math.min(b, v));
      const points = horizons.map(t => [clamp(fixedX + dx * speed * t, -23552, 23552) / 128,
                                        clamp(fixedY + dy * speed * t, 4096, 55296) / 128]);
      return {mode:'normal-sht-estimate', sha:record.sha, points, focus,
        unknown:['active-bomb','exact-time-rate','touch','future-gates-and-special-states']};
    }
    return {request, path};
  }

  const movementModel = createMovementModel(m, w);
  let bombAt = -1000, rescue = null, rescueCooldown = 0;
  let move = 0, nextMoveCapture = -1;
  const emit = (event) => {
    state.events.push(event);
    if (state.events.length > 256) state.events.shift();
  };
  const set = (mask, dialogue = true) => {
    // A dialogue opened while Z/Ctrl were already held needs a new press to
    // arm skip. Supply ordinary release/press edges without changing its state.
    if (dialogue && state.lastFrame % 60 < 10) mask &= ~(1 | 512);
    for (const [bit, scan] of keys)
      if ((mask ^ state.mask) & bit) m._th13_key(scan, +!!(mask & bit));
    state.mask = mask;
  };
  const old = m.onGameFrame;
  m.onGameFrame = (ok, ms, ticks) => {
    old?.(ok, ms, ticks);
    if (!ok) return;
    // Receive work can expose a historical correction boundary. Keep the
    // current hosted keys until replay finishes; pending fresh capture alone
    // does not make a forward world historical.
    const network = json(m._th13_debug_mp_net_state);
    const correction = m._th13_mp_rollback_status() >>> 2;
    if (network.simulating && m.HEAPU32[correction + 7]) return;
    const clock = m._th13_frame();
    if (network.simulating ? clock <= state.lastFrame : clock === state.lastFrame) return;
    const pointer = m._th13_probe_state() >>> 2,
      probe = Array.from(m.HEAP32.subarray(pointer, pointer + 16));
    const stage = probe[2],
      frame = probe[5];
    if (stage !== state.stage) {
      emit({ type: "stage", stage, frame, time: performance.now() });
      state.stage = stage;
      bombAt = -1000;
      move = 0; nextMoveCapture = -1;
      rescue = null; rescueCooldown = 0; state.rescue = null;
    }
    state.lastFrame = clock;
    const pulse = clock % 30 < 5;
    const saved = (() => {
      try {
        return m.FS.readdir("/save/replay").some((x) =>
          /^th13mp_\d{2}\.rpy$/.test(x),
        );
      } catch {
        return false;
      }
    })();
    if (probe[0] !== 1) {
      const title = json(m._th13_debug_pending);
      const exitSavedSlot = title.has_title && title.title_screen === 15 &&
        title.title_substate === 2 && saved;
      set(probe[0] === 0 && probe[8] === 1 ? 0 : pulse ? (exitSavedSlot ? 2 : 1) : 0);
      return;
    }
    const pause = json(m._th13_debug_pause_state);
    if (pause.screen) {
      state.pause = pause;
      let mask = 1;
      if (pause.screen === 4 || pause.screen === 2 || pause.screen === 5) {
        if (pause.phase === 6)
          mask =
            pause.cursor === (saved ? 0 : 1)
              ? 1
              : pause.cursor < (saved ? 0 : 1)
                ? 32
                : 16;
        if (pause.phase === 17 && saved) mask = 256;
      }
      set(pulse ? mask : 0);
      return;
    }
    const s = json(() => m._th13_debug_mp_seat_state(seat));
    const hazards = json(m._th13_debug_mp_hazards),
      team = json(m._th13_debug_mp_trance_state);
    const dialogueMask = hazards.dialogue ? 512 : 0;
    if (!Number.isFinite(s.x)) {
      set(1 | dialogueMask, Boolean(hazards.dialogue));
      return;
    }
    if (state.resources !== `${s.extra_lives}/${s.bombs}/${s.phase}`) {
      state.resources = `${s.extra_lives}/${s.bombs}/${s.phase}`;
      emit({ type: "resources", stage, frame, seat: s, team });
    }
    // Election is based only on sampled seats. The elected donor keeps its
    // own attempt until the recipient recovers or the donor becomes ineligible.
    const seats = Array.from({ length: s.seat_count }, (_, index) =>
      index === seat ? s : json(() => m._th13_debug_mp_seat_state(index)));
    const eligibleDonor = player => !player.ghost && !player.dead &&
      player.phase === 1 && player.extra_lives > 0;
    if (rescue && !seats[rescue.recipient]?.ghost) {
      emit({ type: "rescue-complete", stage, frame, donor: seat,
        recipient: rescue.recipient, recipientState: seats[rescue.recipient] });
      rescue = null;
    }
    if (rescue && !eligibleDonor(s)) {
      emit({ type: "rescue-abort", stage, frame, reason: "donor-unavailable" });
      rescue = null;
    }
    if (!rescue && frame >= rescueCooldown && eligibleDonor(s)) {
      const ghosts = seats.filter(player => player.ghost)
        .sort((a, b) => a.extra_lives - b.extra_lives || a.seat - b.seat);
      const donor = seats.find(eligibleDonor);
      if (ghosts.length && donor?.seat === seat) {
        rescue = { recipient: ghosts[0].seat, phase: "approach", presses: 0,
          down: false, changedAt: -1, attempts: 1 };
        emit({ type: "rescue-start", stage, frame, donor: seat,
          recipient: rescue.recipient, donorState: s, recipientState: ghosts[0] });
      }
    }
    let targetX = 0,
      targetY = 392;
    const foes = hazards.enemies.filter(
      (x) => !x.dead && x.y > 0 && x.y < 300,
    );
    const bodies = hazards.enemies.filter(enemy => !enemy.dead &&
      !(enemy.flags & (0x22 | 0x8000000)) && enemy.body_invulnerable <= 0);
    const boss = foes.find(enemy => enemy.serial === hazards.boss_main_serial);
    if (foes.length)
      targetX = Math.max(
        -145,
        Math.min(
          145,
          (boss || foes.sort((a, b) => b.life - a.life)[0]).x,
        ),
      );
    const hitSize = [2, 3, 3, 3][s.character];
    const bullets = hazards.bullets
      .filter(b => b.alive && b.collision_ready && (b.flags & 2) &&
        Math.abs(b.x - s.x) < 170 && b.y > s.y - 180 && b.y < s.y + 70)
      .map(b => {
        const scale = b.flags & 0x40 ? b.scale : 1,
          circle = !!(b.flags & 0x10),
          rx = circle ? Math.hypot(hitSize, b.width * scale) :
            (hitSize + b.width * scale) * 0.5,
          ry = circle ? rx : (hitSize + b.height * scale) * 0.5;
        return { ...b, circle, rx, ry };
      });
    let nearestResource = Infinity;
    const resource = (item, safe) => {
      const {x, y} = item,
        distance = Math.hypot(x - s.x, y - s.y);
      if (safe && y > 40 && y < 430 && distance < nearestResource) {
        nearestResource = distance;
        targetX = x; targetY = y;
      }
    };
    for (const item of hazards.items) {
      const distance = Math.hypot(item.x - s.x, item.y - s.y);
      const useful = [1, 3, 4, 5, 6, 7, 8].includes(item.kind) &&
        !(s.power >= 400 && [1, 3, 8].includes(item.kind));
      resource(item, useful && (s.invulnerable > 60 ||
        (distance < 170 && (bullets.length < 12 || distance < 75))));
    }
    for (const spirit of hazards.spirits) {
      const distance = Math.hypot(spirit.x - s.x, spirit.y - s.y);
      resource(spirit, s.invulnerable > 60 || team.active ||
        (distance < 150 && (bullets.length < 12 || distance < 65)));
    }
    const capture = network.capture_frame;
    if (rescue) {
      const recipient = seats[rescue.recipient];
      targetX = recipient.x; targetY = recipient.y;
      const distance = Math.hypot(s.x - targetX, s.y - targetY);
      if (rescue.phase === "approach" && distance <= 12) {
        // First release the ordinary held focus/fire keys and let that release
        // pass through a real device sample before beginning the five presses.
        rescue.phase = "tap"; rescue.down = false; rescue.changedAt = capture;
      } else if (rescue.phase !== "approach" && distance > 18) {
        rescue.phase = "approach"; rescue.presses = 0;
      }
      if (rescue.phase === "tap" && capture > rescue.changedAt) {
        if (rescue.down) {
          rescue.down = false;
          if (rescue.presses === 5) rescue.phase = "wait";
        } else {
          rescue.down = true; ++rescue.presses;
          emit({ type: "rescue-press", stage, frame, capture,
            donor: seat, recipient: rescue.recipient, press: rescue.presses });
        }
        rescue.changedAt = capture;
      }
      if (rescue.phase === "wait" && capture - rescue.changedAt > 48) {
        if (rescue.attempts < 3) {
          rescue.phase = "approach"; rescue.presses = 0; ++rescue.attempts;
        } else {
          emit({ type: "rescue-abort", stage, frame, reason: "gesture-not-accepted" });
          rescue = null; rescueCooldown = frame + 120;
        }
      }
    }
    state.rescue = rescue;
    // Decide from each fresh visible world through ordinary hosted input.
    // Corrections above never resample the controller from historical state.
    const chooseMovement = capture >= nextMoveCapture;
    const retainedSpeed = move & 8 ? 2 : 4.5,
      retainedX = move & 64 ? -1 : move & 128 ? 1 : 0,
      retainedY = move & 16 ? -1 : move & 32 ? 1 : 0;
    let best = null;
    const horizons = [1, 3, 6, 10, 15, 20];
    const predicted = bullets.map(b => horizons.map(t =>
      [b.x + b.vx * t, b.y + b.vy * t]));
    for (const speed of [2, 4.5])
      for (let dx = -1; dx <= 1; dx++)
        for (let dy = -1; dy <= 1; dy++) {
          if (!chooseMovement && (speed !== retainedSpeed ||
              dx !== retainedX || dy !== retainedY)) continue;
          const scale = dx && dy ? Math.SQRT1_2 : 1,
            vx = dx * speed * scale,
            vy = dy * speed * scale;
          const prediction = movementModel.path({character:s.character, s, hazards, team,
            shift:speed === 2, dx, dy, horizons:[...horizons, 8]});
          const fallbackPath = t => [Math.max(-184, Math.min(184, s.x + vx * t)),
            Math.max(32, Math.min(432, s.y + vy * t))];
          const path = prediction.points ? prediction.points.slice(0, horizons.length) : horizons.map(fallbackPath);
          let nearestSquared = 200 * 200;
          for (let k = 0; k < predicted.length; ++k)
            for (let point = 0; point < horizons.length; ++point) {
              const dx = predicted[k][point][0] - path[point][0],
                dy = predicted[k][point][1] - path[point][1],
                b = bullets[k],
                clearance = b.circle ? Math.hypot(dx, dy) - b.rx :
                  Math.hypot(Math.max(0, Math.abs(dx) - b.rx),
                    Math.max(0, Math.abs(dy) - b.ry));
              nearestSquared = Math.min(nearestSquared, Math.max(0, clearance) ** 2);
            }
          const [px, py] = prediction.points ? prediction.points[horizons.length] : fallbackPath(8);
          let cost =
            10000 / Math.max(1, nearestSquared) +
            Math.hypot(px - targetX, (py - targetY) * 1.5) * 0.007;
          let laserDanger = false;
          let bodyDanger = false;
          // Use the same live collision geometry/gates as StageBattle. Enemy
          // motion is observed, so a moving body cannot look like empty space.
          for (const enemy of bodies)
            for (let point = 0; point < 4; ++point) {
              const t = horizons[point], ex = enemy.x + enemy.dx * t,
                ey = enemy.y + enemy.dy * t;
              let clearance;
              if (!(enemy.flags & 0x8000)) {
                clearance = Math.hypot(path[point][0] - ex, path[point][1] - ey) -
                  Math.hypot(hitSize, enemy.width * 0.5);
              } else {
                const ca = Math.cos(-enemy.angle), sa = Math.sin(-enemy.angle),
                  dx = path[point][0] - (ex - Math.cos(enemy.angle) * enemy.height * 0.5),
                  dy = path[point][1] - (ey - Math.sin(enemy.angle) * enemy.height * 0.5),
                  longitudinal = ca * dx - sa * dy, lateral = sa * dx + ca * dy,
                  side = Math.max(0, Math.abs(lateral) - (enemy.width + hitSize) * 0.5),
                  front = Math.max(0, -longitudinal - hitSize * 0.5,
                    longitudinal - enemy.height - hitSize * 0.5);
                clearance = Math.hypot(side, front);
              }
              cost += 2000 / Math.max(1, Math.max(0, clearance) ** 2);
              if (clearance < 10) bodyDanger = true;
            }
          if (px < -168 || px > 168 || py > 420 || py < 80) cost += 0.3;
          cost += Math.max(0, Math.abs(px) - 145) * 0.035;
          if (s.invulnerable === 0 && !team.active)
            for (let point = 0; point < 4; ++point) {
              const [x, y] = path[point];
              if (m._th13_debug_mp_laser_contact(x, y, 5)) {
                cost += 100;
                laserDanger = true;
              }
              else if (m._th13_debug_mp_laser_contact(x, y, 12)) cost += 3;
            }
          if (s.invulnerable > 45 || team.active)
            cost = cost * 0.2 + Math.hypot(px - targetX, (py - targetY) * 1.5) * 0.025;
          if (rescue?.phase === "approach")
            cost += Math.hypot(px - targetX, py - targetY) * 0.05;
          if (state.direction === (dx + 1) * 3 + dy + 1) cost -= 0.03;
          if (!best || cost < best.cost)
            best = { cost, nearest: Math.sqrt(nearestSquared), laserDanger, bodyDanger, dx, dy, speed,
              prediction: {mode:prediction.mode, reason:prediction.reason || null,
                sha:prediction.sha || null, unknown:prediction.unknown || [],
                focus:prediction.focus ?? null, dx, dy, shift:speed === 2,
                scope:"normal-SHT trajectory estimate; future gates and special states not guaranteed"} };
        }
    state.movementPrediction = best.prediction;
    if (chooseMovement) {
      state.direction = (best.dx + 1) * 3 + best.dy + 1;
      move = (best.speed === 2 ? 8 : 0) |
        (best.dx < 0 ? 64 : best.dx > 0 ? 128 : 0) |
        (best.dy < 0 ? 16 : best.dy > 0 ? 32 : 0);
      nextMoveCapture = capture + 1;
      ++state.movementDecisions;
    }
    let mask = 1 | dialogueMask | move;
    const deathbomb = s.phase === 4;
    // Predicted paths select movement; spend finite defense resources only
    // after the real production hit opens the 16-frame deathbomb window.
    if (deathbomb) {
      if (team.slot >= 600 && !team.active) mask |= 2048;
      else if (s.bombs > 0 && (!team.active || team.owner !== seat)) mask |= 2;
      // A previous defensive press can have been rejected by a dialogue or
      // active Bomb. During the actual deathbomb window, release a held key
      // before trying the next press through the production input sampler.
      if (deathbomb) mask &= ~(state.mask & (2 | 2048));
      if (mask & (2048 | 2)) {
        bombAt = frame;
        emit({ type: "defend", stage, frame, nearest: best.nearest,
          laserDanger: best.laserDanger, bodyDanger: best.bodyDanger, deathbomb, mask });
      }
    }
    if (rescue && rescue.phase !== "approach")
      mask = (rescue.down ? 8 : 0) | (mask & (2 | 2048));
    set(mask, Boolean(hazards.dialogue));
    ++state.decisions;
  };
  set(1);
}
