# Mega Man 2: The Power Fighters — features and roadmap

Where this port stands against a finished one, and what each missing piece needs. The reference is the
toolkit's own complete example, [vampire-savior-recomp](https://github.com/tmcleroy/vampire-savior-recomp)
(`games/vampire_savior/src/`), and the toolkit's documents `docs/FEATURES.md` and `docs/FIGHT.md`.

Everything below is an **optional feature**: with every feature off, the port must run exactly as the
board does. Nothing from the game (no ROM, no RAM dump, no ripped art/text) may ever be committed — each
thing found is recorded in `PROGRESS.md` as *how* it was found (a RAM diff, a write watch, a trace),
never as the data itself.

This is a two-player **co-operative** boss rush: the two players fight the Robot Master together, so
the training aids here are about reading the fight (input display, boxes, damage), not about driving
an opponent. The CPU's Robot Master is the game's own, not player 2.

## What works today

From the toolkit, with no game work:

- The **launcher**: chooses the ROM set, checks every file by CRC-32, sets the controls, Play.
- The **in-game menu** (Esc, `F1`, a stick's click, or Start+Select): save states (9 slots), Reset,
  Insert coin, Player 2 joins, Quit game.
- **Stop, step, slow** (P, O, `-`/`=`).
- **Run-ahead** and the **Display** options (scaling, picture shape, window size, full screen).
- Every key and button **remappable**, for each player.

Built on the game's own memory (`games/megaman2/src/`):

- `free_play` (Enhanced) — holds two credits, so Start works with no coin.
- `no_damage`, `no_damage_2` — a player's health is put back to the battle's full value every frame.
- `infinite_time` — the battle's clock stands still.
- `one_hit` — the Robot Master is left a single point of health.
- `input_display` (Training) — what each player holds, and for how long, turned with the stick each
  player pushes (the two players fight the Robot Master together, so there is no opponent to turn to).

`--list-features` shows these with the toolkit's.

## What a finished port adds (this is the work)

Steps 0 and 1 are **done** as far as the cheats and the input display need; the state word (step 0) and
everything in step 2 beyond the input display are still to come.

### 0. Foundation — know the game's memory (mostly done)

The credits, the clock, the two players' records and the Robot Master's health are found and named in
`games/megaman2/src/megaman2.h` — health, the place across the stage, the boss's health — with how each
was found in `PROGRESS.md`. Still missing: **the state that says what a player is doing** (standing,
walking, jumping, shooting, charging, reeling), the **charge gauge** and the **attack boxes**, which the
frame data and the hit boxes want; and the Robot Master's own record beyond its health (place, state,
boxes), which the damage readout and the Overdrive readout want.

How the toolkit finds these (all in `docs/FEATURES.md`):

- `--dump-ram F:FILE` + `tools/ramfind.py DUMP=VALUE ...` — addresses that held a known value across
  several dumps.
- `--trace LOW-HIGH:FILE` with a run that does known things at known frames.
- `--watch LOW[-HIGH]` — every write to an address and the instruction that made it, read against
  `work/megaman2/program.lst`.
- `--poke ADDR=BYTE,...` to try a guess; `--menu FRAME:KEYS` to press the menu's own rows;
  `--shot-every N:PFX` to see a run.

### 1. The Features page (the game's own switches) — done

| Feature | Kind | Needs |
| --- | --- | --- |
| `free_play` | on/off | the credit word — **done** |
| `no_damage`, `no_damage_2` | on/off | each player's health and the battle's full value — **done** |
| `infinite_time` | on/off | the battle's clock — **done** |
| `one_hit` | on/off | the Robot Master's health — **done** |
| `full_charge` | on/off | the value behind each player's buster charge — **not yet** |
| Board/operator settings (difficulty, free play) | choices | reading the EEPROM's operator settings |
| "The game's own test menu" | command | a routine address to jump to |

### 2. Training mode (the headline feature)

| Piece | What it adds | Needs |
| --- | --- | --- |
| Input display | what the stick holds, and for how long | **done** — turned with the stick each player pushes |
| Hit boxes | the players', the Robot Master's and the shots' boxes, drawn on the picture | the box tables and the camera's place |
| Frame data | startup, active, recovery of each attack | the state that says a move is out, per player |
| Damage readout | hits, damage and what of it comes back | the health words and the damage rule |
| Overdrive readout | when the Robot Master's second phase begins | the boss record's phase flag |

The toolkit's `fight/` parts (`fight/fight.h`) give all of these once the game's own state is known;
the input display is the first one and needs only the facing.

### 3. Alternative modes

Each is a command with a page of its own: an **attract watcher** (the credit word held at zero), and a
**one-credit run** (the credit word held at one, and the continue screen skipped). A two-machine link
is not possible (the board's link hardware is a separate build).

### 4. Widescreen

A switch on the launcher's first page, off in every preset (it can change how the game plays). The
field is shown in 16:9, 512 columns where the board's 384, at the same height and scale. Needs
rewrites of the camera and the stage scrollers and of the objects that must be kept while they are in
the wide picture. The toolkit's `widescreen.cpp` in Vampire Savior is the example.

### 5. Readable rewrites (correctness, and what the above builds on)

Rewrite the game's key routines by hand as named C++ (`docs/READABLE.md`): the players' movement and
collision, the damage rule, the charge/shot code, and the Robot Master's phase change. This is what
makes a clean `one_hit`, the damage readout and the widescreen possible, and it is the part most worth
doing carefully.

## Recommended order

1. **The foundation** — the boss record, the state word, the camera and the attack boxes.
2. **The training aids** — hit boxes first, then frame data and the damage readout.
3. **Readable rewrites** — needed by 2 and 4, and correctness.
4. **Widescreen.**
5. **Alternative modes.**

## Before uploading: a checklist

- [x] `README.md` states the exact ROM set (`megaman2`, USA 960708) and that **only that revision**
      works, with the Capcom disclaimer.
- [x] `rom/`, `build/`, `work/` are ignored and **absent** from the repository; no game data, keys or
      dumps anywhere (`.github/workflows/check.yml` fails a push that tracks one); no screenshots.
- [x] `PROGRESS.md` records how each finding was made (a RAM diff, a watch, a trace), not the data.
- [x] `make check`, `make check-toolkit`, `make check-common` all pass (the CI runs the last two).
- [x] `LICENSE` (GPL-3.0-only) and `THIRD_PARTY_NOTICES.md` (the cipher is MAME's, BSD-3-Clause); the
      release archives carry them and the licences of what they ship.
- [x] Repo description set (see README's title line / the About field).
- [x] First commit made and pushed; only source, docs and tooling are tracked (no ROM, key or derived
      data — `rom/`, `work/` and `build/` are ignored).
- [ ] The repository builds from a fresh clone with the player's own ROM set (`make`, `make explore`,
      `make launcher`).
