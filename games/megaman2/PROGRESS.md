# Mega Man 2: The Power Fighters: progress

What is known of *Mega Man 2: The Power Fighters* (USA 960708)'s memory, and what has been built on it.
Every address here was confirmed by experiment on the running board with the toolkit's own tools
(`docs/FEATURES.md`). Two published cheat collections for this same ROM set (FinalBurn Neo's and the
Kawaks/FBA archive) were used as a list of candidates; each candidate was then checked on the board
before it was used, and where one was wrong it is said so below. Nothing is a dump of the game's data,
and no game data is written down.

## The ROM set

The set is MAME's **`megaman2`** (USA 960708), the parent of `megaman2a`, `megaman2h` and `rockman2j`.
It was told apart from the clones, and its files listed, from the set's own contents against MAME's ROM
definition for `megaman2` (Capcom, 1996, board `cps2_2p6b`, 2 players, 3 buttons):

- the program is three ROMs — `rm2u.03`, `rm2u.04`, `rm2.05` — 1.5 MiB in all, so `code_end = 180000`;
- the graphics are four 2 MiB ROMs — `rm2.14m`, `rm2.16m`, `rm2.18m`, `rm2.20m` — side by side at
  `0x800000` (MAME fills 8 MiB first, unlike the earlier CPS-2 boards);
- the sound program is `rm2.01a`, `rm2.02`; the QSound samples are `rm2.11m`, `rm2.12m`;
- the key is `megaman2.key`.

Every file is checked by CRC-32 (`games/megaman2/game.cfg`), so only this revision is accepted.

## How the game's own memory is found

The tools used, in the order they are reached for (all in `docs/FEATURES.md`):

- `--movie FILE` replays an input movie; a run made with `--auto` and recorded with `--record` is how
  the game is driven to a match and made to flail at it. The recording has a coin at frame 772 and the
  match starts at about 1400.
- `--poke ADDR=BYTE` holds bytes at a guess, after every frame; `--trace LOW-HIGH:FILE` writes a
  stretch of work RAM after every frame; `--dump-ram FRAME:FILE` writes all 64 KiB after one frame;
  `--shot-every N:PFX` writes pictures of the board.
- `--watch LOW[-HIGH]` prints every write by the interpreter (`--cpu blocks`) with the instruction's
  address; it stops after 2000 reports, so it is useful only on a narrow range and after the boot's
  clearing of RAM.
- Every trace here has one trap worth writing down: the tool writes **`high - low + 1` bytes per
  frame**, not `high - low`. A range read back with the wrong width is shifted by one byte a frame and
  looks like a plausible but wrong record; every trace below was read with the width it was written at.

**Getting to a match.** A movie made with `--auto 1 --record` inserts coins, presses Start and plays
badly; its first real match (Shadow Man) is at about frame 1400. It was replayed with `--movie` for
every trace and dump so that the same thing happens at the same frame. Two coins are not needed for a
credit in this game: one coin makes one credit, and the title offers Start when the count is at least
one.

**The credits.** A `--trace` of the whole of work RAM over the coin frames was searched for a small byte
that went up as a coin pulse ended; `FF8068` went 0 → 1 at frame 801 and is at most 9. Holding it at 2
with `--poke` (no coin at all) put "CREDIT(S) 2 (0/2)" under the title, and holding it at 2 in the
`free_play` feature makes Start work from a cold boot. The neighbouring bytes are the pulse: `FF8062`
rises with player 1's coin and `FF8063`/`FF8067` with player 2's, and `FF8066` is a 50-frame countdown
afterwards.

**The clock.** A `--trace` of the player records showed a byte counting down in BCD one step every 70
frames; `FF811D` starts at `0x89` at "READY" and reaches 0, with `FF811C` a 0/1 "running" flag beside
it. Holding `FF811D` at `0x99` with `--poke` made the digits stand on "99" through a whole match; that
is what `infinite_time` does. (The cheat archive's address is `FF811C`, the high byte of the word; the
byte that is the two digits is `FF811D`.)

**The player records.** The two records are 0x400 apart, player 1's at `FF8300` and player 2's at
`FF8700`; the cheat archive names the same two bases. Within a record:

- `+0x51` is the health, a byte, `0x20` at a battle's start and 0 outside one. A `--trace` showed
  `FF8351` falling 0x20 → 0x1C → ... → 0 as player 1 was hit, and `FF8751` doing the same for player 2;
  `--poke FF8351=20` and `--poke FF8751=20` held both bars up through a whole match. The cheats name
  the same two bytes.
- `+0x10` and `+0x14` are the place across the stage and the height, words. A movie that walked left
  then right made `FF8310` ramp down and up (larger is further right) while `FF8314` stayed at the
  floor and rose on jumps. `FF8710`/`FF8714` are player 2's. These are not used by a feature yet.
- `+0x00` is a small state (0 outside, 1/2 in a battle) and `+0x01` an animation phase; neither is the
  state word the training aids want.

**The Robot Master's health.** A whole-RAM dump at frame 1500 of the replayed match (at "READY", boss
bar full) and another at 5000 (the same match, after hits) were compared for bytes that fell. Three
bytes answered, each `0x40` at the first dump and `0x2E` at the second: `FFCD51`, `FFCE51` and
`FFEF61`. The cheat archive's "1 Hit Kill" names the three records one byte lower (`FFCD50`, `FFCE50`,
`FFEF60`), which is the same two bytes read as a word; the health is the low byte. Holding **one** of
them at 1 with `--poke FFCD51=01` ended the first match with "WIN!" and the CLEAR BONUS screen by frame
2700, while the same movie without the poke was still fighting; holding all three (`one_hit`) does the
same. A hit does at least one point, and a defeated boss's byte reads 0, so the feature only lowers a
byte that is above 1 and never revives one.

**A wrong turn: the charge.** The cheat archive's "P1 Charge Shot" names `FF8434` (values 0, 2, 4) and
its "Hyper Charge" names `FF815A`/`FF815B` (0xFF). `FF8434` stayed 0 through a 14000-frame trace of
ordinary play, and `FF815A`/`FF815B` stayed 0 too; `FF8435` turned out to be a **shot type** (2 for a
normal buster shot, briefly 4 when a long hold released) and `FF8430`/`FF8433` the attack's own
timers (`FF8430` is 1 while Punch 1 is held). Holding `FF815A`/`FF815B` at 0xFF changed the game's
screen flow, so those are not a charge switch this port should touch. The charge gauge is still to be
found; `full_charge` is **not** built (see `ROADMAP.md`).

**Facing.** The input display turns with the stick each player holds. This is a two-player co-operative
boss rush, so there is no opponent on the other side to turn towards; a player's record does have a
small byte at `+0x05` that changed 0 → 2 during a right-hand walk, but it also moved with state changes
(death, spawn) in the same run, so it is not named as a facing flag. The stick is the safer source and
is what `training.cpp` uses.

## The memory so far

| Address | Size | What | How it was found |
| --- | --- | --- | --- |
| `FF8068` | byte | **Credits**: what the title reads before it offers Start; one coin, one credit (max 9) | Whole-RAM `--trace` across the recorded coin: 0 → 1 at frame 801; `--poke` at 2 shows "CREDIT(S) 2"; `free_play` starts with no coin. |
| `FF811D` | byte | **Clock**: BCD seconds, `0x89` at "READY", down to 0 | `--trace` of the records over a match; `--poke` at `0x99` stands the digits on "99". The word at `FF811C` holds the run flag in its high byte. |
| `FF8300` | 0x400-byte record | **Player 1** | The cheat archive names the base; the record's fields were found by trace and poke. |
| `FF8700` | 0x400-byte record | **Player 2** | Same, 0x400 on. |
| `FF8351`, `FF8751` | byte each | **Health** of player 1 and player 2: `0x20` at a battle's start, 0 outside one | `--trace` fell as each was hit; `--poke` at `0x20` held both bars up; also in the cheat archive. |
| `FF8310`, `FF8710` | word each | **Place across the stage** (larger is further right) | A movie that walked left then right; `--trace` of the record. |
| `FF8314`, `FF8714` | word each | **Height above the floor** (larger is higher) | The same movie with jumps; `--trace`. |
| `FFCD51`, `FFCE51`, `FFEF61` | byte each | **Robot Master's health**: `0x40` at a battle's start | Whole-RAM dumps at frames 1500/5000 (0x40 → 0x2E in all three); `--poke` at 1 gives "WIN!"; the cheat archive's word addresses are one lower. |

## What was built on it

All in `games/megaman2/src/`:

- `free_play` (Enhanced) — holds the credit byte at 2.
- `no_damage`, `no_damage_2` — puts each player's health back to the battle's full value (learned
  during "READY") every frame, if that player is in the battle.
- `infinite_time` — holds the clock byte at `0x99` while a battle is on.
- `one_hit` — lowers each Robot Master health byte that is above 1 to 1.
- `input_display` (Training) — the toolkit's `fight::InputHistory`, turned with the stick each player
  holds.

A battle is recognised by player 1's health being 1..`0x20` and the clock being a BCD number; outside
one the health reads 0 and every cheat leaves the game alone. Each feature was run from the built
program with `--features NAME` over the recorded movie, and the result seen in a picture: the title's
"CREDIT(S) 2", the clock standing on "99", both health bars full, the first hit's "WIN!" and CLEAR
BONUS, and the input display at both sides of the picture. With every feature off the same movie gives
the same run as before the game's sources were added.

## The translation

The first `make` (interpreter only) ran the set at about 800 frames a second; `make explore` found
**6,105 entries** from the five roots it reached (0 → 4002 → 5796 → 6093 → 6104), and the final
`make` translated them to **65,105 instructions** in 43 files, with **69 instructions left to the
interpreter** (46 of them data a guess took for code). A 14000-frame replayed match then reported
**0 instructions run in the interpreter**, and the sound program 1 instruction without a translation
out of 4,430.

## What is still unknown

- **What a player is doing** (standing, walking, jumping, shooting, charging, reeling): the records
  have several small bytes that move with the animation, but none has been told apart yet. The frame
  data, the training dummy and the combo readout all wait on it.
- **The charge gauge**: see the wrong turn above. `full_charge` is not built.
- **The Robot Master's place, state and boxes**: only its health is named; the damage readout and the
  Overdrive readout want the rest of its record.
- **The attack boxes** and the camera's place, for the hit boxes and widescreen.
