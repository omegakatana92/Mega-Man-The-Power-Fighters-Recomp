// SPDX-License-Identifier: GPL-3.0-only
//
// Mega Man 2: The Power Fighters: the training aids, put together from the toolkit's fight/ parts
// (fight/fight.h). This game is a two-player co-operative boss rush: the two players fight the Robot
// Master together, so the input display turns with the stick each player holds, not towards the other
// player. The other aids (frame data, the combo readout, the hit boxes) wait on more of the game's own
// memory being known: the players' and the boss's records are found (megaman2.h), with their health and
// places picked out, but what a player is doing is not yet told apart (PROGRESS.md).
#include "megaman2.h"

#include "fight.h"
#include "machine.h"
#include "state.h"

namespace cps2recomp {

namespace {
    using namespace megaman2;

    fight::InputHistory g_inputs;
    bool g_facingLeft[2] = {false, false};

    void ReadInputs(Machine &machine) {
        for (int n = 0; n < 2; n++) {
            const uint16_t held = machine.players[n];
            // The picture turns a player to the way the stick is pushed (holding left faces left), so
            // the notation does too; with the stick centred the last way is kept.
            if (held & fight::kStickLeft) {
                g_facingLeft[n] = true;
            } else if (held & fight::kStickRight) {
                g_facingLeft[n] = false;
            }
            g_inputs.Push(n, held, g_facingLeft[n]);
        }
    }
} // namespace

void TrainingBeforeFrame(Machine &machine) {
    ReadInputs(machine);
}

void TrainingFrame(Machine &) {
    std::vector<HudLine> lines;
    if (FeatureOn("input_display")) {
        g_inputs.Hud(0, lines);
        g_inputs.Hud(1, lines);
    }
    SetHud(lines);
}

void TrainingState(Archive &ar) {
    // (fight::InputHistory's own frame counts are not part of the save state: a load starts the display over)
    if (!ar.Saving()) {
        g_inputs.Clear();
        g_facingLeft[0] = g_facingLeft[1] = false;
    }
}

void RegisterTraining() {
    Game &game = TheGame();
    game.features.push_back({"input_display", "Training", "Input display",
                             "What each player holds, and for how long, at their side of the picture."});
    TrainingAids({"input_display"});
}

} // namespace cps2recomp
