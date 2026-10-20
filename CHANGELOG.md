# Mega Man 2: The Power Fighters Recomp — changelog

## 0.1.0-beta.1

First build. The 68000 and Z80 programs of *Mega Man 2: The Power Fighters* (USA 960708) are
translated to C++ ahead of time and run on cps2recomp's own machine. The launcher, the in-game menu,
save states, run-ahead, the display options and every control are there; both players' controls are
remappable.

The game's own features are in, from addresses found on the running board (each recorded in
[games/megaman2/PROGRESS.md](games/megaman2/PROGRESS.md) with how it was found):

- **Free play**, so Start works without a coin;
- the cheats: **No damage** for either player, **Infinite time**, and **One-hit knockouts** for the
  Robot Master;
- the training mode's first aid: an **Input display** at each player's side, turned with the stick
  each player pushes.

**Full charge** is not in this build: the value behind the buster's charge was not confirmed on the
board (the cheat archive's candidate stayed 0 in ordinary play, and its neighbour is a shot type).
Frame data, the combo readout, the dummy and the hit boxes are also still to come; they wait on the
state that says what a player is doing. See [ROADMAP.md](ROADMAP.md).
