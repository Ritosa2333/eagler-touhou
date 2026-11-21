// Generated from RUEEE/thprac (MIT). definitions 748ad3affe12d6f30913c6c6401a92885a89afcfb1be665406238e18d0369359
export const TH13_THPRAC = {
  "defaults": {
    "mode": 1,
    "stage": 0,
    "section": 0,
    "phase": 0,
    "dlg": false,
    "score": 0,
    "life": 8,
    "extend": 0,
    "life_fragment": 0,
    "bomb": 8,
    "bomb_fragment": 0,
    "power": 400,
    "value": 10000,
    "graze": 0,
    "trance_meter": 200,
    "spirit_start_left": false,
    "useOD": false
  },
  "ranges": {
    "mode": [
      0,
      1
    ],
    "stage": [
      0,
      6
    ],
    "section": [
      0,
      19999
    ],
    "phase": [
      0,
      8
    ],
    "score": [
      0,
      9999999990
    ],
    "life": [
      0,
      8
    ],
    "extend": [
      0,
      6
    ],
    "life_fragment": [
      0,
      8
    ],
    "bomb": [
      0,
      8
    ],
    "bomb_fragment": [
      0,
      8
    ],
    "power": [
      0,
      400
    ],
    "value": [
      0,
      9999990
    ],
    "graze": [
      0,
      99999999
    ],
    "trance_meter": [
      0,
      600
    ]
  },
  "functional": [
    "coarse-stage-warp",
    "initial-resources"
  ],
  "deferred": [
    "exact-section-warp",
    "multi-phase-spell-start",
    "section-dialogue",
    "overdrive",
    "spirit-side"
  ]
};
