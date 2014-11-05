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

#define _cluster_size(a, b) \
  Race_data[cur_id]->cluster_min = (a); Race_data[cur_id]->cluster_max = (b)

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

#define _skill(t, n) \
  Race_data[cur_id]->skill_level[ (t) ] = (n)

void init_races()
{
  for (int i = 0; i < RACE_MAX; i++) {
    Race_data[i] = new Race_datum();
    Race_data[i]->uid = i;
  }

  int cur_id = 0;

  _race(RACE_HUMAN);
    _name("human");
    _plural_name("humans");
    _adjective("human");

    _color(c_white);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_blue);
    _kingdom_color(c_cyan);
/*
    _kingdom_color(c_white);
    _kingdom_color(c_ltblue);
    _kingdom_color(c_ltcyan);
    _kingdom_color(c_yellow);
*/

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

    _city_name_start("al", "ba", "bir", "cam", "car", "chel", "co", "da", "der",
                     "ex", "glo", "her", "lam", "le", "li", "lo", "nor",
                     "ox", "pe", "pre", "sa", "so", "sum", "tru", "wa",
                     "wim", "wol", "yor");

    _city_name_middle("", "", "can", "ces", "der", "el", "er", "fa", "ga",
                      "ing", "ke", "ler", "man", "mer", "ming", "nor", "per",
                      "sing", "ter", "til", "ting", "to", "ven", "ver", "ving");

// Use repeats to make an ending more likely.
    _city_name_end("borough", "borough", "burg", "burg", "burg", "bury", "bury",
                   "bury", "by", "by", "caster", "cester", "don", "field",
                   "field", "ford", "ford", "forth", "gale", "ham", "ham",
                   "ham", "ia", "kirk", "mouth", "neck", "over", "pool", "pool",
                   "sex", "ter", "tol", "ton", "ton", "vale", "ville");

    _base_combat(10);
    _hp(100);
    _food_consumption(100);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,     110);
    _starting_resources (RES_STONE,    110);

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

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_HUNTING,   3);
    _skill(SKILL_LIVESTOCK, 3);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  3);


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

    _cluster_size(2, 4);

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

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,     120);
    _starting_resources (RES_STONE,    100);

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

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_HUNTING,   5);
    _skill(SKILL_LIVESTOCK, 3);
    _skill(SKILL_MINING,    2);
    _skill(SKILL_FORESTRY,  5);


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

    _cluster_size(2, 5);

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

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    6000);
    _starting_resources (RES_WOOD,     100);
    _starting_resources (RES_STONE,    100);

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

    _skill(SKILL_FARMING,   2);
    _skill(SKILL_HUNTING,   2);
    _skill(SKILL_LIVESTOCK, 2);
    _skill(SKILL_MINING,    5);
    _skill(SKILL_FORESTRY,  2);


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

    _cluster_size(4, 6);

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

    _starting_population(CIT_PEASANT,  120);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,     100);
    _starting_resources (RES_STONE,    100);

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

    _skill(SKILL_FARMING,   1);
    _skill(SKILL_HUNTING,   4);
    _skill(SKILL_LIVESTOCK, 2);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  3);


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

    _cluster_size(5, 8);

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

    _starting_population(CIT_PEASANT,  140);
    _starting_resources (RES_GOLD,    4000);
    _starting_resources (RES_WOOD,     100);
    _starting_resources (RES_STONE,    100);

    _birth_rate         (CIT_PEASANT,   90);
    _birth_rate         (CIT_MERCHANT,  90);
    _birth_rate         (CIT_BURGHER,   90);
    _citizen_ratio      (CIT_MERCHANT,  14);
    _citizen_ratio      (CIT_BURGHER,   12);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   75);
    _low_tax_rate       (CIT_PEASANT,   25);
    _low_tax_rate       (CIT_MERCHANT,  20);
    _low_tax_rate       (CIT_BURGHER,   15);
    _high_tax_rate      (CIT_PEASANT,   90);
    _high_tax_rate      (CIT_MERCHANT,  80);
    _high_tax_rate      (CIT_BURGHER,   60);

    _skill(SKILL_FARMING,   3);
    _skill(SKILL_HUNTING,   2);
    _skill(SKILL_LIVESTOCK, 1);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  2);



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

    _cluster_size(1, 3);

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

    _base_combat(15);
    _hp(150);
    _food_consumption(140);

    _starting_population(CIT_PEASANT,   90);
    _starting_resources (RES_GOLD,    4500);
    _starting_resources (RES_WOOD,     130);
    _starting_resources (RES_STONE,    130);

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

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_HUNTING,   2);
    _skill(SKILL_LIVESTOCK, 4);
    _skill(SKILL_MINING,    4);
    _skill(SKILL_FORESTRY,  4);



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

    _cluster_size(1, 2);

    _city_name_start("ak", "ar", "ark", "buk", "bur", "burk", "duk", "dar",
                     "dark", "dur", "dor", "durk", "ek", "er", "hak", "har",
                     "hark", "hek", "hur", "hurk", "lar", "luk", "lor", "lork",
                     "lur", "mak", "mar", "mark", "mor", "muk", "mur", "murk",
                     "rak", "rek", "rok", "ruk", "tak", "tar", "tark", "tek",
                     "ter", "terk", "tuk", "turk", "uk", "ur", "urk");

    _city_name_middle(" A", " Ba", " Be", " Bu", " Da", " De", " Du", " E",
                      " Ha", " He", " Hu", " La", " Le", " Lu", " Ma", " Me",
                      " Mu", " Ra", " Re", " Ro", " Ru", " Ta", " Te", " Tu",
                      " U", "a", "e", "l", "r", "t", "u");

    _city_name_end("", "k", "k", "k", "k", "k", "r", "r", "r", "rk", "rk");

    _base_combat(20);
    _hp(200);
    _food_consumption(180);

    _starting_population(CIT_PEASANT,   20);
    _starting_resources (RES_GOLD,    6000);
    _starting_resources (RES_WOOD,     100);
    _starting_resources (RES_STONE,    150);

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

    _skill(SKILL_FARMING,   1);
    _skill(SKILL_HUNTING,   4);
    _skill(SKILL_LIVESTOCK, 1);
    _skill(SKILL_MINING,    2);
    _skill(SKILL_FORESTRY,  1);



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

    _cluster_size(3, 4);

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
    _food_consumption(120);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,     120);
    _starting_resources (RES_STONE,    100);

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

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_HUNTING,   1);
    _skill(SKILL_LIVESTOCK, 3);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  4);

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

    _map_type(MAP_FOREST,      100);
    _map_type(MAP_BASIN,        30);
    _map_type(MAP_SWAMP,        15);
    _map_type(MAP_PLAINS,       10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_MOUNTAINOUS, -30);

    _cluster_size(2, 4);

    _city_name_start( );

    _city_name_middle( );

    _city_name_end( );

    _food_consumption(100);

    _starting_population(CIT_PEASANT,  100);
    _starting_resources (RES_GOLD,    5000);
    _starting_resources (RES_WOOD,     100);
    _starting_resources (RES_STONE,    100);

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

    _skill(SKILL_FARMING,   3);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  3);

*/
