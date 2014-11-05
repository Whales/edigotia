#include "combat.h"
#include "rng.h"

// max_attacks defaults to 0 (no maximum)
Battle_result quick_battle(int attacker_number, int attacker_strength,
                           int attacker_hp,
                           int defender_number, int defender_strength,
                           int defender_hp,
                           int max_attacks)
{
  int attackers_left = attacker_number, defenders_left = defender_number;

  while (attackers_left > 0 && defenders_left > 0) {
    for (int i = 0; i < defenders_left && (max_attacks == 0 || i < max_attacks);
         i++) {
      attacker_hp -= rng(0, defender_strength);
    }
    for (int i = 0; i < attackers_left && (max_attacks == 0 || i < max_attacks);
         i++) {
      defender_hp -= rng(0, attacker_strength);
    }

    while (attacker_hp <= 0) {
      attacker_hp += 100;
      attackers_left--;
    }

    while (defender_hp <= 0) {
      defender_hp += 100;
      defenders_left--;
    }
  }

  if (attackers_left < 0) {
    attackers_left = 0;
  }
  if (defenders_left < 0) {
    defenders_left = 0;
  }
  int attackers_dead = attacker_number - attackers_left;
  int defenders_dead = defender_number - defenders_left;

  Battle_result ret;
  ret.attackers_dead = attackers_dead;
  ret.defenders_dead = defenders_dead;

  if (attackers_left == 0 && defenders_left == 0) {
// Everyone died!  Oh, the humanity!
    ret.result = COMBAT_RES_BOTH_DEAD;
  } else if (attackers_left == 0) {
    ret.result = COMBAT_RES_DEFENDER_WON;
  } else if (defenders_left == 0) {
    ret.result = COMBAT_RES_ATTACKER_WON;
  } else {  // Should never happen, but for safety's sake...
    ret.result = COMBAT_RES_DRAW;
  }

  return ret;
}
