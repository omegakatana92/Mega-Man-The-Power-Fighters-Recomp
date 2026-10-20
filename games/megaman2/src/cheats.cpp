// SPDX-License-Identifier: GPL-3.0-only
//
// Mega Man 2: The Power Fighters: cheats and free play. Optional features (the launcher's Features page
// and the in-game menu switch them); they work on the game's memory after every frame. With every
// feature off the game runs exactly as the board does.
#include "megaman2.h"

#include "state.h"

namespace cps2recomp {

namespace {
    using namespace megaman2;

    // What the battle began with: the full health of each player (learned during the "READY" frames,
    // when the bars are still up). 0: no battle has been seen yet, or that player is not in it.
    uint8_t healthAtStart[2] = {0, 0};

    void ForgetBattle() {
        healthAtStart[0] = healthAtStart[1] = 0;
    }

    void Frame(Machine &) {
        // (two credits: a second player can always join. It is what the title waits for, so the attract
        // show gives way to it, as on a board with free play set.)
        if (FeatureOn("free_play") && Ram8(kCredits) < kCreditsFree) {
            SetRam8(kCredits, kCreditsFree);
        }
        if (!InBattle()) {
            ForgetBattle();
            return;
        }
        // (learn the battle's full health while the bars are still up)
        for (int n = 0; n < 2; n++) {
            const uint8_t health = Ram8(Player(n) + kHealth);
            if (health > healthAtStart[n] && health <= kHealthFull) {
                healthAtStart[n] = health;
            }
        }
        if (FeatureOn("infinite_time")) {
            // The clock stands still on "99", as the board's own free-play settings never do.
            SetRam8(kClock, kClockFull);
        }
        static const char *const kNoDamage[2] = {"no_damage", "no_damage_2"};
        for (int n = 0; n < 2; n++) {
            if (FeatureOn(kNoDamage[n]) && healthAtStart[n] != 0) {
                SetRam8(Player(n) + kHealth, healthAtStart[n]);
            }
        }
        // One-hit knockouts: every Robot Master's record is left with a single point of health, so any
        // hit ends the battle. (A hit does at least one, and a defeated boss's record reads 0, which is
        // left alone.)
        if (FeatureOn("one_hit")) {
            for (uint32_t address : kBossHealth) {
                if (Ram8(address) > 1) {
                    SetRam8(address, 1);
                }
            }
        }
    }
} // namespace

void CheatsFrame(Machine &machine) {
    Frame(machine);
}

void CheatsState(Archive &ar) {
    ar.Pod(healthAtStart[0]);
    ar.Pod(healthAtStart[1]);
}

void RegisterCheats() {
    Game &game = TheGame();
    game.features.push_back({"free_play", "Game", "Free play", "Start works without a coin.", kFeatureEnhanced});
    game.features.push_back({"no_damage", "Cheats", "No damage", "Player 1's health does not go down."});
    game.features.push_back(
        {"no_damage_2", "Cheats", "No damage, player 2", "Player 2's health does not go down."});
    game.features.push_back({"infinite_time", "Cheats", "Infinite time", "The battle's clock stands still on 99."});
    game.features.push_back(
        {"one_hit", "Cheats", "One-hit knockouts", "The Robot Master goes down with the first hit."});
}

} // namespace cps2recomp
