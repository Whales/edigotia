#include "race.h"
#include "stringfunc.h"
#include "rng.h"    // For Race_datum::get_city_name()
#include "window.h" // For debugmsg()
#include <cstdarg>  // For variadic function Race_datum::add_city_names()
#include <sstream>  // For Race_datum::get_city_name()

Race_datum::Race_datum()
{
  uid = -1;
  name        = "Uninitialized Race";
  plural_name = "Uninitialized Race";
  adjective   = "Uninitialized Race";
  description = "Uninitialized Race";

  color = c_ltgray;

// Some reasonable defaults.
  for (int i = 0; i < CIT_MAX; i++) {
    starting_population [i] = 0;
    birth_rate          [i] = 100;
    citizen_ratio       [i] = 0;
    morale_requirement  [i] = 0;
    low_tax_rate        [i] = 10;
    high_tax_rate       [i] = 70;
  }

  for (int i = 0; i < RES_MAX; i++) {
    starting_resources[i] = 0;
  }

  for (int i = 0; i < CITY_TYPE_MAX; i++) {
    city_size_min[i] = 100 + 1000 * i;
    city_size_max[i] = 1000 * i * i;
  }

// Some special cases...
  citizen_ratio[CIT_MERCHANT] = 10;
  citizen_ratio[CIT_BURGHER]  = 10;
  morale_requirement[CIT_MERCHANT] = 50;
  morale_requirement[CIT_BURGHER]  = 50;

  for (int i = 0; i < RACE_MAX; i++) {
    relations[i] = 0;
  }

  for (int i = 0; i < SKILL_MAX; i++) {
    skill_level[i] = 3;
  }

// Non-array-based variables
  cluster_min = 3;
  cluster_max = 4;
  base_combat = 10;
  hp = 100;
  food_consumption = 100;
  warlikeness = -2;
  life_value = 3;

}

Race_datum::~Race_datum()
{
}

void Race_datum::add_city_names(std::string pos, ...)
{
  std::string fixed_pos = no_caps( trim( pos ) );
  std::vector<std::string>* names;
  if (fixed_pos == "start") {
    names = &city_name_start;
  } else if (fixed_pos == "middle") {
    names = &city_name_middle;
  } else if (fixed_pos == "end") {
    names = &city_name_end;
  } else {
    debugmsg("Race_datum::add_city_names(\"%s\") called!", fixed_pos.c_str());
    return;
  }

// Start our variadic list and all that
  va_list ap;
  va_start(ap, pos);
  char* tmp;
  bool done = false;
  while (!done) {
    tmp = va_arg(ap, char*);
    if (tmp == NULL) {
      done = true;
    } else {
      names->push_back( std::string(tmp) );
    }
  }
}

std::string Race_datum::get_city_name()
{
  std::stringstream ret;
  if (!city_name_start.empty()) {
    int i = rng(0, city_name_start.size() - 1);
    ret << city_name_start[i];
  }
  if (!city_name_middle.empty()) {
    int i = rng(0, city_name_middle.size() - 1);
    ret << city_name_middle[i];
  }
  if (!city_name_end.empty()) {
    int i = rng(0, city_name_end.size() - 1);
    ret << city_name_end[i];
  }

/* Check for the characters '*' '?' or '!'
 * '*' means "repeat the previous consonant if it's not preceded by a
 * consonant."  e.g. "Wil*iams" => "Williams" but "Wilm*iams" => "Wilmiams"
 * '?' means "if the previous character and the following character are
 * identical, remove one."  e.g. "Wis?ster" => "Wister"
 * '!' means "if the previous character is a consonant, remove the following
 * character."  e.g. "Tat!bol" => "Tatol" but "Ta!bol" => "Tabol"
 */

  std::string name = ret.str();

  for (int i = 0; i < name.size(); i++) {

    if (name[i] == '*') {
      if (i == 0) { // Start of the name; just remove the *
        name = name.substr(1);
        i--;
      } else if (i == 1) {  // Again, just remove the *
        std::stringstream new_name;
        new_name << name[0] << name.substr(2);
        name = new_name.str();
        i--;
      } else if (!is_letter(name[i - 1]) || is_vowel(name[i - 1]) ||
                 !is_vowel(name[i - 2])) {
// Once again, just remove the *
        std::stringstream new_name;
        new_name << name.substr(0, i) << name.substr(i + 1);
        name = new_name.str();
        i--;
      } else {  // Replace the *!
        name[i] = name[i - 1];
      }

    } else if (name[i] == '?') {
      if (i == 0) { // Just remove the ?
        name = name.substr(1);
        i--;
      } else if (i == name.size() - 1) {  // Just remove the ? again
        name = name.substr(0, name.size() - 1);
        i--;
      } else if (name[i - 1] != name[i + 1]) { // Just remove the ? !!!
        std::stringstream new_name;
        new_name << name.substr(0, i) << name.substr(i + 1);
        name = new_name.str();
        i--;
      } else {  // Remove the ? AND the character before it.
        std::stringstream new_name;
        new_name << name.substr(0, i - 1) << name.substr(i + 1);
        name = new_name.str();
        i--;
      }

    } else if (name[i] == '!') {
      if (i == 0) { // Just remove the !
        name = name.substr(1);
        i--;
      } else if (i == name.size() - 1) {  // Just remove the ! again
        name = name.substr(0, name.size() - 1);
        i--;
      } else if (!is_letter(name[i - 1]) || is_vowel(name[i - 1])) { // Remove !
        std::stringstream new_name;
        new_name << name.substr(0, i) << name.substr(i + 1);
        name = new_name.str();
        i--;
      } else {  // Remove the ! AND the character after it.
        std::stringstream new_name;
        new_name << name.substr(0, i) << name.substr(i + 2);
        name = new_name.str();
        i--;
      }
    } // if (name[i] == '!')
  }

  return capitalize_all_words( name );
}

std::string Race_datum::generate_help_text()
{
// TODO: More links.
  std::stringstream ret;
  ret << "<c=" << color_tag(color) << ">" << capitalize(plural_name) <<
         "<c=/>" << std::endl;
  ret << "<c=ltgreen>Starting population:<c=/> " << std::endl;
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    std::string cit_name = citizen_type_name( Citizen_type(i), true);
    cit_name = capitalize(cit_name);
    ret << "  " << cit_name << ":";
    for (int n = 0; n < 10 - cit_name.length(); n++) {
      ret << " ";
    }
    if (starting_population[i] < 10) {
      ret << "  ";
    } else if (starting_population[i] < 100) {
      ret << " ";
    }
    if (starting_population[i] == 0) {
      ret << "<c=dkgray>";
    } else {
      ret << "<c=ltgray>";
    }
    ret << starting_population[i] << "<c=/>" << std::endl;
  }
  ret << std::endl;
  ret << "<c=yellow>Starting resources:<c=/> " << std::endl;
  for (int i = 0; i < RES_MAX; i++) {
    if (starting_resources[i] > 0) {
      std::string res_name = capitalize( Resource_data[i]->name );
      if (res_name.length() > 15) {
        res_name = res_name.substr(0, 15);
      }
      ret << "  " << res_name << ":";
      for (int n = 0; n < 16 - res_name.length(); n++) {
        ret << " ";
      }
      std::string amount_str = itos(starting_resources[i]);
      if (amount_str.length() < 6) {
        for (int n = 0; n < 6 - amount_str.length(); n++) {
          ret << " ";
        }
      }
      ret << amount_str << std::endl;
    }
  }

  ret << std::endl;
  std::string combat_color = "white";
  if (base_combat >= 14) {
    combat_color = "ltgreen";
  } else if (base_combat >= 11) {
    combat_color = "ltblue";
  } else if (base_combat <= 6) {
    combat_color = "red";
  } else if (base_combat <= 9) {
    combat_color = "yellow";
  }
  ret << "<c=ltred>Base combat skill: <c=" << combat_color << ">" <<
         base_combat << "<c=/>" << std::endl;

  std::string hp_color = "white";
  if (hp >= 130) {
    hp_color = "ltgreen";
  } else if (hp > 100) {
    hp_color = "ltblue";
  } else if (hp <= 80) {
    hp_color = "red";
  } else if (hp < 100) {
    hp_color = "yellow";
  }
  ret << "<c=ltred>HP: <c=" << hp_color << ">" << hp << "<c=/>" << std::endl;

  ret << std::endl;
  ret << "<c=white>Citizen:      Birth rate:     Acceptable tax range:<c=/>";
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    std::string cit_name = capitalize(citizen_type_name(Citizen_type(i), true));
    ret << "<c=white>" << cit_name << "<c=/>";
    for (int n = 0; n < (18 - cit_name.length()); n++) {
      ret << " ";
    }
    std::string rate_text = itos( birth_rate[i] * 100 );
    for (int n = 0; n < (6 - rate_text.length()); n++) {
      ret << " ";
    }
    ret << "<c=ltgreen>" << rate_text << "<c=/>";
    ret << "        ";
    if (low_tax_rate[i] < 10) {
      ret << " ";
    }
    ret << "<c=yellow>" << low_tax_rate[i] << "  - ";
    if (high_tax_rate[i] < 10) {
      ret << "  ";
    } else if (high_tax_rate[i] < 100) {
      ret << " ";
    }
    ret << high_tax_rate[i] << "<c=/>" << std::endl;
  }
  ret << "<c=white>(Birth rate is the number of citizens required to result " <<
         "in one birth per day.)<c=/>" << std::endl << std::endl;

  ret << "<c=yellow>" << citizen_ratio[CIT_MERCHANT] << " <c=white>peasants " <<
         "<c=ltgreen>required to support one <c=white>merchant<c=/>." <<
         std::endl;
  ret << "<c=white>Peasant<c=ltgreen> morale must be at least <c=yellow>" <<
         morale_requirement[CIT_MERCHANT] << " <c=ltgreen>to guarantee that " <<
         "<c=white>merchants<c=ltgreen> can be born.<c=/>" << std::endl;
  ret << "<c=yellow>" << citizen_ratio[CIT_BURGHER] << " <c=white>merchants " <<
         "<c=ltgreen>required to support one <c=white>burgher<c=/>." <<
         std::endl;
  ret << "<c=white>Merchant<c=ltgreen> morale must be at least <c=yellow>" <<
         morale_requirement[CIT_BURGHER] << " <c=ltgreen>to guarantee that " <<
         "<c=white>burghers<c=ltgreen> can be born.<c=/>" << std::endl;
  ret << std::endl;

  ret << "<c=green>Food consumption: <c=white>" << food_consumption << 
         "%%%%<c=/>" << std::endl << std::endl;

  ret << "<c=yellow>Skills: <c=/>" << std::endl;
// Start at 1 to skip SKILL_NULL
  for (int i = 1; i < SKILL_MAX; i++) {
    std::string skill_name = race_skill_name( Race_skill(i) );
    skill_name = capitalize_all_words(skill_name);
    if (skill_name.length() > 18) {
      skill_name = skill_name.substr(0, 18);
    }
    ret << "<link=" << skill_name << ">" << skill_name << "</link>:  ";
    for (int n = 0; n < (19 - skill_name.length()); n++) {
      ret << " ";
    }
    std::string color_tag = "white";
    switch (skill_level[i]) {
      case 1: color_tag = "red";      break;
      case 2: color_tag = "yellow";   break;
      case 4: color_tag = "ltblue";   break;
      case 5: color_tag = "ltgreen";  break;
    }
    ret << "<c=" << color_tag << ">" << skill_level[i] << "<c=/>" << std::endl;
  }

  ret << std::endl;
  ret << "<c=pink>Race relations:<c=/>" << std::endl;
// Start at 1 to skip RACE_NULL
  for (int i = 1; i < RACE_MAX; i++) {
    std::string race_name = capitalize( Race_data[i]->plural_name );
    ret << "<c=white>" << race_name << ":<c=/>";
    int extra_space = (relations[i] >= 0 ? 1 : 0);
    if (race_name.length() < 15) {
      for (int n = 0; n < (extra_space + 15 - race_name.length()); n++) {
        ret << " ";
      }
    }
    std::string color_tag = "ltgray";
    if (relations[i] <= -3) {
      color_tag = "red";
    } else if (relations[i] < 0) {
      color_tag = "yellow";
    } else if (relations[i] >= 3) {
      color_tag = "ltgreen";
    } else if (relations[i] > 0) {
      color_tag = "ltblue";
    }
    ret << "<c=" << color_tag << ">" << relations[i] << "<c=/>" << std::endl;
  }

// TODO: Incude map_type_travel_cost

  ret << std::endl;
  ret << description;

  return ret.str();
}

Race_skill lookup_race_skill(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < SKILL_MAX; i++) {
    Race_skill ret = Race_skill(i);
    if (name == no_caps( trim( race_skill_name(ret) ) ) ) {
      return ret;
    }
  }
  return SKILL_NULL;
}

std::string race_skill_name(Race_skill skill)
{
  switch (skill) {
    case SKILL_NULL:          return "NULL";
    case SKILL_FARMING:       return "farming";
    case SKILL_HUNTING:       return "hunting";
    case SKILL_LIVESTOCK:     return "livestock";
    case SKILL_MINING:        return "mining";
    case SKILL_FORESTRY:      return "forestry";
    case SKILL_CONSTRUCTION:  return "construction";
    case SKILL_TRADE:         return "trade";
    case SKILL_ESPIONAGE:     return "espionage";
    case SKILL_MAGIC:         return "magic";
    case SKILL_EARTH_MAGIC:   return "earth magic";
    case SKILL_WATER_MAGIC:   return "water magic";
    case SKILL_AIR_MAGIC:     return "air magic";
    case SKILL_FIRE_MAGIC:    return "fire magic";
    case SKILL_LIFE_MAGIC:    return "life magic";
    case SKILL_DEATH_MAGIC:   return "death magic";
    case SKILL_MAX:           return "BUG - SKILL_MAX";
    default:                  return "BUG - Unnamed Race_skill";
  }
  return "BUG - Escaped race_skill_name() switch";
}
