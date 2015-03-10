#ifndef _PLAYER_CITY_H_
#define _PLAYER_CITY_H_

#include "city.h"

#include "area.h"
#include "building.h"
#include "citizen.h"
#include "map.h"
#include "military.h"
#include "race.h"
#include "resource.h"
#include "game.h"
#include "date.h"
#include "city_achievement.h"
#include "city_type.h"
#include "world.h"

#include "window.h"
#include "cuss.h"

#include <vector>
#include <string>
#include <map>
#include <istream>

enum Message_type
{
  MESSAGE_NULL = 0,
  MESSAGE_MINOR,    // Player MIGHT want to take action on this
  MESSAGE_MAJOR,    // Player will almost certainly want to take action on this
  MESSAGE_URGENT,   // Player MUST take action on this!
  MESSAGE_UNLOCK,    // We've unlocked a new area, building, or other feature
  MESSAGE_MAX
};

// Color-coding for the different message types.
nc_color message_type_color(Message_type type);

struct Message
{
  Message(Message_type MT = MESSAGE_MINOR, std::string T = "") :
    type (MT), text (T) { }

  std::string save_data();
  bool load_data(std::istream& data);

  Message_type type;
  std::string text;
  Date date;
};

class Player_city : public City
{
public:
  Player_city();
  virtual ~Player_city();

  virtual bool is_player_city() { return true; }

  virtual std::string save_data();
  virtual bool load_data(std::istream& data);

// Starting functions
  bool place_keep();
  void pick_race();
  void set_name();
  virtual void start_new_city();
// Set our Map_seen to see all tiles in our kingdom, plus some outside.
  void set_starting_tiles_seen();
  void mark_nearby_tiles_seen(int range);

// Display output
  glyph get_glyph();
  void draw_map(cuss::element* e_draw, Point sel = Point(-1, -1),
                bool radius_limited = false, bool only_terrain = false);

// Standard turn functions
  virtual void do_turn();
  void handle_livestock();
  void feed_citizens(); // Handles starvation, too, if it occurs

  void check_unlockables();// Go through all unlockable items & see if we get it

// Mutators
  Area_queue_status add_area_to_queue(Area_type type, Point location);
  Area_queue_status add_area_to_queue(Area area);

  void add_open_area(Area area);  // Move it from queue to areas.
  void destroy_area_at(int x, int y);
  void destroy_area_at(Point pos);

  Building_queue_status add_building_to_queue(Building_type type);
  Building_queue_status add_building_to_queue(Building building);
  bool cancel_queued_building(int index);
  void add_open_building(Building building);  // Move it from queue to buildings

  bool employ_citizens(Citizen_type type, int amount, Building* job_site);
  bool fire_citizens  (Citizen_type type, int amount, Building* job_site);
  void kill_citizens  (Citizen_type type, int amount,
                       Cause_of_death cause = DEATH_UNKNOWN,
                       std::string cause_text = "");

// The variadic arguments allow for printf-style substitution.
  void add_message(Message_type type, std::string text, ...);

// Data accessor functions

// Message-related functions

// The vector returned always has one element for each message type
// It goes through the unread messages and adds 1 for each messages of that type
  std::vector<int> get_unread_message_count();


// Map-related functions
  bool inside_radius(int x, int y);
  bool inside_radius(Point p);

  Area* area_at(int x, int y);
  Area* area_at(Point p);

  std::string get_map_info(Point p);


// Citizens/population/housing-related functions
  int get_total_population(Citizen_type type = CIT_NULL);
// Returns the number of citizens in the given class currently without a job.
  int get_unemployed_citizens(Citizen_type type = CIT_NULL);

  int get_total_housing(Citizen_type type = CIT_NULL);

  int get_military_count();
  int get_military_supported();

// Gain this many birth points each turn.  Based mostly on population.  When our
// birth points reaches 100, a new citizen is born.
  int get_daily_birth_points();
/* The number of citizens of the class below this required for a citizen of this
 * class.  For instance, if get_required_ratio(CIT_MERCHANT) is 20, then we need
 * 20 peasants before a merchant can be born.
 */
  int get_required_ratio(Citizen_type cit_type);
  int get_population_cap(Citizen_type cit_type);  // Based on get_required_ratio
/* The morale of the class below this required for a citizen of this class.  For
 * instance, if get_required_morale(CITY_MERCHANT) is 75, then our peasants'
 * morale level must be at least 75 before a merchant can be born.
 */
  int get_required_morale(Citizen_type cit_type);
/* When a new citizen is born, we need to choose its type; this function returns
 * the chance that its type will be cit_type.  The chances are independant of
 * each other; they may total to more than 100%.
 */
  int get_chance_to_birth(Citizen_type cit_type);
  void birth_citizens(int num = 1); // Picks a Citizen_type and adds new citizen


// Building/Area-related functions
  std::vector<Building*> get_all_buildings();
  std::vector<Building*> get_pure_buildings(); // i.e. those not of an Area
// get_employers() returns buildings with at least 1 worker of the type passed.
// If it's passed CIT_NULL (or nothing), it ignores the Citizen_type.
  std::vector<Building*> get_employers(Citizen_type cit_type = CIT_NULL);

// If these are passed BUILD_NULL/AREA_NULL (or aren't passed a parameter at
// all) they will return the sum total of ALL buildings/areas.
  int get_number_of_buildings(Building_type type = BUILD_NULL);
  int get_number_of_areas    (Area_type     type = AREA_NULL);
  int get_buildings_supported();  // Total of buildings_supported of all areas
  int get_total_maintenance();
  int get_fields_worked();
  int get_empty_fields();
  int get_shafts_worked();
  int get_free_shafts();


// Resource-related functions
  int get_resource_production(Resource res);  // From all sources
// Total amount produced daily, including from buildings and imports
  virtual int get_gross_resource_production (Resource res);


  // General finance
  int get_total_wages(Citizen_type type = CIT_NULL);
  int get_military_expense();

  void set_tax_rate(Citizen_type type, int rate);
  int get_low_tax_rate (Citizen_type type);
  int get_high_tax_rate(Citizen_type type);
  int get_taxes(Citizen_type type = CIT_NULL);

  int get_corruption_percentage();
  int get_corruption_amount();

  // Crops & food
  int get_food_production();
  std::vector<Crop_amount> get_crops_grown();

  // Mining
  int get_amount_mined(Mineral mineral = MINERAL_NULL);
  std::map<Mineral,int> get_minerals_mined();
  std::map<Mineral,int> get_minerals_used();

  // Animals & hunting
  Animal_action get_hunting_action(Animal animal);
  void set_hunting_action(Animal animal, Animal_action action);
  void do_hunt(Area* hunting_camp);
  void kill_animals(Animal animal, int amount, Point pos = Point(-1, -1));

  // Livestock
  int get_livestock_total();  // Number of animals we have
  int get_livestock_capacity(); // Amount of RES_LIVESTOCK_SPACE from areas

// Trade-related functions
  int get_import(Resource res);
  int get_export(Resource res);


// Misc functions
  bool meets_achievement(City_achievement achievement);


// *** DATA ***
  int tax_rate[CIT_MAX];

  std::vector<Military_unit> units_stationed;

  int radius;

  bool area_unlocked[AREA_MAX];
  bool building_unlocked[BUILD_MAX];

  std::vector<Building> buildings;
  std::vector<Building> building_queue;
  std::vector<Area> areas;
  std::vector<Area> area_queue;

  std::map<Animal,int> hunt_kills;
  int hunt_record_days; // How many days since we started recording hunt food?
  int hunt_record_food; // The amount of food gained from hunting in this time

  int unread_messages;
  std::vector<Message> messages;
  bool show_hunting_messages;
  bool show_livestock_messages;

// Map_seen is defined in world.h; record of which world tiles we've seen
  Map_seen world_seen;

private:
  int birth_points; // We gain these each turn; at 100, a citizen is born.

  Animal_action hunting_action[ANIMAL_MAX];
};


#endif
