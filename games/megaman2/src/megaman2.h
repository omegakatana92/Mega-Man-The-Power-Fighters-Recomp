// SPDX-License-Identifier: GPL-3.0-only
//
// Mega Man 2: The Power Fighters (USA 960708): names for what is known of the game's memory. Every
// address here was found on the running board with the toolkit's own tools (--movie, --dump-ram,
// --poke, --trace) and checked again with --poke and the picture, as PROGRESS.md tells in full. A
// public FinalBurn Neo cheat list named some candidates; each was confirmed here before it was used.
// No game data is written down, only where the game keeps it.
#pragma once

#include "game.h"

#include <cstdint>

namespace megaman2 {

// The credits the game is holding, a byte: the title shows it as "CREDIT(S) n", and a coin pulse at
// FF8062/FF8067 makes it count up when the pulse ends. Two credits are enough for both players to
// start. Found with a whole-RAM --trace across a recorded coin (FF8068 goes 0 -> 1 at frame 801), and
// checked by holding it at 2: the title then shows "CREDIT(S) 2" without a coin.
constexpr uint32_t kCredits = 0xFF8068;
constexpr uint8_t kCreditsFree = 2;

// A player's record: the two are 0x400 apart, player 1's first. Within a record:
//   +0x51  the health, a byte, 0x20 at a battle's start (0 outside a battle)
//   +0x10  the place across the stage, a word (larger is further right)
//   +0x14  the height above the floor, a word (larger is higher)
// Found with --trace over a match: the health byte falls as a player is hit, and the places move with
// the stick; --poke at 0x20 holds the health bar up (PROGRESS.md).
constexpr uint32_t kPlayer0 = 0xFF8300, kPlayerStride = 0x400;
constexpr uint32_t kHealth = 0x51, kPositionX = 0x10, kPositionY = 0x14;
constexpr uint8_t kHealthFull = 0x20;
inline constexpr uint32_t Player(int n) {
    return kPlayer0 + static_cast<uint32_t>(n) * kPlayerStride;
}

// The battle's clock. The high byte of the word at FF811C says whether it runs; the low byte
// (FF811D) counts down in BCD from 0x89. Holding that byte at 0x99 makes the clock stand on "99":
// found by --trace of a countdown and checked with --poke and the picture (PROGRESS.md).
constexpr uint32_t kClock = 0xFF811D;
constexpr uint8_t kClockFull = 0x99;

// The Robot Master's health, a byte, 0x40 at a battle's start. The game keeps the same value in three
// places (the active boss, a second boss slot, and the record the life bar is drawn from); a hit takes
// from all that are there. Found by dumping RAM at frames 1500 and 5000 of the same recorded match
// (each 0x40 -> 0x2e), and checked by holding all three at 1: the first hit gives "WIN!" (PROGRESS.md).
constexpr uint32_t kBossHealth[3] = {0xFFCD51, 0xFFCE51, 0xFFEF61};

// A battle is being fought when player 1 is up (health 1..0x20) and the clock is a BCD number. Outside
// one (the title, the character and story screens, the continue screen) the health reads 0, which is
// what tells the cheats not to touch anything.
inline bool InBattle() {
    const uint8_t health = cps2recomp::Ram8(Player(0) + kHealth);
    if (health == 0 || health > kHealthFull) {
        return false;
    }
    const uint8_t clock = cps2recomp::Ram8(kClock);
    return (clock >> 4) <= 9 && (clock & 0x0F) <= 9;
}

} // namespace megaman2
