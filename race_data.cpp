#include "race.h"

Race_datum* Race_data[RACE_MAX];


#define _race(n) \
  cur_id = (n)

#define _name(s) \
  Race_data[cur_id]->name = (s)

#define _plural_name(s) \
  Race_data[cur_id]->plural_name = (s)

#define _adjective(s) \
  Race_data[cur_id]->adjective = (s)

#define _color(c) \
  Race_data[cur_id]->color = (c)

#define _kingdom_color(c) \
  Race_data[cur_id]->kingdom_colors.push_back( (c) )

#define _map_type(m, i) \
  Race_data[cur_id]->map_type_value[(m)] = (i)

#define _travel_cost(m, i) \
  Race_data[cur_id]->map_type_travel_cost[(m)] = (i)

#define _cluster_size(a, b) \
  Race_data[cur_id]->cluster_min = (a); Race_data[cur_id]->cluster_max = (b)

#define _city_size(t, a, b) \
  Race_data[cur_id]->city_size_min[(t)] = (a); \
  Race_data[cur_id]->city_size_max[(t)] = (b); \

/* We use two special characters in city names - '*' and '?'
 *
 * '*' means "Repeat the previous consonant if it's not preceded by a
 * consonant."  e.g. "Wil*iams" => "Williams" but "Wilm*iams" => "Wilmiams"
 *
 * '?' means "If the previous characters AND the following character are
 * identical, remove one."  e.g. "Wis?ster" => "Wister"
 *
 * '!' means "If the previous character is a consonant, remove the following
 * character."  e.g. "Tat!bol" => "Tatol" but "Ta!bol" => "Tabol"
 */

#define _city_name_start(...) \
  Race_data[cur_id]->add_city_names("start", __VA_ARGS__, 0)

#define _city_name_middle(...) \
  Race_data[cur_id]->add_city_names("middle", __VA_ARGS__, 0)

#define _city_name_end(...) \
  Race_data[cur_id]->add_city_names("end", __VA_ARGS__, 0)

#define _base_combat(n) \
  Race_data[cur_id]->base_combat = (n)

#define _hp(n) \
  Race_data[cur_id]->hp = (n)

#define _food_consumption(n) \
  Race_data[cur_id]->food_consumption = (n)

#define _warlikeness(n) \
  Race_data[cur_id]->warlikeness = (n)

#define _life_value(n) \
  Race_data[cur_id]->life_value = (n)

#define _starting_population(t, n) \
  Race_data[cur_id]->starting_population[ (t) ] = (n)

#define _starting_resources(t, n) \
  Race_data[cur_id]->starting_resources[ (t) ] = (n)

#define _birth_rate(t, n) \
  Race_data[cur_id]->birth_rate[ (t) ] = (n)

#define _citizen_ratio(t, n) \
  Race_data[cur_id]->citizen_ratio[ (t) ] = (n)

#define _morale_requirement(t, n) \
  Race_data[cur_id]->morale_requirement[ (t) ] = (n)

#define _low_tax_rate(t, n) \
  Race_data[cur_id]->low_tax_rate[ (t) ] = (n)

#define _high_tax_rate(t, n) \
  Race_data[cur_id]->high_tax_rate[ (t) ] = (n)

#define _relations(r, n) \
  Race_data[cur_id]->relations[ (r) ] = (n)

#define _skill(t, n) \
  Race_data[cur_id]->skill_level[ (t) ] = (n)

#define _description(s) \
  Race_data[cur_id]->description = (s)

void init_races()
{
  for (int i = 0; i < RACE_MAX; i++) {
    Race_data[i] = new Race_datum();
    Race_data[i]->uid = i;
  }

  int cur_id = 0;

  _race(RACE_NULL);
    _name("NULL");

  _race(RACE_HUMAN);
    _name("human");
    _plural_name("humans");
    _adjective("human");

    _color(c_white);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_blue);
    _kingdom_color(c_cyan);

    _map_type(MAP_BASIN,       100);
    _map_type(MAP_PLAINS,       90);
    _map_type(MAP_CANYON,       50);
    _map_type(MAP_FOREST,       40);
    _map_type(MAP_FOOTHILLS,    10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_JUNGLE,      -30);
    _map_type(MAP_SWAMP,       -20);
    _map_type(MAP_MOUNTAINOUS,  -5);

    _cluster_size(3, 4);
    _city_size(CITY_TYPE_CITY,     100, 1200);
    _city_size(CITY_TYPE_DUCHY,    800, 4000);
    _city_size(CITY_TYPE_CAPITAL, 2000, 8000);

    _city_name_start("al", "am", "an", "bir", "birm", "cam", "can", "car",
                     "chel", "dan", "dar", "dun", "ed", "el", "ex", "fel",
                     "hil", "hut", "lan", "lat", "nor", "port", "quart", "ruth",
                     "south", "tel", "wan", "wil", "wilm", "yor", "york");

    _city_name_middle("", "", "a", "*a", "al", "*al", "*an", "*as", "ca",
                      "cing", "*er", "*ers", "e", "e", "es", "es", "*es",
                      "*ial", "*iam", "*iams", "*ing", "ing", "*is", "*is",
                      "*or", "*ow", "*os", "ser", "sing", "ter", "*um", "us");

    _city_name_end("", "", "", "", "", "borough", "burg", "burg", "burg",
                   "bury", "bury", "by", "by", "cas", "caster", "don", "*ers",
                   "*ex", "field", "field", "ford", "forth", "gale", "ham",
                   "ham", "herst", "ia", "*ia", "in", "*in", "ire", "kirk",
                   "mouth", "or", "over", "pool", "?s", "?s", "?s", "?s",
                   "?ston", "ter", "tin", "ton", "ton", "ton", "vale", "ville",
                   "ville");

    _base_combat(10);
    _hp(100);
    _food_consumption(100);
    _warlikeness(-2);
    _life_value(3);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    2500);
    _starting_resources (RES_STONE,   2500);

    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  90);
    _birth_rate         (CIT_BURGHER,   80);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   70);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _relations(RACE_HUMAN,    3);
    _relations(RACE_ELF,      2);
    _relations(RACE_DWARF,    1);
    _relations(RACE_GOBLIN,  -1);
    _relations(RACE_OGRE,     2);
    _relations(RACE_TROLL,   -3);
    _relations(RACE_HALFLING, 2);
    _relations(RACE_RAKSHASA, 1);
    _relations(RACE_NAGA,    -2);

    _skill(SKILL_FARMING,       4);
    _skill(SKILL_HUNTING,       3);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        3);
    _skill(SKILL_FORESTRY,      3);
    _skill(SKILL_CONSTRUCTION,  3);
    _skill(SKILL_TRADE,         3);
    _skill(SKILL_ESPIONAGE,     3);
    _skill(SKILL_MAGIC,         2);
    _skill(SKILL_EARTH_MAGIC,   3);
    _skill(SKILL_WATER_MAGIC,   3);
    _skill(SKILL_AIR_MAGIC,     3);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    3);
    _skill(SKILL_DEATH_MAGIC,   3);

    _description("\
Humans are a versatile <link=race>race</link>, and can inhabit almost any part \
of the world.  They are notably average in many senses.  They give birth \
fairly quickly, though they do not live particularly long.  They are \
reasonably skilled in all areas.  <link=farming>Farming</link> is a particular \
strength for humans, and they are most at home living on flat \
<link=plains>plains</link>.\n\
Humans are natural <link=kingdom>kingdom</link>-builders, and most prefer to \
live in established cities rather thanon their own.  Their loyalty to their \
<link=monarch>king or queen</link> is great, but it is not unknown for humans \
to backstab their way to the top.\n\
Humans are a generally friendly race, and get along well with most other \
races.  True to their generally vain nature, they get along best with those \
who resemble them; in particular, the elves and the halflings.\n\
Humans are not particularly in touch with the art of <link=magic>magic</link>, \
and have no affinity for any particular <link=magical school>school</link>.\
");


  _race(RACE_ELF);
    _name("elf");
    _plural_name("elves");
    _adjective("elvish");

    _color(c_ltgreen);
    _kingdom_color(c_green);
    _kingdom_color(c_cyan);
    _kingdom_color(c_brown);
/*
    _kingdom_color(c_ltgreen);
    _kingdom_color(c_ltcyan);
*/

    _map_type(MAP_FOREST,      100);
    _map_type(MAP_JUNGLE,       45);
    _map_type(MAP_BASIN,        30);
    _map_type(MAP_SWAMP,        15);
    _map_type(MAP_PLAINS,       10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_MOUNTAINOUS, -30);

    _travel_cost(MAP_FOREST,      25);
    _travel_cost(MAP_SWAMP,      100);
    _travel_cost(MAP_JUNGLE,     100);

    _cluster_size(2, 4);
    _city_size(CITY_TYPE_CITY,      80,  900);
    _city_size(CITY_TYPE_DUCHY,    600, 3500);
    _city_size(CITY_TYPE_CAPITAL, 1600, 6000);

// Should end with a vowel or "lnrs"
    _city_name_start("al", "bi", "bir", "bri", "dil", "e", "el", "en", "er",
                     "ere", "es", "fi", "for", "il", "in", "is", "kor", "li",
                     "lo", "me", "mi", "mir", "na", "ri", "ta", "to", "tol",
                     "va", "val", "vi");

// Should end with a vowel or "lnr"
    _city_name_middle("", "", "", "", "", "", "allo", "an", "he",
                      "i", "la", "lo", "lum", "lun", "ni", "ny", "rio", "ro",
                      "ta", "the", "tir", "thro", "va", "ver", "ya");

    _city_name_end("a", "bar", "bel", "dell", "dell", "dell", "dolin", "e",
                   "ion", "le", "lest", "lin", "lin", "lo", "lon", "lond",
                   "lond", "mar", "men", "min", "o", "on", "os", "se", "th",
                   "the", "thon", "thrion", "thrond", "vin", "vin", "vin");

    _base_combat(9);
    _hp(90);
    _food_consumption(95);
    _warlikeness(-2);
    _life_value(4);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    3000);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,   8);
    _citizen_ratio      (CIT_BURGHER,   12);
    _morale_requirement (CIT_MERCHANT,  65);
    _morale_requirement (CIT_BURGHER,   75);
    _low_tax_rate       (CIT_PEASANT,   15);
    _low_tax_rate       (CIT_MERCHANT,  10);
    _low_tax_rate       (CIT_BURGHER,    5);
    _high_tax_rate      (CIT_PEASANT,   75);
    _high_tax_rate      (CIT_MERCHANT,  75);
    _high_tax_rate      (CIT_BURGHER,   65);

    _relations(RACE_HUMAN,     1);
    _relations(RACE_ELF,       3);
    _relations(RACE_DWARF,     1);
    _relations(RACE_ORC,      -2);
    _relations(RACE_GOBLIN,   -2);
    _relations(RACE_OGRE,     -1);
    _relations(RACE_TROLL,    -3);
    _relations(RACE_GNOME,    -1);
    _relations(RACE_MINOTAUR, -1);
    _relations(RACE_RAKSHASA,  2);
    _relations(RACE_NAGA,      2);

    _skill(SKILL_FARMING,       4);
    _skill(SKILL_HUNTING,       5);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      5);
    _skill(SKILL_CONSTRUCTION,  2);
    _skill(SKILL_TRADE,         2);
    _skill(SKILL_ESPIONAGE,     4);
    _skill(SKILL_MAGIC,         4);
    _skill(SKILL_EARTH_MAGIC,   3);
    _skill(SKILL_WATER_MAGIC,   3);
    _skill(SKILL_AIR_MAGIC,     2);
    _skill(SKILL_FIRE_MAGIC,    2);
    _skill(SKILL_LIFE_MAGIC,    4);
    _skill(SKILL_DEATH_MAGIC,   2);

    _description("\
Elves are a tall, slender, nimble <link=race>race</link> of <link=forest>forest\
</link>-dwellers.  Elves have a great affinity for plants, and fare \
excellently in forests, <link=jungle>jungles</link> and <link=swamp>swamps\
</link> but poorly in <link=desert>deserts</link> or <link=mountain>mountains\
</link>.  They are skilled woodsmen; however, elves consider it immoral to \
fell a living tree, and they can not build sawmills.  Instead, elves can build \
a <link=sacred grove>Sacred Grove</link>, an enchanted copse of trees which \
willingly give up their branches for use.  Elves are also legendary for their \
finely crafted <link=bow>bows</link>, and prowess at <link=hunting>hunting\
</link>.\n\
Elves have long lives, but are slow to give birth.  Many prefer to live as \
<link=homesteader>loners</link> deep in the forest, and it may be difficult to \
lure them into city living.  They are noble warriors with a strong sense of \
duty to their <link=kingdom>kingdom</link>, but they are also careful \
thinkers, more prone to finding diplomatic solutions than going to \
<link=war>war</link>.\n\
Elves are a rather haughty race, and tend to look down on other races, while \
not actually harboring any ill will.  They have a particular distaste for \
those races for whom destruction, either careless or wanton, lies in their \
nature, such as <link=orcs>orcs</link> or <link=goblins>goblins</link>.  The \
exceptions to this rule are <link=humans>humans</link> and \
<link=dwarves>dwarves</link>, with whom the elves share a history of mutual \
respect.\n\
Elves are skilled <link=magic>spellcasters</link>, particularly in the \
<link=magical school>school</link> of <link=life magic>life</link>.\
");


  _race(RACE_DWARF);
    _name("dwarf");
    _plural_name("dwarves");
    _adjective("dwarvish");

    _color(c_brown);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_red);
    _kingdom_color(c_brown);
/*
    _kingdom_color(c_ltred);
    _kingdom_color(c_yellow);
*/

    _map_type(MAP_CANYON,      100);
    _map_type(MAP_MOUNTAINOUS,  90);
    _map_type(MAP_FOOTHILLS,    70);
    _map_type(MAP_WASTELAND,    10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_JUNGLE,      -25);
    _map_type(MAP_PLAINS,      -20);

// Dwarves are squat... and thus travel slower on almost all terrain!
// However they're great at navigating rocks, hills, mountains etc.
    _travel_cost(MAP_TUNDRA,        75);
    _travel_cost(MAP_DESERT,        66);
    _travel_cost(MAP_WASTELAND,     33);
    _travel_cost(MAP_PLAINS,        33);
    _travel_cost(MAP_FOREST,       100);
    _travel_cost(MAP_SWAMP,        175);
    _travel_cost(MAP_JUNGLE,       250);
    _travel_cost(MAP_FOOTHILLS,     33);
    _travel_cost(MAP_ICY_FOOTHILLS, 75);
    _travel_cost(MAP_MOUNTAINOUS,  150);
    _travel_cost(MAP_ICY_MOUNTAIN, 300);
    _travel_cost(MAP_BASIN,         33);
    _travel_cost(MAP_GLACIER,      100);
    _travel_cost(MAP_COASTAL,       33);
    _travel_cost(MAP_ICECAP,       150);

    _cluster_size(2, 5);
    _city_size(CITY_TYPE_CITY,      90, 1000);
    _city_size(CITY_TYPE_DUCHY,    650, 3800);
    _city_size(CITY_TYPE_CAPITAL, 1800, 7200);

// Start should end with any of "ioulr"
    _city_name_start("amo", "bel", "bil", "bor", "bur", "dai", "du",
                     "dur", "fal", "glau", "glo", "glor", "gor", "grel", "gu",
                     "hel", "ho", "hol", "hor", "hur", "ilu", "lor", "mon",
                     "mor", "nar", "nau", "ni", "nir", "no", "nol", "nor", "or",
                     "su", "tal", "thi", "tho", "thor", "tri", "tur", "ul",
                     "ul");

// Middle should start w/ anything that follows "ioulr", end with a vowel or "r"
    _city_name_middle("a", "ar", "be", "bi", "bor", "dar", "de", "dene", "der",
                      "di", "dine", "do", "dor", "dore", "dostr", "du", "e",
                      "fa", "far", "ga", "gar", "ge", "ger", "gerde", "gla",
                      "glar", "gle", "gor", "gora", "gu", "gunde", "le", "lege",
                      "mir", "mli", "mo", "na", "nar", "nda", "ndar", "nde",
                      "nder", "ndi", "ndo", "ndor", "ne", "nere", "nesse",
                      "ngi", "nlo", "nlir", "nur", "nur", "nunde", "sar", "se",
                      "sela", "selar", "ser", "sere", "ste", "sto", "stone",
                      "stone", "stone", "stor", "sune", "suner", "te", "ter",
                      "tor", "tori", "tre", "tro", "ve", "vi", "vise", "vur",
                      "vurse");

    _city_name_end("", "", "", "", "", "", "", "", "", "", "", "", "der", "dom",
                   "dom", "dom", "don", "donar", "dum", "dun", "dun", "dun",
                   "forge", "forge", "gost", "gost",  "grad", "grod", "grod",
                   "lan", "lind", "nar", "nar", "roth", "th", "th", "thun",
                   "thundar");

    _base_combat(11);
    _hp(120);
    _food_consumption(105);
    _warlikeness(-1);
    _life_value(3);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    6500);
    _starting_resources (RES_WOOD,    2000);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  40);
    _morale_requirement (CIT_BURGHER,   60);
    _low_tax_rate       (CIT_PEASANT,   25);
    _low_tax_rate       (CIT_MERCHANT,  20);
    _low_tax_rate       (CIT_BURGHER,   15);
    _high_tax_rate      (CIT_PEASANT,   85);
    _high_tax_rate      (CIT_MERCHANT,  75);
    _high_tax_rate      (CIT_BURGHER,   65);

    _relations(RACE_HUMAN,     1);
    _relations(RACE_ELF,       1);
    _relations(RACE_DWARF,     1);
    _relations(RACE_ORC,      -1);
    _relations(RACE_TROLL,    -5);
    _relations(RACE_HALFLING,  1);
    _relations(RACE_GNOME,     2);
    _relations(RACE_MINOTAUR,  2);
    _relations(RACE_RAKSHASA,  1);
    _relations(RACE_NAGA,     -1);

    _skill(SKILL_FARMING,       2);
    _skill(SKILL_HUNTING,       2);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        5);
    _skill(SKILL_FORESTRY,      2);
    _skill(SKILL_CONSTRUCTION,  4);
    _skill(SKILL_TRADE,         4);
    _skill(SKILL_ESPIONAGE,     3);
    _skill(SKILL_MAGIC,         2);
    _skill(SKILL_EARTH_MAGIC,   5);
    _skill(SKILL_WATER_MAGIC,   2);
    _skill(SKILL_AIR_MAGIC,     1);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    2);
    _skill(SKILL_DEATH_MAGIC,   2);

    _description("\
Dwarves are a short, stocky <link=race>race</link>.  They have a great \
affinity for tunnels and caverns, and their small stature helps to make them \
excellent <link=mining>miners</link>.  Because of this, dwarves are usually \
most at home in <link=mountain>mountains</link> and <link=hill>foothills\
</link>, and less comfortable in wide open spaces.  Their small size and great \
strength makes them excellent at navigating mountains, but slower than most \
when traveling open <link=plains>plains</link>.\n\
Dwarves are not particularly social creatures, and give birth more slowly than \
most races.  They also often prefer to live as <link=homesteader>hermits\
</link> rather than in cities.  Dwarves have strong regional ties, and are \
often more loyal to their home <link=city>city</link> rather than to some far-\
off <link=monarch>king</link>.  They are rarely aggressive, but fight with a \
great ferocity if provoked.  A dwarf can be rather difficult to kill!  Their \
<link=armor>armor</link> and <link=weapons>weapons</link> are of legendary \
quality, particularly their <link=axes>battleaxes</link>, which are considered \
some of the best weapons in the world.\n\
Dwarves are fairly affable in regards to the other races in the world.  They \
have a particular fondness for <link=gnomes>gnomes</link>, due to their \
similar stature and mountain homes.\n\
Dwarves fare rather poorly when it comes to <link=magic>magic</link>, and they \
lack an understanding of most <link=magical school>schools</link> of magic.  \
However, they do possess an innate understanding of <link=earth magic>earth\
</link> magic, and their <link=geomancer>geomancers</link>, rare though they \
are, are among the most powerful in the world.\
");


  _race(RACE_ORC);
    _name("orc");
    _plural_name("orcs");
    _adjective("orcish");

    _color(c_ltred);
    _kingdom_color(c_red);
    _kingdom_color(c_magenta);
    _kingdom_color(c_brown);
/*
    _kingdom_color(c_ltred);
    _kingdom_color(c_yellow);
*/

    _map_type(MAP_WASTELAND,  100);
    _map_type(MAP_BASIN,      100);
    _map_type(MAP_FOOTHILLS,   80);
    _map_type(MAP_CANYON,      45);
    _map_type(MAP_PLAINS,      40);
    _map_type(MAP_JUNGLE,      15);
    _map_type(MAP_MOUNTAINOUS, 10);

    _map_type(MAP_DESERT,     -10);

// Orcs' strength and affinity for hills make them better at traversing rocky
    _travel_cost(MAP_WASTELAND,      33);
    _travel_cost(MAP_FOOTHILLS,      40);
    _travel_cost(MAP_ICY_FOOTHILLS,  80);
    _travel_cost(MAP_MOUNTAINOUS,   200);
    _travel_cost(MAP_ICY_MOUNTAIN,  350);
    _travel_cost(MAP_CANYON,         33);

    _cluster_size(4, 6);
    _city_size(CITY_TYPE_CITY,     100, 1800);
    _city_size(CITY_TYPE_DUCHY,    800, 4500);
    _city_size(CITY_TYPE_CAPITAL, 2000, 9200);

    _city_name_start("arz", "bhor", "bor", "boz", "dhul", "dush", "ghal",
                     "ghor", "ghul", "grag", "gros", "kaz", "khar", "khaz",
                     "khor", "khul", "kuz", "larg", "mhor", "mol", "mhol",
                     "narz", "nash", "orc", "orc", "orcr", "ork", "ork", "ork",
                     "ork", "orkr", "ors", "ors", "orz", "rhas", "rhaz", "ror",
                     "rhul", "rhuz", "santh", "sanz", "sarz", "sorz", "suth",
                     "taz", "thor", "tor", "thor", "ur", "urz", "vhoz", "wur",
                     "zath", "zho'", "zhuth");

    _city_name_middle("", "", "", "", "", "", "", "", "",
                      " Khal", " Khal", " Khor", " Mhor", " Mhor", " Ork",
                      " Ork", " Rhi", " Vir", " Zhor", "ak", "ak", "akh", "akr",
                      "akr", "al", "anik", "ankh", "ar", "ark", "ath", "az",
                      "ek", "ek", "ekr", "el", "en", "ent", "entr", "enz", "er",
                      "erz", "et", "eth", "etr", "ez", "i'", "ik", "ikr", "in",
                      "ink", "inz", "ir", "nik", "nikr", "ok", "ok", "okr",
                      "uk", "uk", "uk", "uk", "ukr", "ukr", "ul", "ur", "urk",
                      "urz", "uz", "zek", "zel", "zhor");

    _city_name_end("", "", "", "", "", "a", "a", "a", "ak", "al", "ani", "anik",
                   "arth", "ath", "az", "e", "el", "en", "ez", "o", "one", "or",
                   "ork", "orz", "os", "os", "ur", "urk", "urk", "uz", "uz");

    _base_combat(14);
    _hp(130);
    _food_consumption(105);
    _warlikeness(3);
    _life_value(2);

    _starting_population(CIT_PEASANT,  120);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    2000);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  95);
    _birth_rate         (CIT_BURGHER,   90);
    _citizen_ratio      (CIT_MERCHANT,  15);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  30);
    _morale_requirement (CIT_BURGHER,   50);
    _low_tax_rate       (CIT_PEASANT,   30);
    _low_tax_rate       (CIT_MERCHANT,  25);
    _low_tax_rate       (CIT_BURGHER,   20);
    _high_tax_rate      (CIT_PEASANT,  100);
    _high_tax_rate      (CIT_MERCHANT,  85);
    _high_tax_rate      (CIT_BURGHER,   70);

    _relations(RACE_ELF,      -1);
    _relations(RACE_DWARF,     1);
    _relations(RACE_ORC,       2);
    _relations(RACE_GOBLIN,   -1);
    _relations(RACE_OGRE,      1);
    _relations(RACE_TROLL,     1);
    _relations(RACE_HALFLING, -3);
    _relations(RACE_GNOME,    -2);
    _relations(RACE_MINOTAUR,  2);
    _relations(RACE_RAKSHASA,  1);

    _skill(SKILL_FARMING,       1);
    _skill(SKILL_HUNTING,       4);
    _skill(SKILL_LIVESTOCK,     2);
    _skill(SKILL_MINING,        3);
    _skill(SKILL_FORESTRY,      3);
    _skill(SKILL_CONSTRUCTION,  2);
    _skill(SKILL_TRADE,         2);
    _skill(SKILL_ESPIONAGE,     2);
    _skill(SKILL_MAGIC,         1);
    _skill(SKILL_EARTH_MAGIC,   2);
    _skill(SKILL_WATER_MAGIC,   1);
    _skill(SKILL_AIR_MAGIC,     1);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    1);
    _skill(SKILL_DEATH_MAGIC,   3);

    _description("\
Orcs are a nasty, brutish <link=race>race</link>, and they are proud of it.  \
Among all the races, orcs are the most eager for <link=war>conflict</link>.  \
Orcish citizens quickly grow bored if there is not a war to fight; this is the \
opposite of most races, which <link=war weariness>grow discontent during war\
</link>.  Fortunately (for them), orcs are naturally skilled in \
<link=combat>combat</link> and difficult to kill.  <c=white>Un<c=/>\
fortunately, orcs are not very skilled at crafting <link=armor>armor</link> or \
<link=weapons>weapons</link>, and will usually seek to <link=trade>buy</link> \
them from a race who can produce better than their crude equipment.\n\
Outside of combat, orcs are largely unskilled.  They are decent \
<link=mining>miners</link> and <link=forestry>woodsmen</link>, and good at the \
<link=hunting>hunt</link>, but fare poorly at other methods of obtaining \
food.  Orcs tend to have food imported, or more often, raid cities and \
<link=caravan>trade caravans</link> for their food supply.\n\
Orcs fare quite well in any terrain.  Their strength makes them better than \
most at traversing rough terrain, and they are quite happy living in \
<link=hill>hills</link> and <link=mountain>mountains</link>.  Orcs have a \
complex social structure.  While they do have a sense of loyalty to their \
<link=monarch>monarch</link> and <link=duke>dukes</link>, aggression is as \
common within an orc <link=kingdom>kingdom</link> as it is against other \
races.  Orcish cities which fail to demonstrate their success in the \
battlefield will be perceived as weak and unworthy, and it is perfectly \
socially acceptable for their neighbors to attack and destroy them.  Orcs do \
not have a typical sense of hereditary <link=succession>succession</link> as \
most races do.  Instead, a duke - or even the king or queen - is replaced when \
one of their subjects manages to invade their city and kill them.\n\
Orcs' relations with other races are ruled by might.  Orcs have respect for \
races who are skilled in combat, and nothing but contempt for those who are \
not.  The only exception is their degenerate cousins the <link=goblins>goblins\
</link> - orcs should despise these weaklings, but instead only view them with \
a mild distaste.\n\
Orcs have one of the worst affinities for <link=magic>magic</link> of all the \
races, and only very rarely does an orc take up its study.  They also fare \
very poorly in almost all <link=magical school>schools</link> of magic, the \
exceptions being <link=fire magic>fire</link> and <link=death magic>death\
</link>; and even those are only average.\
");


  _race(RACE_GOBLIN);
    _name("goblin");
    _plural_name("goblins");
    _adjective("goblin");

    _color(c_ltgray);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_brown);
    _kingdom_color(c_red);

    _map_type(MAP_WASTELAND,   100);
    _map_type(MAP_BASIN,        80);
    _map_type(MAP_PLAINS,       70);
    _map_type(MAP_JUNGLE,       60);
    _map_type(MAP_FOOTHILLS,    50);
    _map_type(MAP_MOUNTAINOUS,  20);

    _map_type(MAP_DESERT,      -10);

// Gobbos are good in forests & mountains, bad in ice
    _travel_cost(MAP_TUNDRA,         75);
    _travel_cost(MAP_FOREST,         50);
    _travel_cost(MAP_JUNGLE,        150);
    _travel_cost(MAP_FOOTHILLS,      40);
    _travel_cost(MAP_ICY_FOOTHILLS, 150);
    _travel_cost(MAP_MOUNTAINOUS,   200);
    _travel_cost(MAP_ICY_MOUNTAIN,  500);
    _travel_cost(MAP_CANYON,         33);
    _travel_cost(MAP_ICECAP,        150);

    _cluster_size(5, 8);
    _city_size(CITY_TYPE_CITY,     150,  2000);
    _city_size(CITY_TYPE_DUCHY,   1200,  6000);
    _city_size(CITY_TYPE_CAPITAL, 3000,  1200);

    _city_name_start("ak", "ar", "bek", "bik", "bir", "bu", "buk", "dak", "dek",
                     "dik", "dir", "du", "ek", "er", "fek", "fik", "fu", "hak",
                     "hu", "ik", "ir", "kak", "ku", "kuk", "krak", "krek", "na",
                     "nak", "nik", "nuk", "pik", "prik", "pu", "su", "rak",
                     "rik", "ru", "ruk", "sak", "sek", "sik", "stak", "stek",
                     "stik", "stuk", "su", "suk", "tak", "tek", "tik", "tu",
                     "tuk", "tra", "trak", "trek", "trik", "truk", "u", "uk",
                     "vak", "vik", "vu");

    _city_name_middle("", "", "", "", "", "", "", "", "", "", "", "", "", "",
                      "", "", "", "", "", "", "", "", "", "", "", "a", "aba",
                      "abi", "abu", "ada", "adi", "adu", "aka", "aki", "aku",
                      "ala", "ali", "alu", "ava", "avi", "avu", "la", "li",
                      "lu", "ra", "ri", "ru");

    _city_name_end("ba", "bak", "bek", "bu", "buk", "da", "dak", "dek", "dik",
                   "du", "duk", "fak", "fik", "fuk", "hak", "hik", "huk", "ka",
                   "kak", "kek", "kik", "ku", "kuk", "na", "nak", "ni", "nik",
                   "nok", "nu", "nuk", "pa", "pak", "pi", "pik", "pok", "puk",
                   "rak", "rik", "ro", "ru", "ruk", "sak", "sik", "so", "sok",
                   "spak", "sprak", "suk", "tak", "tek", "tik", "to", "trak",
                   "trik", "truk");

    _base_combat(8);
    _hp(85);
    _food_consumption( 90);
    _warlikeness(-1);
    _life_value(1);

    _starting_population(CIT_PEASANT,  140);
    _starting_resources (RES_GOLD,    4000);
    _starting_resources (RES_WOOD,    1600);
    _starting_resources (RES_STONE,   1600);

    _birth_rate         (CIT_PEASANT,   80);
    _birth_rate         (CIT_MERCHANT,  80);
    _birth_rate         (CIT_BURGHER,   80);
    _citizen_ratio      (CIT_MERCHANT,  20);
    _citizen_ratio      (CIT_BURGHER,   14);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   75);
    _low_tax_rate       (CIT_PEASANT,   25);
    _low_tax_rate       (CIT_MERCHANT,  20);
    _low_tax_rate       (CIT_BURGHER,   15);
    _high_tax_rate      (CIT_PEASANT,   90);
    _high_tax_rate      (CIT_MERCHANT,  80);
    _high_tax_rate      (CIT_BURGHER,   60);

    _relations(RACE_HUMAN,    -1);
    _relations(RACE_ELF,      -2);
    _relations(RACE_DWARF,     1);
    _relations(RACE_ORC,       2);
    _relations(RACE_GOBLIN,    1);
    _relations(RACE_OGRE,      1);
    _relations(RACE_TROLL,     2);
    _relations(RACE_GNOME,     1);
    _relations(RACE_RAKSHASA,  2);
    _relations(RACE_NAGA,      1);

    _skill(SKILL_FARMING,       3);
    _skill(SKILL_HUNTING,       2);
    _skill(SKILL_LIVESTOCK,     1);
    _skill(SKILL_MINING,        3);
    _skill(SKILL_FORESTRY,      2);
    _skill(SKILL_CONSTRUCTION,  3);
    _skill(SKILL_TRADE,         3);
    _skill(SKILL_ESPIONAGE,     5);
    _skill(SKILL_MAGIC,         2);
    _skill(SKILL_EARTH_MAGIC,   2);
    _skill(SKILL_WATER_MAGIC,   2);
    _skill(SKILL_AIR_MAGIC,     3);
    _skill(SKILL_FIRE_MAGIC,    2);
    _skill(SKILL_LIFE_MAGIC,    2);
    _skill(SKILL_DEATH_MAGIC,   2);

    _description("\
Goblins are a distant cousin of <link=orcs>orcs</link>, though the resemblance \
is faint.  Unlike their large, strong, vicious relatives, goblins are small in \
stature (often measuring less than four feet tall) and cowardly in nature.  \
They fare poorly in <link=combat>combat</link> and are quite easily killed.  \
However, they possess a cunning nature and make excellent <link=espionage>spies\
</link> and <link=sabotauge>saboteurs</link>.  Other <link=race>races</link> \
are wary of goblins, and rightfully so; they'll profess to be your friend, all \
the while spying on your <link=city>cities</link> and selling what they learn \
to your enemies.  If they feel they have an opportunity, goblins will happily \
backstab you for whatever reward they can reap.  Still, goblins are eager to \
find customers for their mercenary-spies, and they can be a good friend to \
have.\n\
One of the most notable traits of goblins is their \
<link=citizen reproduction>fecundity</link>.  Few races give birth in greater \
numbers than goblins do.  With this high birth rate comes a low sense of the \
value of a life; goblins don't find it particularly immoral to kill another \
goblin, and suicide missions are accepted as a fact of life.  Goblin cities \
tend to be dominated by the <link=peasants>peasant</link> population, as this \
lowly caste feels more \"natural\" to them.  Though decent as \
<link=farming>farmers</link>, goblins fare poorly at the <link=hunting>hunt\
</link>, and even worse at keeping <link=livestock>livestock</link>; they have \
absolutely no affinity for animals.\n\
Goblins actually respect and even like members of other races, in particular \
those seen as \"ugly\" by <link=humans>humans</link> and <link=elves>elves\
</link> - whom the goblins despise due to their vanity and beauty.  Goblins \
carry a particular fondness for their cousins the orcs.  Unfortunately for the \
poor goblins, almost none of these admirations are returned.\n\
Goblins are quite poor at the study of <link=magic>magic</link>, lacking both \
the patience and the depth of thought required for its mastery.  They lack an \
affinity for any of the <link=magical school>schools of magic</link>, save for \
<link=air magic>air magic</link> at which they are merely average.\
");


  _race(RACE_OGRE);
    _name("ogre");
    _plural_name("ogres");
    _adjective("ogre");

    _color(c_brown);
    _kingdom_color(c_brown);
    _kingdom_color(c_red);
    _kingdom_color(c_magenta);

    _map_type(MAP_BASIN,       100);
    _map_type(MAP_PLAINS,       90);
    _map_type(MAP_WASTELAND,    50);
    _map_type(MAP_FOOTHILLS,    40);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_JUNGLE,      -30);
    _map_type(MAP_SWAMP,       -20);
    _map_type(MAP_MOUNTAINOUS, -10);

// Ogres' size make them bad at navigating tight quarters, like forests.  But
// they can traverse rocky areas more easily.
    _travel_cost(MAP_WASTELAND,      33);
    _travel_cost(MAP_FOREST,        150);
    _travel_cost(MAP_SWAMP,         175);
    _travel_cost(MAP_JUNGLE,        300);
    _travel_cost(MAP_FOOTHILLS,      33);
    _travel_cost(MAP_ICY_FOOTHILLS,  66);
    _travel_cost(MAP_MOUNTAINOUS,   200);
    _travel_cost(MAP_ICY_MOUNTAIN,  350);
    _travel_cost(MAP_CANYON,         33);

    _cluster_size(1, 3);
    _city_size(CITY_TYPE_CITY,      50,  800);
    _city_size(CITY_TYPE_DUCHY,    500, 2800);
    _city_size(CITY_TYPE_CAPITAL, 1400, 5000);

// End with lr
    _city_name_start("al", "ar", "bal", "bar", "bel", "bil", "dal", "dar",
                     "del", "el", "fal", "far", "ful", "hal", "har", "her",
                     "hul", "il", "ir", "lar", "lor", "lur", "mal", "mar",
                     "mel", "mer", "mil", "mor", "mul", "mur", "nal", "nar",
                     "nel", "nil", "nor", "nul", "or", "rel", "ril", "tal",
                     "tar", "ter", "til", "tor", "tul", "ur", "war", "wer",
                     "wil", "wor", "yal", "yar", "yel", "yil", "yor", "yus");

// Start with bdkt, end with vowel or lr
    _city_name_middle("", "", "", "", "bal", "bar", "ba", "bel", "be", "bil",
                      "bi", "bor", "bo", "bul", "bur", "bu", "dal", "dar",
                      "da", "del", "de", "dil", "di", "dor", "do", "dul",
                      "du", "kal", "kar", "ka", "kel", "ke", "kil", "ki",
                      "kor", "ko", "kul", "ku", "tal", "tar", "ta", "tel",
                      "te", "til", "ti", "tor", "to", "tu");

// Start with bdkt, end with aodklnrt,rd,rk,rt,
    _city_name_end("", "", "", "", "ba", "bad", "bak", "ban", "bar", "bat",
                   "bard", "bark", "bart", "bed", "bek", "bel", "ben", "bet",
                   "berd", "bid", "bik", "bil", "bin", "bit", "birk", "birt",
                   "bo", "bod", "bok", "bon", "bot", "bord", "bork", "bort",
                   "bud", "buk", "bun", "bur", "but", "burd", "burk", "burt",
                   "da", "dad", "dak", "dal", "dan", "dar", "dat", "dard",
                   "dark", "dart", "ded", "dek", "del", "den", "det", "derk",
                   "dert", "did", "dil", "din", "dit", "dirk", "dirt", "do",
                   "dod", "dok", "don", "dor", "dot", "dork", "dort", "dud",
                   "duk", "dun", "dut", "durk", "durt", "ka", "kad", "kal",
                   "kan", "kar", "kat", "kard", "kark", "kart", "ked", "kel",
                   "ken", "ket", "kerk", "kert", "kid", "kik", "kin", "kit",
                   "kirk", "ko", "kod", "kok", "kol", "kon", "kor", "kot",
                   "kord", "kork", "kort", "kud", "kuk", "kul", "kut", "kurd",
                   "kurk", "kurt", "ta", "tad", "tak", "tal", "tan", "tar",
                   "tat", "tard", "tark", "tart", "ted", "tek", "tel", "ten",
                   "ter", "tet", "terd", "terk", "tert", "tid", "tik", "til",
                   "tin", "tit", "tird", "tird", "tirt", "to", "tod", "tok",
                   "ton", "tor", "tot", "tord", "tork", "tort", "tud", "tuk",
                   "tun", "tut", "turd", "turk", "turt");

    _base_combat(13);
    _hp(150);
    _food_consumption(140);
    _warlikeness(-2);
    _life_value(4);

    _starting_population(CIT_PEASANT,   90);
    _starting_resources (RES_GOLD,    4500);
    _starting_resources (RES_WOOD,    2800);
    _starting_resources (RES_STONE,   2800);

    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   70);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _relations(RACE_HUMAN,     2);
    _relations(RACE_ELF,       2);
    _relations(RACE_DWARF,     1);
    _relations(RACE_ORC,      -2);
    _relations(RACE_GOBLIN,   -1);
    _relations(RACE_OGRE,      3);
    _relations(RACE_TROLL,    -4);
    _relations(RACE_HALFLING,  3);
    _relations(RACE_GNOME,    -1);
    _relations(RACE_RAKSHASA,  1);
    _relations(RACE_MINOTAUR,  2);

    _skill(SKILL_FARMING,       4);
    _skill(SKILL_HUNTING,       2);
    _skill(SKILL_LIVESTOCK,     4);
    _skill(SKILL_MINING,        3);
    _skill(SKILL_FORESTRY,      4);
    _skill(SKILL_CONSTRUCTION,  2);
    _skill(SKILL_TRADE,         2);
    _skill(SKILL_ESPIONAGE,     2);
    _skill(SKILL_MAGIC,         2);
    _skill(SKILL_EARTH_MAGIC,   4);
    _skill(SKILL_WATER_MAGIC,   4);
    _skill(SKILL_AIR_MAGIC,     3);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    4);
    _skill(SKILL_DEATH_MAGIC,   2);

    _description("\
Ogres are a large, lumbering <link=race>race</link>, resembling a huge version \
of something from <link=humans>humans</link>' evolutionary past.  Their size \
and innate strength make them decent fighters and quite difficult to kill, but \
in fact ogres are a rather peaceful race.  The structure of their \
<link=kingdom>kingdoms</link> is rather loose, and often ogres would rather \
lead a <link=homesteaders>solitary existence</link> rather than accept the \
social contract of living in a <link=city>city</link>.\n\
Their large size gives ogres quite an appetite, and two ogres eat almost as \
much as three humans.  Fortunately for them, ogres are skilled \
<link=farming>farmers</link> and keepers of <link=livestock>livestock</link>.  \
They are good <link=forestry>woodsmen</link> as well, but their size makes it \
difficult for them to navigate <link=forest>forests</link> and other dense \
terrain.\n\
Ogres <link=citizen reproduction>give birth</link> somewhat slowly, and tend \
to live in smaller numbers than most races.  Families tend to be small but \
close-knit.\n\
Ogres get along quite well with most other races, in particular their tiny \
agrarian friends the <link=halflings>halflings</link>.  Due to their peace-\
loving ways, ogres harbor a dislike for the more warfaring races like \
<link=orcs>orcs</link>.  They're also highly distrustful of the arcane studies \
of the <link=gnomes>gnomes</link> and of the wily ways of the \
<link=goblins>goblins</link>.\n\
Ogres don't have a particularly strong affinity for <link=magic>magic</link>, \
but on the rare occasion that an ogre does undertake the study required to \
become a <link=mage>mage</link> they have a strong connection to \
<link=earth magic>earth</link>, <link=water magic>water</link> and \
<link=life magic>life</link> magic, and fare well at most others.\
");


  _race(RACE_TROLL);
    _name("troll");
    _plural_name("trolls");
    _adjective("trollish");

    _color(c_dkgray);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_brown);
    _kingdom_color(c_red);

    _map_type(MAP_FOOTHILLS,   100);
    _map_type(MAP_CANYON,      100);
    _map_type(MAP_MOUNTAINOUS,  80);

    _map_type(MAP_SWAMP,       -40);
    _map_type(MAP_PLAINS,      -20);

    _travel_cost(MAP_WASTELAND,      25);
    _travel_cost(MAP_MOUNTAINOUS,   150);
    _travel_cost(MAP_ICY_MOUNTAIN,  250);

    _cluster_size(1, 2);
    _city_size(CITY_TYPE_CITY,      25,  300);
    _city_size(CITY_TYPE_DUCHY,    200,  800);
    _city_size(CITY_TYPE_CAPITAL,  500, 2000);

    _city_name_start("ak", "ar", "ark", "buk", "bur", "burk", "duk", "dar",
                     "dark", "dur", "dor", "durk", "ek", "er", "hak", "har",
                     "hark", "hek", "hur", "hurk", "lar", "luk", "lor", "lork",
                     "lur", "mak", "mar", "mark", "mor", "muk", "mur", "murk",
                     "rak", "rek", "rok", "ruk", "tak", "tar", "tark", "tek",
                     "ter", "terk", "tuk", "turk", "uk", "ur", "urk");

    _city_name_middle(" A", " Ba", " Be", " Bu", " Da", " De", " Du", " E",
                      " Ha", " He", " Hu", " La", " Le", " Lu", " Ma", " Me",
                      " Mu", " Ra", " Re", " Ro", " Ru", " Ta", " Te", " Tu",
                      " U", "a", "e", "la", "le", "lu", "ra", "re", "ro", "ru",
                      "ta", "te", "u");

    _city_name_end("", "k", "k", "k", "k", "k", "r", "r", "r", "rk", "rk");

    _base_combat(20);
    _hp(200);
    _food_consumption(180);
    _warlikeness(0);  // Trolls truly don't care if they're at war or not!
    _life_value(2);

    _starting_population(CIT_PEASANT,   20);
    _starting_resources (RES_GOLD,    6000);
    _starting_resources (RES_WOOD,    2000);
    _starting_resources (RES_STONE,   3500);

    _birth_rate         (CIT_PEASANT,  180);
    _birth_rate         (CIT_MERCHANT, 170);
    _birth_rate         (CIT_BURGHER,  160);
    _citizen_ratio      (CIT_MERCHANT,  20);
    _citizen_ratio      (CIT_BURGHER,   20);
    _morale_requirement (CIT_MERCHANT,  20);
    _morale_requirement (CIT_BURGHER,   30);
    _low_tax_rate       (CIT_PEASANT,    5);
    _low_tax_rate       (CIT_MERCHANT,   5);
    _low_tax_rate       (CIT_BURGHER,    5);
    _high_tax_rate      (CIT_PEASANT,  100);
    _high_tax_rate      (CIT_MERCHANT,  95);
    _high_tax_rate      (CIT_BURGHER,   90);

    _relations(RACE_HUMAN,    -2);
    _relations(RACE_ELF,      -3);
    _relations(RACE_DWARF,    -5);
    _relations(RACE_GOBLIN,   -2);
    _relations(RACE_OGRE,     -2);
    _relations(RACE_HALFLING, -3);
    _relations(RACE_GNOME,    -3);
    _relations(RACE_MINOTAUR, -1);
    _relations(RACE_RAKSHASA,  3);
    _relations(RACE_NAGA,     -1);

    _skill(SKILL_FARMING,       1);
    _skill(SKILL_HUNTING,       4);
    _skill(SKILL_LIVESTOCK,     1);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      1);
    _skill(SKILL_CONSTRUCTION,  1);
    _skill(SKILL_TRADE,         1);
    _skill(SKILL_ESPIONAGE,     1);
    _skill(SKILL_MAGIC,         1);
    _skill(SKILL_EARTH_MAGIC,   2);
    _skill(SKILL_WATER_MAGIC,   1);
    _skill(SKILL_AIR_MAGIC,     1);
    _skill(SKILL_FIRE_MAGIC,    2);
    _skill(SKILL_LIFE_MAGIC,    1);
    _skill(SKILL_DEATH_MAGIC,   4);

    _description("\
Of all the <link=race>races</link> in the world, the trolls are without a \
doubt the most terrifying and garish.  Most races regard them as barely more \
than animals; those who are lucky enough to have never met one often think of \
them as nothing more than fairy tales to scare children.  But trolls are all \
too real, and do indeed gather to live in <link=city>settlements</link>.  \
Calling them \"cities\" may be a bit of a stretch, as they rather number more \
than a few hundred residents.  The structure of troll \"<link=kingdom>kingdoms\
</link>\" is similarly loose, with only a passing amount of rule wielded by \
the <link=monarch>monarch</link> or <link=duke>duchies</link>.\n\
Trolls are famed for being deadly in <link=combat>combat</link>.  They are not \
skilled in using or making <link=weapon>weapons</link>, but they hardly need \
them, as trolls have natural set of vicious claws and a skin that exceeds \
<link=leather armor>leather armor</link> in toughness.  They also have a \
natural regenerative ability, making <link=medic>field medics</link> totally \
unnecessary.  They use these traits to their advantage when \
<link=hunting>hunting</link>, essentially tearing through the terrain, \
devouring any creatures unlucky enough to be in their path.  However, trolls \
are totally useless at <link=farming>farming</link> and rearing \
<link=livestock>livestock</link>.  In fact, they are abysmal at most \
<link=race skills>skills</link>.  All trolls know is killing and eating.\n\
And they eat a lot - a troll eats almost as much as two <link=humans>humans\
</link> each day.  Since they <link=anthropophage>eat other races</link>, \
trolls often rely on raiding cities to kill and eat their inhabitants.  \
Despite their fast metabolism, trolls are quite slow to \
<link=citizen reproduction>reproduce</link> and tend to have very small \
cities, with most trolls living in the wilderness in a semi-feral state.\n\
Trolls see little point in <link=diplomacy>diplomacy</link> or \
<link=trade>trade</link> with non-trolls - after all, they're supposed to be \
food, not friends.  They are particularly loathe to fraternize with \
<link=dwarves>dwarves</link>, their ancient enemies.  One (slight) exception \
is the <link=orcs>orcs</link>, whose prowess in battle is respected (to a \
degree) by trolls.  Trolls are also particularly susceptible to the \
\"glamour\" of the <link=rakshasa>rakshasa</link>, viewing them with a kind of \
awe.\n\
Trolls lack the intelligence or organization to pursue <link=magic>magic\
</link>, and even if they did, they have essentially no connection to any of \
the <link=magical school>magical schools</link> besides \
<link=death magic>death magic</link>.\
");


  _race(RACE_HALFLING);
    _name("halfling");
    _plural_name("halflings");
    _adjective("halfling");

    _color(c_ltblue);
    _kingdom_color(c_blue);
    _kingdom_color(c_cyan);
    _kingdom_color(c_green);

    _map_type(MAP_BASIN,       100);
    _map_type(MAP_PLAINS,       90);
    _map_type(MAP_FOREST,       80);

    _map_type(MAP_DESERT,      -90);
    _map_type(MAP_JUNGLE,      -70);

// Halflings are small, and just plain poor travelers, and as a result travel
// slowly almost everywhere.  However, their small size makes it easier to
// traverse forests and the like, and they travel normally there.
    _travel_cost(MAP_TUNDRA,         75);
    _travel_cost(MAP_DESERT,         60);
    _travel_cost(MAP_WASTELAND,      60);
    _travel_cost(MAP_PLAINS,         33);
    _travel_cost(MAP_FOOTHILLS,      75);
    _travel_cost(MAP_ICY_FOOTHILLS, 150);
    _travel_cost(MAP_MOUNTAINOUS,   375);
    _travel_cost(MAP_ICY_MOUNTAIN,  600);
    _travel_cost(MAP_BASIN,          33);
    _travel_cost(MAP_CANYON,         60);
    _travel_cost(MAP_GLACIER,       100);
    _travel_cost(MAP_COASTAL,        33);
    _travel_cost(MAP_ICECAP,        150);

    _cluster_size(3, 4);
    _city_size(CITY_TYPE_CITY,     100, 1200);
    _city_size(CITY_TYPE_DUCHY,    800, 4000);
    _city_size(CITY_TYPE_CAPITAL, 2000, 8000);

    _city_name_start("bl", "br", "ch", "cl", "cr", "dr", "fl", "fr", "gl", "gr",
                     "l", "pl", "pr", "r", "st", "str", "th", "tr", "wh", "wr");

    _city_name_middle("aid", "aig", "aigh", "aight", "ail", "aim", "ain", "air",
                      "airt", "aist", "ait", "aive", "aize", "aud", "aug",
                      "augh", "aught", "aum", "aun", "aur", "aurt", "aust",
                      "aut", "auve", "ay", "ead", "eal", "eam", "ean", "ear",
                      "eart", "east", "eat", "eave", "eaze", "eed", "eegh",
                      "eel", "eem", "een", "eer", "eest", "eet", "eeve", "eeze",
                      "eum", "eun", "eur", "euze", "iad", "iar", "ian", "iar",
                      "iom", "ion", "ior", "iot", "oar", "oast", "oid", "oil",
                      "oin", "oist", "oit", "oize", "ood", "oog", "oogh", "oom",
                      "oon", "oor", "oost", "oot", "oud", "oun", "oust", "out");

    _city_name_end(" Bottom", "s Bottom", " End", "s End", " Field", "s Field",
                   " Glen", " Hall", "sberry", "sborough", "sburg", "sbury",
                   "sby", "sdon", "dover", "field", "ford", "ford", "sforth",
                   "gale", "ham", "ham", "ham", "skirk", "smouth", "sneck",
                   "ton", "ton", "wood", "svale", "sville");

    _base_combat(5);
    _hp(80);
    _food_consumption(100);
    _warlikeness(-3);
    _life_value(4);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    2500);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  90);
    _birth_rate         (CIT_BURGHER,   80);
    _citizen_ratio      (CIT_MERCHANT,   7);
    _citizen_ratio      (CIT_BURGHER,    7);
    _morale_requirement (CIT_MERCHANT,  60);
    _morale_requirement (CIT_BURGHER,   80);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _relations(RACE_HUMAN,     1);
    _relations(RACE_ELF,      -1);
    _relations(RACE_DWARF,     2);
    _relations(RACE_ORC,      -3);
    _relations(RACE_GOBLIN,   -2);
    _relations(RACE_OGRE,      2);
    _relations(RACE_TROLL,    -4);
    _relations(RACE_HALFLING,  3);
    _relations(RACE_GNOME,    -1);
    _relations(RACE_RAKSHASA,  1);
    _relations(RACE_NAGA,     -2);

    _skill(SKILL_FARMING,       4);
    _skill(SKILL_HUNTING,       1);
    _skill(SKILL_LIVESTOCK,     5);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      4);
    _skill(SKILL_CONSTRUCTION,  3);
    _skill(SKILL_TRADE,         2);
    _skill(SKILL_ESPIONAGE,     3);
    _skill(SKILL_MAGIC,         3);
    _skill(SKILL_EARTH_MAGIC,   3);
    _skill(SKILL_WATER_MAGIC,   3);
    _skill(SKILL_AIR_MAGIC,     3);
    _skill(SKILL_FIRE_MAGIC,    2);
    _skill(SKILL_LIFE_MAGIC,    4);
    _skill(SKILL_DEATH_MAGIC,   2);

    _description("\
Halflings are a reclusive <link=race>race</link>, small in stature (about half \
the height of a <link=human>human</link> - hence the name).  They shun \
adventure and <link=war>warfaring</link>, preferring instead to focus on \
enjoying a life of leisure at home.  A halfling's favorite form of leisure is \
<link=food>eating</link>.  They have a strong appetite, and will eat as much \
as a human despite their small size.  What's more, halflings treat food as a \
<link=luxury>luxury</link> and extra rations can be assigned in the \
<link=luxury management screen>luxury management screen</link> to boost \
<link=morale>morale</link>.\n\
Fortunately for their appetites, halflings are skilled <link=farming>farmers\
</link>, and their affinity with <link=livestock>livestock</link> is \
unrivaled.  They are extremely poor <link=hunting>hunters</link>, however, and \
not at all at home in a <link=mining>mine</link>.  They are also excessively \
weak in <link=combat>combat</link>, and prefer to settle conflicts via \
<link=diplomacy>diplomacy</link>.\n\
Halflings don't particularly get along well with other races, particularly the \
more adventurous or warlike of them.  Their similar stature makes \
<link=dwarves>dwarves</link> their natural friends, and their agrarian \
lifestyle leads to positive relations with <link=ogres>ogres</link>.\n\
Halflings display a more or less average aptitude with <link=magic>magic\
</link>, with a particular aptitude for the healing powers of \
<link=life magic>life magic</link>.  They are less in touch with the more \
destructive <link=magical school>schools</link> of <link=fire magic>fire\
</link> and <link=death magic>death</link>.\
");


  _race(RACE_GNOME);
    _name("gnome");
    _plural_name("gnomes");
    _adjective("gnomic");

    _color(c_ltgray);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_brown);
    _kingdom_color(c_cyan);

    _map_type(MAP_CANYON,         100);
    _map_type(MAP_FOOTHILLS,       90);
    _map_type(MAP_MOUNTAINOUS,     85);
    _map_type(MAP_BASIN,           40);
    _map_type(MAP_PLAINS,          20);

    _map_type(MAP_ICECAP,         -95);
    _map_type(MAP_ICY_MOUNTAIN,   -95);
    _map_type(MAP_TUNDRA,         -90);
    _map_type(MAP_DESERT,         -90);
    _map_type(MAP_ICY_FOOTHILLS,  -85);
    _map_type(MAP_GLACIER,        -80);
    _map_type(MAP_SWAMP,          -60);
    _map_type(MAP_JUNGLE,         -30);
    _map_type(MAP_FOREST,         -10);

// They're small so most travel takes 50% longer; but small size means they're
// good at picking through forests etc. so those have no penalty.  Also they're
// excellent at mountain traveling, so they move faster there!
    _travel_cost(MAP_TUNDRA,         90);
    _travel_cost(MAP_DESERT,         75);
    _travel_cost(MAP_WASTELAND,      75);
    _travel_cost(MAP_PLAINS,         50);
    _travel_cost(MAP_FOOTHILLS,      75);
    _travel_cost(MAP_ICY_FOOTHILLS, 150);
    _travel_cost(MAP_MOUNTAINOUS,   175);
    _travel_cost(MAP_ICY_MOUNTAIN,  275);
    _travel_cost(MAP_BASIN,          45);
    _travel_cost(MAP_CANYON,         45);
    _travel_cost(MAP_GLACIER,       130);
    _travel_cost(MAP_COASTAL,        50);
    _travel_cost(MAP_ICECAP,        165);
    

    _cluster_size(1, 4);
    _city_size(CITY_TYPE_CITY,      40,   750);
    _city_size(CITY_TYPE_DUCHY,    800,  4000);
    _city_size(CITY_TYPE_CAPITAL, 3500, 12000);

// City names are vaguely based on ancient greek!
    _city_name_start("ab", "ab", "ach", "act", "aeg", "aeg", "aen", "akr", "al",
                     "al", "amn", "and", "ant", "ap", "ast", "bhr", "br", "ch",
                     "chr", "chron", "col", "cum", "cyn", "cyr", "dec", "del",
                     "dod", "dor", "edes", "elat", "emp", "eph", "epid", "eret",
                     "eub", "gour", "gort", "gyth", "hel", "hell", "herm",
                     "hier", "hybl", "ias", "imbr", "ith", "kall", "kimm",
                     "knid", "knos", "kyd", "kyr", "lam", "laod", "lemp",
                     "lind", "liss", "mall", "mant", "morg", "myr", "myrm",
                     "naucr", "naudr", "nic", "nymp", "nyr", "oen", "od", "old",
                     "onch", "pand", "pandr", "padr", "patr", "perg", "phaitr",
                     "phass", "phil", "phocr", "pis", "pix", "psych", "psydr",
                     "pyr", "rhod", "sam", "sel", "sest", "syb", "syr", "tar",
                     "tan", "thas", "th", "thr", "tr", "trip", "tyr", "tyr");
                     

    _city_name_middle("", "", "", "", "", "ac", "acl", "ad", "aed", "aeg",
                      "ael", "aen", "aerr", "aes", "aeth", "alc", "all", "am",
                      "amn", "ant", "anth", "ar", "aryn", "arynth", "ass",
                      "ecus", "eil", "ein", "eir", "el", "eon", "em", "emb",
                      "embr", "en", "end", "er", "esm", "ess", "est", "et",
                      "eth", "i", "ic", "id", "ig", "il", "in", "ion", "iop",
                      "ios", "ipp", "is", "ist", "istr", "oc", "od", "odr",
                      "on", "ondr", "ones", "or", "os", "oss", "ost", "ostr",
                      "oll", "om", "opol", "ycth", "yd", "yn", "ynth", "yntr",
                      "yr", "yss", "yst", "yth");

    _city_name_end("a", "aca", "ae", "aea", "ai", "ana", "ana", "atis", "e",
                   "ea", "ea", "ede", "edon", "ene", "es", "esos", "esus",
                   "eia", "ia", "ia", "ia", "ias", "ina", "ios", "is", "ium",
                   "izi", "oa", "och", "oli", "olis", "olis", "olis", "on",
                   "os", "us", "us", "yn");

    _base_combat(6);
    _hp(75);
    _food_consumption(50);
    _warlikeness(-2);
    _life_value(3);

    _starting_population(CIT_PEASANT,   90);
    _starting_resources (RES_GOLD,    8500);
    _starting_resources (RES_WOOD,    1500);
    _starting_resources (RES_STONE,   1500);

    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,   5);
    _citizen_ratio      (CIT_BURGHER,    5);
    _morale_requirement (CIT_MERCHANT,  20);
    _morale_requirement (CIT_BURGHER,   60);
    _low_tax_rate       (CIT_PEASANT,   10);
    _low_tax_rate       (CIT_MERCHANT,   7);
    _low_tax_rate       (CIT_BURGHER,    5);
    _high_tax_rate      (CIT_PEASANT,   65);
    _high_tax_rate      (CIT_MERCHANT,  50);
    _high_tax_rate      (CIT_BURGHER,   35);

    _relations(RACE_HUMAN,      2);
    _relations(RACE_DWARF,      3);
    _relations(RACE_ORC,       -1);
    _relations(RACE_GOBLIN,     1);
    _relations(RACE_OGRE,      -1);
    _relations(RACE_TROLL,     -3);
    _relations(RACE_HALFLING,  -1);
    _relations(RACE_GNOME,      2);
    _relations(RACE_RAKSHASA,   1);

    _skill(SKILL_FARMING,       3);
    _skill(SKILL_HUNTING,       1);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        4);
    _skill(SKILL_FORESTRY,      3);
    _skill(SKILL_CONSTRUCTION,  3);
    _skill(SKILL_TRADE,         5);
    _skill(SKILL_ESPIONAGE,     3);
    _skill(SKILL_MAGIC,         1);
    _skill(SKILL_EARTH_MAGIC,   4);
    _skill(SKILL_WATER_MAGIC,   2);
    _skill(SKILL_AIR_MAGIC,     2);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    1);
    _skill(SKILL_DEATH_MAGIC,   1);

    _description("\
Gnomes are a secretive, reclusive <link=race>race</link> who tend to live in \
defensible locations like <link=mountains>mountains</link> or \
<link=foothills>hills</link>.  They are the most diminuitive of all the races, \
and as a result travel tends to be quite slow when on foot (except when in \
mountains, as gnomes are quite skilled at traversing their uneven terrain).  \
Gnomes are most famous for their love of <link=gold>gold</link> and skill in \
<link=trade>trading</link> and all other forms of commerce.  However, gnomes \
are not misers, and are happy to extend a <link=loan>loan</link> to nearly \
anyone, particularly their fellow gnomes.\n\
Unfortunately, many races overlook this and tend to regard gnomes with \
suspicion and distrust.  For their part, gnomes tend to like other races, \
though they look down their noses a bit at those whom they see as \"country \
bumpkins\" like <link=ogres>ogres</link> or <link=halfling>halflings</link>.  \
Gnomes prefer to live in cities with the latest amenities and developments.  \
They are always interested in new ideas or <link=research>research</link>, and \
have a great curiousity about the world (though they are much more likely to \
pay another race for a glimpse at their <link=world map>map</link> than they \
are to explore it themselves).  On the other hand, gnomes have absolutely no \
time for the absurdity that is <link=gods>religion</link>.  They are totally \
areligious, and the gods in turn will have nothing to do with them.\n\
Their small stature means that gnomes do not need very much food at all, \
eating half as much as a human does.  It also makes them particularly weak in \
combat, and easy to kill.  However, gnomes are quite skilled with \
<link=crossbow>crossbows</link>, and the sight of a city wall lined with \
gnomic crossbowmen is enough to turn away many armies.\n\
Outside of trade, gnomes are fairly decent at most skills, though they don't \
have the patience or vigor for <link=hunting>hunting</link>.  Their society \
prizes craftsmanship above all else, and gnomic <link=citizens>citizens</link> \
tend to have as many <link=merchants>merchants</link> or \
<link=burghers>burghers</link> as <link=peasants>peasants</link>.\n\
Gnomes' strong connection to the world that they can see, touch, and study \
scientifically leaves them with a deficit in the world of <link=magic>magic\
</link>, and it is extremely rare that a gnome will take up its study.  On the \
exceptionally rare occasion that one does, they will find themselves skilled \
in <link=earth magic>earth</link> and <link=fire magic>fire</link> magic, and \
few other <link=magical school>schools</link>.\
");


  _race(RACE_MINOTAUR);
    _name("minotaur");
    _plural_name("minotaurs");
    _adjective("minotaur");

    _color(c_red);
    _kingdom_color(c_red);
    _kingdom_color(c_blue);
    _kingdom_color(c_green);

    _map_type(MAP_BASIN,           100);
    _map_type(MAP_PLAINS,           90);
    _map_type(MAP_WASTELAND,        75);
    _map_type(MAP_FOOTHILLS,        60);
    _map_type(MAP_FOREST,           40);
    _map_type(MAP_CANYON,           35);
    _map_type(MAP_MOUNTAINOUS,      20);
    _map_type(MAP_DESERT,           10);

    _map_type(MAP_ICECAP,         -100);
    _map_type(MAP_ICY_MOUNTAIN,   -100);
    _map_type(MAP_TUNDRA,          -90);
    _map_type(MAP_ICY_FOOTHILLS,   -70);
    _map_type(MAP_GLACIER,         -50);
    _map_type(MAP_SWAMP,           -15);
    _map_type(MAP_JUNGLE,          -10);

    _travel_cost(MAP_DESERT,        50);
    _travel_cost(MAP_WASTELAND,     30);
    _travel_cost(MAP_PLAINS,        20);

    _cluster_size(2, 4);
    _city_size(CITY_TYPE_CITY,     100, 1000);
    _city_size(CITY_TYPE_DUCHY,    750, 3800);
    _city_size(CITY_TYPE_CAPITAL, 1800, 7500);

// More greek-flavored names!  kinda!
    _city_name_start("ass", "b", "byr", "cr", "cyr", "d", "ess", "f", "g",
                     "gr", "gyr", "h", "hyr", "ill", "iss", "k", "kn", "l", "m",
                     "mn", "n", "oss", "pl", "pr", "pyr", "r", "st", "t", "tyr",
                     "ull", "ulv", "z");

    _city_name_middle("ad", "aed", "ael", "aer", "al", "all", "am", "amn", "an",                      "ap", "ar", "ard", "ars", "art", "at", "ath", "atr", "eg",                      "el", "ell", "em", "en", "esc", "esk", "esp", "ess",                            "est", "etr", "il", "in", "inn", "ist", "itr", "oc", "od",                      "odr", "oel", "ohem", "ol", "om", "omn", "on", "op",                            "opr", "os", "osk", "oss", "ost", "ostr", "ot", "otr",
                      "ul", "uss", "ust", "ustr", "yd", "ydr", "yg", "yl", "ym",
                      "ymn", "yp", "ypr", "ys", "yst", "ystr", "yt", "ytr");

    _city_name_end("a", "e", "es", "ia", "o", "oa", "os", "otia", "ua", "ude",
                   "ute", "yde", "ys");

    _base_combat(12);
    _hp(110);
    _food_consumption(110);
    _warlikeness(-1);
    _life_value(3);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    2000);
    _starting_resources (RES_STONE,   3000);

    _birth_rate         (CIT_PEASANT,   75);
    _birth_rate         (CIT_MERCHANT,  85);
    _birth_rate         (CIT_BURGHER,   90);
    _citizen_ratio      (CIT_MERCHANT,  12);
    _citizen_ratio      (CIT_BURGHER,   14);
    _morale_requirement (CIT_MERCHANT,  30);
    _morale_requirement (CIT_BURGHER,   50);
    _low_tax_rate       (CIT_PEASANT,   30);
    _low_tax_rate       (CIT_MERCHANT,  20);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   85);
    _high_tax_rate      (CIT_MERCHANT,  70);
    _high_tax_rate      (CIT_BURGHER,   65);

    _relations(RACE_ELF,       -1);
    _relations(RACE_DWARF,      2);
    _relations(RACE_GOBLIN,    -1);
    _relations(RACE_OGRE,       1);
    _relations(RACE_TROLL,     -2);
    _relations(RACE_GNOME,     -1);
    _relations(RACE_MINOTAUR,   2);
    _relations(RACE_RAKSHASA,   1);
    _relations(RACE_NAGA,      -1);

    _skill(SKILL_FARMING,       3);
    _skill(SKILL_HUNTING,       2);
    _skill(SKILL_LIVESTOCK,     4);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      3);
    _skill(SKILL_CONSTRUCTION,  5);
    _skill(SKILL_TRADE,         3);
    _skill(SKILL_ESPIONAGE,     2);
    _skill(SKILL_MAGIC,         2);
    _skill(SKILL_EARTH_MAGIC,   4);
    _skill(SKILL_WATER_MAGIC,   3);
    _skill(SKILL_AIR_MAGIC,     2);
    _skill(SKILL_FIRE_MAGIC,    3);
    _skill(SKILL_LIFE_MAGIC,    3);
    _skill(SKILL_DEATH_MAGIC,   3);

    _description("\
Minotaurs are a <link=race>race</link> of large, muscular beings resembling a \
bipedal <link=cow>cow</link>.  Like their bovine brethren, they are \
<link=vegetarian>vegetarian</link>, and eat no meat.  They are at home in a \
wide variety of environments, but prefer the open <link=farming>farmable\
</link> terrain of <link=plains>plains</link>.  Minotaurs are famous for their \
<link=construction>architectural</link> skills, and are proud of them.  Their \
<link=building>buildings</link> are more ornate than any other race's, and \
built more quickly to boot.\n\
Minotaurs are a noble race, placing great value on the labor of their \
<link=peasants>peasants</link> as well as the other <link=citizens>classes\
</link>.  They likewise hold respect for those races whom they perceive as \
hard-working, while taking a dislike to races they believe to be effete or \
treacherous, or who value things like study or magic over a hard day's work.\n\
While minotaurs are not particularly <link=war>warlike</link>, they can hold \
their own in <link=combat>battle</link> and are difficult to kill.  They are \
fairly decent at crafting <link=weapons>weapons</link> and can use almost any \
of them fairly well.\n\
Minotaurs are not well-versed in the ways of <link=magic>magic</link>, but it \
is not unknown to them.  They are equally skilled in all \
<link=magical school>schools</link>, though they show a slight preference for \
<link=earth magic>earth magic</link> over <link=air magic>air</link>.\
");


  _race(RACE_RAKSHASA);
    _name("rakshasa");
    _plural_name("rakshasa");
    _adjective("rakshasan");

    _color(c_pink);
    _kingdom_color(c_magenta);
    _kingdom_color(c_blue);
    _kingdom_color(c_ltgray);

    _map_type(MAP_JUNGLE,          100);
    _map_type(MAP_FOREST,           90);
    _map_type(MAP_CANYON,           85);
    _map_type(MAP_BASIN,            80);
    _map_type(MAP_MOUNTAINOUS,      40);
    _map_type(MAP_FOOTHILLS,        30);
    _map_type(MAP_SWAMP,            25);
    _map_type(MAP_PLAINS,           10);

    _map_type(MAP_ICY_MOUNTAIN,   -100);
    _map_type(MAP_ICY_FOOTHILLS,  -100);
    _map_type(MAP_TUNDRA,          -90);
    _map_type(MAP_GLACIER,         -75);
    _map_type(MAP_DESERT,          -10);

// Rakshasa are great at traveling in jungles and forests, but fare poorly in
// cold terrain.
    _travel_cost(MAP_TUNDRA,        100);
    _travel_cost(MAP_FOREST,         50);
    _travel_cost(MAP_SWAMP,         100);
    _travel_cost(MAP_JUNGLE,        125);
    _travel_cost(MAP_ICY_FOOTHILLS, 200);
    _travel_cost(MAP_ICY_MOUNTAIN,  800);
    _travel_cost(MAP_GLACIER,       150);
    _travel_cost(MAP_ICECAP,        200);

    _cluster_size(1, 3);
    _city_size(CITY_TYPE_CITY,      40,  500);
    _city_size(CITY_TYPE_DUCHY,    300, 1800);
    _city_size(CITY_TYPE_CAPITAL,  800, 4000);

// Vaguely based on Arabic city names

    _city_name_start("ab", "ab", "al", "al", "al", "al-", "al-", "al-", "al-",
                     "ahm", "ar", "ark", "ash", "ashk", "bag", "bah", "bahr",
                     "bat", "beir", "ben", "bir", "boum", "bur", "but", "dak",
                     "dak", "dam", "dar", "dhu", "dub", "fal", "fus", "had",
                     "haif", "har", "heb", "hej", "hib", "isk", "jeb", "jed",
                     "jib", "jir", "jub",  "khar", "khir", "khob", "mad", "man",
                     "mar", "mart", "mor", "mos", "nab", "nad", "naj", "niz",
                     "qal", "qar", "rab", "raf", "ram", "raq", "ras Ab", "saf",
                     "sak", "sam", "sam", "shar", "shef", "shul", "sid",
                     "sok", "som", "su", "sul", "tab", "tak", "tar", "tar",
                     "tas", "tid", "tik", "tir", "trip", "tru", "tul", "tun",
                     "uk", "um ", "um ", "zag", "zan");

    _city_name_middle("", "", "", " el-", " el-", "?-es-", "ab", "ab", "ab",
                      "ad", "af", "ahm", "aib", "air", "aj", "ak", "akra-Sh",
                      "an", "ans", "ar", "as", "asr", "assi", "at", "atri",
                      "eb", "ebbr", "ed", "edr", "ef", "efa-Ab", "eid", "eid",
                      "eif", "eir", "eit", "eitr", "ej", "ek", "ekr", "el",
                      "em", "en", "epp", "es", "essr", "est", "estr", "?had",
                      "?hadj", "?hah", "?hid", "?hidr", "?hif", "?hod", "?hof",
                      "?hud", "?hudj", "?huj", "id", "idr", "if", "if", "ifr",
                      "iq", "*iq", "iqr", "*iqr", "iq-Raf", "il", "il", "im",
                      "in", "ir", "ir", "itr", "odj", "odr", "of", "ok", "okr",
                      "ol", "on", "uad", "uar", "uk", "ukhr", "uw");

    _city_name_end("a", "ab", "ab", "ad", "ad", "ada", "ah", "ah", "ah", "ah",
                   "aib", "aid", "ail", "air", "ait", "aj", "aj", "akh", "al",
                   "al", "am", "ama", "amm", "an", "an", "ankh", "ar", "ara",
                   "asa", "at", "ata", "aud", "ayb", "az", "e'an", "e'un", "ea",
                   "eb", "eba", "ed", "edi", "eij", "eikh", "eikh", "eiz", "ej",
                   "em", "eppo", "erun", "es", "eth", "ez", "ezh", "i", "id",
                   "ig", "il", "ina", "ioch", "ir", "ir", "it", "it", "iya",
                   "iz", "o", "och", "od", "on", "ona", "ora", "os", "oukh",
                   "oum", "ous", "outi", "ouz", "uba", "ubi", "ubos", "uel",
                   "uk", "ukh", "un", "una", "ura", "us", "ut", "uzh", "ys");

    _base_combat(12);
    _hp(100);
    _food_consumption(115);
    _warlikeness(-1);
    _life_value(5);

    _starting_population(CIT_PEASANT,   60);
    _starting_population(CIT_MERCHANT,  20);
    _starting_resources (RES_GOLD,    8500);
    _starting_resources (RES_WOOD,    1500);
    _starting_resources (RES_STONE,   1500);

    _birth_rate         (CIT_PEASANT,  110);
    _birth_rate         (CIT_MERCHANT, 100);
    _birth_rate         (CIT_BURGHER,   90);
    _citizen_ratio      (CIT_MERCHANT,   8);
    _citizen_ratio      (CIT_BURGHER,    8);
    _morale_requirement (CIT_MERCHANT,  75);
    _morale_requirement (CIT_BURGHER,   90);
    _low_tax_rate       (CIT_PEASANT,   10);
    _low_tax_rate       (CIT_MERCHANT,   5);
    _low_tax_rate       (CIT_BURGHER,    5);
    _high_tax_rate      (CIT_PEASANT,   70);
    _high_tax_rate      (CIT_MERCHANT,  50);
    _high_tax_rate      (CIT_BURGHER,   40);

    _relations(RACE_HUMAN,    -1);
    _relations(RACE_DWARF,    -1);
    _relations(RACE_ORC,      -2);
    _relations(RACE_GOBLIN,   -3);
    _relations(RACE_OGRE,     -2);
    _relations(RACE_TROLL,    -5);
    _relations(RACE_HALFLING, -2);
    _relations(RACE_GNOME,     1);
    _relations(RACE_MINOTAUR, -1);
    _relations(RACE_RAKSHASA,  4);
    _relations(RACE_NAGA,      2);

    _skill(SKILL_FARMING,       2);
    _skill(SKILL_HUNTING,       4);
    _skill(SKILL_LIVESTOCK,     4);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      2);
    _skill(SKILL_CONSTRUCTION,  2);
    _skill(SKILL_TRADE,         4);
    _skill(SKILL_ESPIONAGE,     5);
    _skill(SKILL_MAGIC,         5);
    _skill(SKILL_EARTH_MAGIC,   4);
    _skill(SKILL_WATER_MAGIC,   4);
    _skill(SKILL_AIR_MAGIC,     4);
    _skill(SKILL_FIRE_MAGIC,    4);
    _skill(SKILL_LIFE_MAGIC,    4);
    _skill(SKILL_DEATH_MAGIC,   4);

    _description("\
Rakshasa are an ancient <link=race>race</link>, said to be descended from \
demons.  They resemble <link=human>humans</link> in form, but have the fur, \
head and tail of a <link=tiger>tiger</link>.  Like tigers, rakshasa are most \
at home in <link=jungle>jungles</link>, though they are also comfortable in \
other wooded areas as well as <link=mountain>mountains</link>.\n\
Rakshasa are creatures of comfort and personal freedom, and their social \
structure reflects that.  All their <link=citizens>citizens</link> demand a \
high level of <link=morale>morale</link> and relatively low <link=taxes>taxes\
</link>.  Their population also tends to have higher numbers of \
<link=merchants>merchants</link> and <link=burghers>burghers</link> than \
others, and new rakshasa cities start with several merchants.  Rakshasa's \
<link=race skills>skills</link> reflect this culture; they are generally poor \
at manual tasks, but skilled at anything that required finess or \
intelligence.  Their tiger-like nature results in a strong \
<link=hunting>hunting</link> skill, while their ability to mentally influence \
others means they are good with <link=livestock>livestock</link>.\n\
Rakshasa have a deep, innate connection to <link=magic>magic</link> and are \
the most adept spellcasters in the world.  This power goes beyond the typical \
use of <link=spell>spells</link>; rakshasa have an ever-present magical aura, \
often referred to as \"glamour,\" which manifests itself as a low-level \
psychic control over anyone who meets a rakshasa (besides other rakshasa).  As \
a result, most other races have a favorable reaction to rakshasa in \
<link=diplomacy>diplomacy</link>.  Rakshasa, for their part, are a fairly \
haughty race and tend to look down on other races, particularly those they see \
as uncultured or agrarian.  There are a few exceptions, however; \
<link=elves>elves</link> for their hauteur, <link=gnomes>gnomes</link> for \
their scientific prowess, and <link=naga>naga</link> for their affinity with \
magic.  Rakshasa consider fellow rakshasa to be beyond reproach, and attacking \
their own kind would be as unthinkable (though not unheard of) as attacking \
their own siblings.\
");


  _race(RACE_NAGA);
    _name("naga");
    _plural_name("naga");
    _adjective("naga");

    _color(c_ltgreen);
    _kingdom_color(c_brown);
    _kingdom_color(c_green);
    _kingdom_color(c_ltgray);

    _map_type(MAP_SWAMP,           100);
    _map_type(MAP_BASIN,            80);
    _map_type(MAP_DESERT,           50);
    _map_type(MAP_JUNGLE,           40);
    _map_type(MAP_FOREST,           20);
    _map_type(MAP_PLAINS,           10);

    _map_type(MAP_ICY_MOUNTAIN,   -100);
    _map_type(MAP_ICY_FOOTHILLS,  -100);
    _map_type(MAP_TUNDRA,         -100);
    _map_type(MAP_GLACIER,         -95);
    _map_type(MAP_MOUNTAINOUS,     -90);
    _map_type(MAP_CANYON,          -70);
    _map_type(MAP_WASTELAND,       -30);
    _map_type(MAP_FOOTHILLS,       -25);

    _travel_cost(MAP_DESERT,        25);
    _travel_cost(MAP_WASTELAND,     50);
    _travel_cost(MAP_FOREST,       100);
    _travel_cost(MAP_SWAMP,         30);
    _travel_cost(MAP_JUNGLE,       175);
    _travel_cost(MAP_MOUNTAINOUS,  500);
    _travel_cost(MAP_ICY_MOUNTAIN, 650);

    _cluster_size(2, 4);
    _city_size(CITY_TYPE_CITY,      80,  900);
    _city_size(CITY_TYPE_DUCHY,    600, 3500);
    _city_size(CITY_TYPE_CAPITAL, 1600, 6000);

// Kinda based on Indian city names

    _city_name_start("a", "acha", "ada", "adi", "ado", "afza", "ai", "baga",
                     "baha", "bala", "bale", "bari", "bathi", "be", "bha",
                     "bhi", "bho", "bika", "bo", "bra", "cha", "chi", "de",
                     "dha", "dhu", "fari", "faro", "go", "gobi", "gu", "guja",
                     "ha", "haji", "jala", "jha", "ka", "kari", "ke", "kha",
                     "laha", "lakhi", "lu", "madhu", "maha", "malka", "mandi",
                     "mi", "mo", "mu", "naga", "naga", "naga", "naha", "nama",
                     "nara", "nau", "nawa", "ne", "niwa", "o", "pa", "pala",
                     "pana", "para", "pau", "peri", "pili", "pra", "pudu",
                     "pura", "ra", "raja", "rama", "raya", "rewa", "sa", "sada",
                     "sadu", "sambha", "sata", "sava", "sha", "she", "sheo",
                     "shi", "sho", "si", "sika", "siva", "sri", "sri ", "sume",
                     "suri", "ta", "tali", "te", "thiru", "ti", "tiru", "tri",
                     "uda", "uma", "ura", "utha", "utta", "vada", "vidi",
                     "vikra", "ya", "yava");

    _city_name_middle("b", "br", "d", "dh", "dr", "g", "h", "hm", "k", "kud",
                      "kund", "l", "m", "mg", "mk", "mp", "mr", "n", "ng", "nk",
                      "nkl", "nt", "p", "pr", "r", "rk", "rkh", "rm", "rmir",
                      "rn", "rp", "rs", "rt", "rudr", "rund", "sh", "ss", "t",
                      "th", "thag", "tl", "tr", "v", "y");

    _city_name_end("", "", "ab", "abad", "adesh", "aka", "al", "ala", "am",
                   "an", "ana", "and", "anja", "anji", "apur", "ara", "asa",
                   "ati", "atra", "eli", "er", "esh", "i", "ia", "impa", "inga",
                   "ior", "iri", "oha", "ul", "ula", "und", "ur", "ura");

    _base_combat(10);
    _hp(100);
    _food_consumption(100);
    _warlikeness(-2);
    _life_value(3);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    6000);
    _starting_resources (RES_WOOD,    2500);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT, 100);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  40);
    _morale_requirement (CIT_BURGHER,   60);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _relations(RACE_ELF,       2);
    _relations(RACE_ORC,      -2);
    _relations(RACE_GOBLIN,   -1);
    _relations(RACE_OGRE,      1);
    _relations(RACE_TROLL,    -3);
    _relations(RACE_HALFLING,  1);
    _relations(RACE_GNOME,    -2);
    _relations(RACE_RAKSHASA,  2);
    _relations(RACE_NAGA,      2);

    _skill(SKILL_FARMING,       3);
    _skill(SKILL_HUNTING,       4);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        2);
    _skill(SKILL_FORESTRY,      4);
    _skill(SKILL_CONSTRUCTION,  2);
    _skill(SKILL_TRADE,         3);
    _skill(SKILL_ESPIONAGE,     4);
    _skill(SKILL_MAGIC,         5);
    _skill(SKILL_EARTH_MAGIC,   4);
    _skill(SKILL_WATER_MAGIC,   5);
    _skill(SKILL_AIR_MAGIC,     3);
    _skill(SKILL_FIRE_MAGIC,    2);
    _skill(SKILL_LIFE_MAGIC,    3);
    _skill(SKILL_DEATH_MAGIC,   4);

    _description("\
Naga are a serpentine <link=race>race</link>, creatures with a large snake-\
like tail and a <link=human>human</link> torso, albeit with four arms.  Nagas \
come in two breeds, Desert Naga and Swamp Naga.  Despite the names, the only \
difference between the two is in coloration; Desert Naga are tan and Swamp \
Naga are are green and blue.  All naga are most comfortable in \
<link=swamp>swamps</link>, but also fare quite well in <link=desert>deserts\
</link>.  Their serpentine form helps them travel rapidly in mud, water, and \
sand, but makes travel through rocky or uneven terrain quite difficult.\n\
Naga are a somewhat reclusive race, not given to lengthy foreign travel or \
invasions.  However, they are fiercely defensive of their homelands, and very \
wary of those who would enter their territory.  Naga have a strong sense of \
reverence for the land, particularly their ancestral homes of swamp and \
desert, and their attitude towards other races reflects this.  They are most \
receptive to relations with those races who share a similar respect for the \
land, or at least a peaceful nature in harmony with it, and are wary of those \
races who tend to ravage and exploit the world around them.\n\
Naga are natural philosophers and mages, and have a strong affinity for \
<link=magic>magic</link>.  They are particularly adept at the \
<link=magical school>school</link> of <link=water magic>water</link> magic, \
and are well-versed in <link=earth magic>earth</link> magic as well.  They \
have a darker nature as well, which puts them in touch with the school of \
<link=death magic>death</link>.\
");

}

/* Template


  _race(RACE_);
    _name("");
    _plural_name("");
    _adjective("");

    _color(c_);
    _kingdom_color(c_);
    _kingdom_color(c_);
    _kingdom_color(c_);

    _map_type(MAP_FOREST,          100);
    _map_type(MAP_BASIN,            30);
    _map_type(MAP_SWAMP,            15);
    _map_type(MAP_PLAINS,           10);

    _map_type(MAP_ICY_FOOTHILLS,  -100);
    _map_type(MAP_DESERT,          -40);
    _map_type(MAP_MOUNTAINOUS,     -30);

    _travel_cost(MAP_DESERT,        50);

    _cluster_size(2, 4);
    _city_size(CITY_TYPE_CITY,      80,  900);
    _city_size(CITY_TYPE_DUCHY,    600, 3500);
    _city_size(CITY_TYPE_CAPITAL, 1600, 6000);

    _city_name_start( );

    _city_name_middle( );

    _city_name_end( );

    _base_combat(10);
    _hp(100);
    _food_consumption(100);
    _warlikeness(-2);
    _life_value(3);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,    2000);
    _starting_resources (RES_STONE,   2000);

    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  90);
    _birth_rate         (CIT_BURGHER,   80);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   70);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _relations(RACE_HUMAN,     1);
    _relations(RACE_DWARF,     1);

    _skill(SKILL_FARMING,       3);
    _skill(SKILL_HUNTING,       3);
    _skill(SKILL_LIVESTOCK,     3);
    _skill(SKILL_MINING,        3);
    _skill(SKILL_FORESTRY,      3);
    _skill(SKILL_CONSTRUCTION,  3);
    _skill(SKILL_TRADE,         3);

*/
