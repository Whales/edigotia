#ifndef _COMBAT_H_
#define _COMBAT_H_

enum Combat_result
{
  COMBAT_RES_NULL = 0,

  COMBAT_RES_ATTACKER_WON,
  COMBAT_RES_DEFENDER_WON,

  COMBAT_RES_ATTACKER_FLED,
  COMBAT_RES_DEFENDER_FLED,

  COMBAT_RES_BOTH_DEAD,
  COMBAT_RES_DRAW,  // A draw for any other reason

  COMBAT_RES_MAX
};

struct Battle_result
{
  Battle_result(Combat_result R = COMBAT_RES_NULL, int AD = 0, int DD = 0) :
    result (R), attackers_dead (AD), defenders_dead (DD) {}

  Combat_result result;
  int attackers_dead;
  int defenders_dead;
};

/* quick_battle returns a positive number of defenders killed, or a negative
 * number of attackers killed.
 * max_attacks limits how many warriors can make an attack at once; if one
 * side's numbers exceed this, the extra warriors will have to "wait" and won't
 * deduct HP from the opposing side.  This helps limit the (possibly
 * overwhelming) advantage of having bigger numbers.
 */
Battle_result quick_battle(int attacker_number, int attacker_strength,
                           int attacker_hp,
                           int defender_number, int defender_strength,
                           int defender_hp,
                           int max_attacks = 0);

#endif
