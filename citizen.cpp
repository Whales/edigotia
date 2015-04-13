#include "citizen.h"
#include "city.h"
#include "stringfunc.h"
#include "rng.h"
#include "window.h"     // For debugmsg()
#include <sstream>

Citizens::Citizens()
{
  count         = 0;
  employed      = 0;
  wealth        = 0;
  tax_morale    = 0;
  morale_points = 0;
  starvation    = 0;
}

Citizens::~Citizens()
{
}

std::string Citizens::save_data()
{
  std::stringstream ret;

  ret << type << " ";
  ret << count << " ";
  ret << employed << " ";
  ret << wealth << " ";
  ret << tax_morale << " ";
  ret << morale_points << " ";
  ret << starvation << std::endl;
  ret << consumption.size() << " ";
  for (std::map<Resource,int>::iterator it = consumption.begin();
       it != consumption.end();
       it++) {
    ret << int(it->first) << " " << it->second << " ";
  }
  ret << std::endl;
  ret << morale_modifiers.size() << " ";
  for (int i = 0; i < morale_modifiers.size(); i++) {
    ret << morale_modifiers[i].save_data() << " ";
  }

  return ret.str();
}

bool Citizens::load_data(std::istream& data)
{
  int tmptype;
  data >> tmptype;
  if (tmptype >= CIT_MAX) {
    debugmsg("Citizens loaded type of %d (Range is 1 to %d)",
             tmptype, CIT_MAX - 1);
    return false;
  }
  type = Citizen_type(tmptype);
  data >> count >> employed >> wealth >> tax_morale >> morale_points >>
          starvation;

  int num_consumption;
  data >> num_consumption;
  for (int i = 0; i < num_consumption; i++) {
    int tmpres, tmpnum;
    data >> tmpres >> tmpnum;
    if (tmpres <= 0 || tmpres >= RES_MAX) {
      debugmsg("Citizens loaded possession of resource %d (range is 1 to %d)",
               tmpres, RES_MAX - 1);
      return false;
    }
    consumption[Resource(tmpres)] = tmpnum;
  }

  int num_moralemod;
  data >> num_moralemod;
  for (int i = 0; i < num_moralemod; i++) {
    Morale_modifier tmpmod;
    if (!tmpmod.load_data(data)) {
      debugmsg("Citizens failed to load Morale_modifier.");
      return false;
    }
    morale_modifiers.push_back(tmpmod);
  }

  return true;
}

int Citizens::get_unemployed()
{
  if (employed >= count) {
    return 0;
  }
  return count - employed;
}

int Citizens::get_income()
{
// Obviously, don't include income from government jobs (i.e. areas/buildings)
  int ret = 0;

// Idle citizens do produce income though!
  int idle = get_unemployed();
  ret += (idle * citizen_idle_income(type)) / 100;

// TODO: Other sources of income?

  return ret;
}

int Citizens::get_morale_percentage()
{
  if (count == 0) {
    return 0;
  }
  int ret = tax_morale;
  for (int i = 0; i < morale_modifiers.size(); i++) {
    ret += morale_modifiers[i].amount / 10;
  }

  return ret;
}

int Citizens::get_starvation_chance()
{
  if (count == 0) {
    return 0;
  }
/* Starvation / count gives us the average number of days that each citizen has
 * been without food.  We then square that and divide by 3 to get the chance
 * that EACH citizen much pass to survive.  Thus if we've been 5 days without
 * food, we have an 8% chance to die; if it's been 10 days we have a 33% chance
 * to die; and once it's been 18 days we have a 100% chance to die (meaning that
 * our entire population will be wiped out).
 * TODO: It would be nice to take our health into account here!
 */
  int ret = starvation / count;
  ret = (ret * ret) / 3;
  return (ret > 100 ? 100 : ret);
}

// colorize defaults to false
std::vector<std::string> Citizens::get_morale_mods(bool colorize)
{
  std::vector<std::string> ret;
  for (int i = 0; i < morale_modifiers.size(); i++) {
    if (colorize) {
      std::stringstream text;
      if (morale_modifiers[i].amount < 0) {
        text << "<c=ltred>";
      } else if (morale_modifiers[i].amount > 0) {
        text << "<c=green>";
      } else {
        text << "<c=dkgray>";
      }
      text << capitalize( morale_modifiers[i].get_name() ) << "<c=/>";
      ret.push_back( text.str() );
    } else {
      ret.push_back( capitalize( morale_modifiers[i].get_name() ) );
    }
  }
  return ret;
}

std::vector<int> Citizens::get_morale_mod_amounts()
{
  std::vector<int> ret;
  for (int i = 0; i < morale_modifiers.size(); i++) {
    ret.push_back( morale_modifiers[i].amount / 10 );
  }
  return ret;
}

void Citizens::reset()
{
  count         = 0;
  wealth        = 0;
  employed      = 0;
  tax_morale    = 0;
  starvation    = 0;
  morale_points = 0;

  consumption.clear();
  morale_modifiers.clear();
}

void Citizens::pick_luxuries(City* city)
{
  if (!city) {
    debugmsg("Citizens::pick_luxuries(NULL) called!");
    return;
  }

// Start at 1 to skip LUX_NULL
  for (int i = 1; i < LUX_MAX; i++) {
    Luxury_type lux_type = Luxury_type(i);
    std::map<Resource,int> luxuries = city->get_luxuries(lux_type);

    if (!luxuries.empty()) {
// A list of chances to set our chosen luxury to each resource
      int chance[RES_MAX];
      int total_chance = 0;
      for (int n = 0; n < RES_MAX; n++) {
        Resource res = Resource(n);
        chance[n] = luxuries[res];
        total_chance += luxuries[res];
      }
      bool change = (total_chance > 0); // Only re-roll if chance == true
// If luxury_demands is already set, there's a strong chance to set change to
// false, unless the availability of other resources is much higher.
      if (change && luxury_demands[lux_type] != RES_NULL) {
        Resource current = luxury_demands[lux_type];
/* So if the currently-demanded resource is half of the available resources,
 * we only change 1 in 275 rolls; if it's only 1% of the available resources,
 * we change 1 in 30 rolls.  We include "25 +" so that even if the currently-
 * demanded resource is unavailable, we only change 1 in 25 rolls.
 */
        if (!one_in(25 + 500 * (chance[current] / total_chance))) {
          change = false;
        }
      }

      if (change) { // Pick a new luxury to demand!  Probably!
        int roll = rng(1, total_chance);
        for (int n = 0; roll > 0 && n < RES_MAX; n++) {
          roll -= chance[n];
          if (roll <= 0) {
            luxury_demands[lux_type] = Resource(n);
          }
        }
      }

    } // if (!luxuries.empty())

  } // for (int i = 1; i < LUX_MAX; i++)

}

void Citizens::consume_luxuries(City* city)
{
  if (!city) {
    debugmsg("Citizens::consume_luxuries(NULL) called!");
    return;
  }

  std::vector<Resource> to_erase;
  for (std::map<Resource,int>::iterator it = consumption.begin();
       it != consumption.end();
       it++) {
    Resource res = it->first;
    int amount = it->second;
    if (city->expend_resource(res, amount)) {
      Resource_datum* res_dat = Resource_data[res];
      Luxury_type lux_type = res_dat->luxury_type;
      int res_demand = (res_dat->demand * count) / 100;
      int res_morale = res_dat->morale;
      if (lux_type != LUX_NULL && luxury_demands[lux_type] != RES_NULL &&
          luxury_demands[lux_type] != res) {
        res_demand *= .5;
        res_morale *= .4;
      }
      int morale_gain = 0;
// If this luxury is not the one we want, then it only gives 40% the morale!
      if (it->second >= res_demand) {
        morale_gain = res_morale;
      } else if (it->second > 0) {
// If we don't meet demand, give partial morale.
        morale_gain = (res_morale * consumption[res]) / (res_demand);
      }
      add_morale_modifier(MORALE_MOD_LUXURY, morale_gain * 10, res);

    } else {  // city didn't have enough resources!
      to_erase.push_back(it->first);
    }
  } // for (std::map<Resource,int>::iterator it = consumption.begin() ... )

// Erase anything we've slated for erasure
  for (int i = 0; i < to_erase.size(); i++) {
    consumption.erase( to_erase[i] );
  }
}

void Citizens::decrease_morale_mods()
{
  for (int i = 0; i < morale_modifiers.size(); i++) {
    if (morale_modifiers[i].amount > 0) {
      morale_modifiers[i].amount--;
    } else if (morale_modifiers[i].amount < 0) {
      morale_modifiers[i].amount++;
    }
// Luxuries are always a single-turn modifier!
    if (morale_modifiers[i].type == MORALE_MOD_LUXURY ||
        morale_modifiers[i].type == MORALE_MOD_BUILDING ||
        morale_modifiers[i].amount == 0) {
      morale_modifiers.erase( morale_modifiers.begin() + i);
      i--;
    }
  }
}

int Citizens::add_possession(Resource_amount res)
{
  return add_possession(res.type, res.amount);
}

int Citizens::add_possession(Resource res, int amount)
{
  int had = 0;
  Resource_datum* res_dat = Resource_data[res];
  int limit = (count * res_dat->demand) / 100;
  if (consumption.count(res)) {
    had = consumption[res];
  }
  if (had + amount > limit) {
    int left = amount - (limit - had);
    consumption[res] = limit;
    return left;
  }
  consumption[res] += amount;
  return 0;
}

void Citizens::add_morale_modifier(Morale_mod_type type, int amount)
{
  add_morale_modifier(type, amount, RES_NULL, "");
}

void Citizens::add_morale_modifier(Morale_mod_type type, int amount,
                                   Resource luxury)
{
  add_morale_modifier(type, amount, luxury, "");
}

void Citizens::add_morale_modifier(Morale_mod_type type, int amount,
                                   std::string term)
{
  add_morale_modifier(type, amount, RES_NULL, term);
}

void Citizens::add_morale_modifier(Morale_mod_type type, int amount,
                                   Resource luxury, std::string term)
{
  if (amount == 0 || type == MORALE_MOD_NULL || type == MORALE_MOD_MAX ||
      count == 0) {
    return;
  }
// Check if we have a modifier of that type.
  for (int i = 0; i < morale_modifiers.size(); i++) {
    if (morale_modifiers[i].type    == type &&
        morale_modifiers[i].luxury  == luxury &&
        morale_modifiers[i].term    == term) {
      morale_modifiers[i].amount += amount;
      return;
    }
  }
  Morale_modifier mod(type, amount, luxury, term);
  morale_modifiers.push_back(mod);
}

void Citizens::add_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
  count += amount;
  morale_points += 50 * amount; // Each one comes into the world with 50% morale
}

void Citizens::remove_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
// get_morale_percentage() basically returns the morale points per citizen.
// When we remove citizens, we need to remove their morale points too.
  morale_points -= amount * get_morale_percentage();
// Ditto starvation.
  starvation    -= amount * get_starvation_chance();
  count -= amount;
  if (count < 0) {
    morale_points = 0;
    count = 0;
  }
}

Citizen_type lookup_citizen_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CIT_MAX; i++) {
    Citizen_type ret = Citizen_type(i);
    if (name == citizen_type_name(ret)) {
      return ret;
    }
  }
  return CIT_NULL;
}

// plural defaults to false
std::string citizen_type_name(Citizen_type type, bool plural)
{
  switch (type) {
    case CIT_NULL:      return "NULL";
    case CIT_SLAVE:     return (plural ? "slaves" : "slave");
    case CIT_PEASANT:   return (plural ? "peasants" : "peasant");
    case CIT_MERCHANT:  return (plural ? "merchants" : "merchant");
    case CIT_BURGHER:   return (plural ? "burghers" : "burgher");
    case CIT_MAX:       return "BUG - CIT_MAX";
    default:            return "Unnamed Citizen_type";
  }
  return "BUG - Escaped citizen_type_name() switch";
}

Morale_mod_type lookup_morale_mod_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < MORALE_MOD_MAX; i++) {
    Morale_mod_type ret = Morale_mod_type(i);
    if (name == no_caps( morale_mod_type_name(ret) ) ) {
      return ret;
    }
  }
  return MORALE_MOD_NULL;
}

std::string morale_mod_type_name(Morale_mod_type type)
{
  switch (type) {
    case MORALE_MOD_NULL:     return "NULL";
    case MORALE_MOD_LUXURY:   return "luxury";
    case MORALE_MOD_BUILDING: return "building";
    case MORALE_MOD_FESTIVAL: return "festival";
    case MORALE_MOD_HUNGER:   return "hunger";
    case MORALE_MOD_DEATH:    return "death";
    case MORALE_MOD_MAX:      return "BUG - MORALE_MOD_MAX";
    default:                  return "BUG - Unnamed Morale_mod_type";
  }
  return "BUG - Escaped morale_mod_type_name() switch!";
}

std::string Morale_modifier::get_name()
{
  if (type == MORALE_MOD_LUXURY) {
    std::stringstream ret;
    ret << "Luxury: " << Resource_data[luxury]->name;
    return ret.str();
  } else if (type == MORALE_MOD_BUILDING) {
    std::stringstream ret;
    ret << "Building: " << term;
    return ret.str();
  }
  return morale_mod_type_name(type);
}

std::string Morale_modifier::save_data()
{
  std::stringstream ret;

  ret << int(type) << " ";
  ret << amount << " ";

  return ret.str();
}

bool Morale_modifier::load_data(std::istream& data)
{
  int tmptype;
  data >> tmptype >> amount;

  if (tmptype <= 0 || tmptype >= MORALE_MOD_MAX) {
    debugmsg("Morale_modifier loaded type %d (range is 1 to %d).",
             tmptype, MORALE_MOD_MAX - 1);
    return false;
  }

  type = Morale_mod_type(tmptype);

  return true;
}

// This is per 100 citizens!
int citizen_food_consumption(Citizen_type type)
{
  switch (type) {
    case CIT_NULL:      return   0;
    case CIT_SLAVE:     return  50;
    case CIT_PEASANT:   return 100;
    case CIT_MERCHANT:  return 140;
    case CIT_BURGHER:   return 180;
    case CIT_MAX:       return   0;
    default:            return   0;
  }
  return 0;
}

// This is per 100 citizens!
int citizen_idle_income(Citizen_type type)
{
  switch (type) {
    case CIT_NULL:      return   0;
    case CIT_SLAVE:     return   0;
    case CIT_PEASANT:   return  30;
    case CIT_MERCHANT:  return  80;
    case CIT_BURGHER:   return 250;
    case CIT_MAX:       return   0;
    default:            return   0;
  }
  return 0;
}

std::string cause_of_death_text(Cause_of_death cause, bool plural)
{
  switch (cause) {
    case DEATH_NULL:        return "NULL";
    case DEATH_UNKNOWN:     return "died mysteriously";
    case DEATH_STARVATION:  return "starved to death";

    case DEATH_HUNTING:
      if (plural) {
        return "were killed while hunting";
      }
      return "was killed while hunting";

    case DEATH_MAX:         return "BUG - DEATH_MAX";
    default:                return "Unnamed Cause_of_death";
  }
  return "BUG - Escaped cause_of_death_name() switch!";
}
