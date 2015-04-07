#include "animal.h"

Animal_datum* Animal_data[ANIMAL_MAX];

#define _animal(x) \
  cur_id = (x)

#define _name(n) \
  Animal_data[cur_id]->name = (n)

#define _name_plural(n) \
  Animal_data[cur_id]->name_plural = (n)

#define _temp_range(a, b) \
  Animal_data[cur_id]->min_temp = (a); Animal_data[cur_id]->max_temp = (b)

#define _altitude_range(a, b) \
  Animal_data[cur_id]->min_altitude = (a); \
  Animal_data[cur_id]->max_altitude = (b) 

#define _rainfall_range(a, b) \
  Animal_data[cur_id]->min_rainfall = (a); \
  Animal_data[cur_id]->max_rainfall = (b)

#define _percentage(n) \
  Animal_data[cur_id]->percentage = (n)

#define _default_action(a) \
  Animal_data[cur_id]->default_action = (a)

#define _danger(n) \
  Animal_data[cur_id]->danger = (n)

#define _difficulty(n) \
  Animal_data[cur_id]->difficulty = (n)

#define _hp(n) \
  Animal_data[cur_id]->hp = (n)

#define _flee_chance(n) \
  Animal_data[cur_id]->flee_chance = (n)

#define _pack_chance(n) \
  Animal_data[cur_id]->pack_chance = (n)

#define _max_pack_size(n) \
  Animal_data[cur_id]->max_pack_size = (n)

#define _reproduction(n) \
  Animal_data[cur_id]->reproduction_rate = (n)

#define _hardiness(n) \
  Animal_data[cur_id]->hardiness = (n)

#define _food_killed(n) \
  Animal_data[cur_id]->food_killed = (n)

#define _food_livestock(n) \
  Animal_data[cur_id]->food_livestock = (n)

#define _carnivore() \
  Animal_data[cur_id]->carnivore = true

#define _tameness(n) \
  Animal_data[cur_id]->tameness = (n)

#define _size(n) \
  Animal_data[cur_id]->size = (n)

#define _resource_killed(t, n) \
  Animal_data[cur_id]->resources_killed.push_back( Resource_amount((t), (n)) )

#define _resource_livestock(t, n) \
  Animal_data[cur_id]->resources_livestock.push_back(Resource_amount((t), (n)))

void init_animal_data()
{
  for (int i = 0; i < ANIMAL_MAX; i++) {
    Animal_data[i] = new Animal_datum;
    Animal_data[i]->uid = i;
  }

  int cur_id;

// LIVESTOCK

  _animal(ANIMAL_CHICKEN);
    _name("chicken");
    _name_plural("chickens");
    _temp_range     ( 25,  80);
    _altitude_range (  0,  85);
    _rainfall_range ( 15,  50);
    _percentage(85);
    _default_action(ANIMAL_ACT_CAPTURE);
    _difficulty(9);
    _hp(5);
    _pack_chance(80);
    _max_pack_size(8);
    _reproduction(800);
    _hardiness(1800);
    _food_killed(2);
    _food_livestock(80);
    _tameness(100);
    _size(1);

  _animal(ANIMAL_PIG);
    _name("pig");
    _name_plural("pigs");
    _temp_range     ( 35,  75);
    _altitude_range (  0,  85);
    _rainfall_range ( 20,  70);
    _percentage(70);
    _default_action(ANIMAL_ACT_CAPTURE);
    _difficulty(260);
    _hp(70);
    _pack_chance(70);
    _max_pack_size(5);
    _reproduction(1000);
    _hardiness(2800);
    _food_killed(60);
    _tameness(92);
    _size(3);

  _animal(ANIMAL_SHEEP);
    _name("sheep");
    _name_plural("sheep");
    _temp_range     ( 10,  60);
    _altitude_range (  0,  95);
    _rainfall_range ( 15,  60);
    _percentage(60);
    _default_action(ANIMAL_ACT_CAPTURE);
    _difficulty(225);
    _hp(50);
    _pack_chance(70);
    _max_pack_size(5);
    _reproduction(1100);
    _hardiness(2500);
    _food_killed(50);
    _tameness(98);
    _resource_killed(RES_FIBER, 50);
    _resource_livestock(RES_FIBER, 15);
    _size(3);

  _animal(ANIMAL_COW);
    _name("cow");
    _name_plural("cows");
    _temp_range     ( 20,  85);
    _altitude_range (  0,  85);
    _rainfall_range ( 15,  60);
    _percentage(60);
    _default_action(ANIMAL_ACT_CAPTURE);
    _difficulty(1250);
    _hp(120);
    _pack_chance(60);
    _max_pack_size(3);
    _reproduction(1400);
    _hardiness(3500);
    _food_killed(250);
    _food_livestock(75);
    _tameness(100);
    _resource_killed(RES_LEATHER, 10);
    _size(6);

  _animal(ANIMAL_HORSE);
    _name("horse");
    _name_plural("horses");
    _temp_range     ( 20,  80);
    _altitude_range (  0,  80);
    _rainfall_range ( 15,  60);
    _percentage(30);
    _default_action(ANIMAL_ACT_CAPTURE);
    _pack_chance(45);
    _max_pack_size(3);
    _danger(6);
    _difficulty(1200);
    _hp(140);
    _reproduction(2000);
    _hardiness(4000);
    _food_killed(200);
    _tameness(80);
    _size(10);
    _resource_killed(RES_LEATHER, 8);


// STANDARD GAME (can also be livestock sometimes!)

  _animal(ANIMAL_HARE);
    _name("hare");
    _name_plural("hares");
    _temp_range     ( 10,  60);
    _altitude_range (  0,  90);
    _rainfall_range ( 10,  75);
    _percentage(90);
    _default_action(ANIMAL_ACT_KILL);
    _difficulty(8);
    _hp(5);
    _pack_chance(25);
    _max_pack_size(3);
    _reproduction(300);
    _hardiness(800);
    _food_killed(2);
    _tameness(75);
    _size(1);
    //_resource_killed(RES_FUR, 1);

  _animal(ANIMAL_FOX);
    _name("fox");
    _name_plural("foxes");
    _temp_range     (  5,  90);
    _altitude_range (  0,  85);
    _rainfall_range (  5,  60);
    _percentage(70);
    _default_action(ANIMAL_ACT_KILL);
    _difficulty(70);
    _hp(30);
    _pack_chance(5);
    _max_pack_size(2);
    _reproduction(1400);
    _hardiness(2000);
    _food_killed(15);
    _carnivore();
    _tameness(35);
    _size(4);
    //_resource_killed(RES_FUR, 1);

  _animal(ANIMAL_BOAR);
    _name("boar");
    _name_plural("boars");
    _temp_range     ( 35,  90);
    _altitude_range (  0,  60);
    _rainfall_range (  0,  70);
    _percentage(60);
    _default_action(ANIMAL_ACT_KILL);
    _pack_chance(35);
    _max_pack_size(5);
    _danger(10);
    _difficulty(350);
    _hp(70);
    _flee_chance(98);
    _reproduction(1600);
    _hardiness(3400);
    _food_killed(75);
    _tameness(25);
    _size(5);

  _animal(ANIMAL_DEER);
    _name("deer");
    _name_plural("deer");
    _temp_range     ( 12,  85);
    _altitude_range (  0,  90);
    _rainfall_range ( 10,  60);
    _percentage(80);
    _default_action(ANIMAL_ACT_KILL);
    _difficulty(400);
    _hp(110);
    _pack_chance(30);
    _max_pack_size(3);
    _reproduction(1800);
    _hardiness(3000);
    _food_killed(80);
    _tameness(30);
    _size(10);
    //_resource_killed(RES_LEATHER, 6);


// DANGEROUS GAME

  _animal(ANIMAL_BEAR);
    _name("bear");
    _name_plural("bears");
    _temp_range     (  0,  55);
    _altitude_range (  5,  80);
    _rainfall_range ( 15,  65);
    _percentage(70);
    _default_action(ANIMAL_ACT_FLEE);
    _danger(12);
    _difficulty(1400);
    _hp(110);
    _flee_chance(92);
    _reproduction(2400);
    _hardiness(3400);
    _food_killed(300);
    _tameness(15);
    _size(14);
    _resource_killed(RES_FUR, 5);

  _animal(ANIMAL_WOLF);
    _name("wolf");
    _name_plural("wolves");
    _temp_range     (  5,  60);
    _altitude_range (  0,  75);
    _rainfall_range ( 15,  60);
    _percentage(60);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(55);
    _max_pack_size(5);
    _danger(16);
// Animals that seek YOU out are less difficult (i.e. less time-consuming)
    _difficulty(300);
    _hp(85);
    _flee_chance(90);
    _reproduction(2000);
    _hardiness(3000);
    _food_killed(80);
    _carnivore();
    _tameness(12);
    _size(10);
    _resource_killed(RES_FUR, 3);

  _animal(ANIMAL_TIGER);
    _name("tiger");
    _name_plural("tigers");
    _temp_range     ( 60, 120);
    _altitude_range (  0,  80);
    _rainfall_range (  5,  90);
    _percentage(30);
    _default_action(ANIMAL_ACT_FLEE);
    _danger(15);
    _difficulty(500);
    _hp(80);
    _flee_chance(92);
    _reproduction(2800);
    _hardiness(3200);
    _food_killed(115);
    _carnivore();
    _tameness(8);
    _size(8);
    //_resource_killed(RES_LEATHER,  8);

  _animal(ANIMAL_LION);
    _name("lion");
    _name_plural("lions");
    _temp_range     ( 70, 110);
    _altitude_range (  0,  50);
    _rainfall_range (  0,  25);
    _percentage(35);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(45);
    _max_pack_size(4);
    _danger(18);
    _difficulty(950);
    _hp(110);
    _flee_chance(85);
    _reproduction(2600);
    _hardiness(3500);
    _food_killed(250);
    _carnivore();
    _tameness(10);
    _size(12);
    //_resource_killed(RES_LEATHER, 12);

// Legit question: could we drink elephant milk?
  _animal(ANIMAL_ELEPHANT);
    _name("elephant");
    _name_plural("elephants");
    _temp_range     ( 65, 100);
    _altitude_range (  0,  60);
    _rainfall_range (  0,  30);
    _percentage(25);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(60);
    _max_pack_size(4);
    _danger(40);
    _difficulty(4500);
    _hp(250);
    _flee_chance(96);
    _reproduction(5000);
    _hardiness(7000);
    _food_killed(1500);
    _tameness(4);
    _size(100);
    _resource_killed(RES_LEATHER, 40);


// FANTASY CREATURES

  _animal(ANIMAL_JELLY);
    _name("jelly");
    _name_plural("jellies");
    _temp_range     ( 32,  75);
    _altitude_range (  0,  80);
    _rainfall_range ( 30,  90);
    _percentage(50);
    _default_action(ANIMAL_ACT_KILL);
    _pack_chance(20);
    _max_pack_size(4);
    _danger(3);
    _difficulty(50);
    _hp(50);
    _flee_chance(98);
    _reproduction(100);
    _hardiness(600);
    _food_killed(0);
    _tameness(50);
    _size(4);
// TODO: Should jellies provide some kind of resource?  Right now they're
//       worthless, just an annoyance.

  _animal(ANIMAL_MAN_EATING_PLANT);
  _name("man-eating plant");
  _name_plural("man-eating plants");
  _temp_range     ( 60, 120);
  _altitude_range (  0,  80);
  _rainfall_range ( 50, 100);
  _percentage(30);
  _default_action(ANIMAL_ACT_FLEE);
  _danger(30);
// Relatively low difficulty; you don't HUNT them so much as you FIND them.
  _difficulty(1300);
  _hp(160);
  _flee_chance(100);
  _reproduction(5000);
  _hardiness(15000);
  _food_killed(300);
  _tameness(0); // Can't be captured!
  _size(25);
  _resource_killed(RES_FIBER, 25);
  

  _animal(ANIMAL_DIRE_WOLF);
    _name("dire wolf");
    _name_plural("dire wolves");
    _temp_range     (  2,  40);
    _altitude_range (  0,  90);
    _rainfall_range (  5,  70);
    _percentage(20);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(20);
    _max_pack_size(3);
    _danger(30);
    _difficulty(600);
    _hp(110);
    _flee_chance(60);
    _reproduction(3500);
    _hardiness(5000);
    _food_killed(180);
    _carnivore();
    _tameness(3);
    _size(20);
    _resource_killed(RES_FUR, 10);

// Inuit legend - half orca, half wolf, amphibious
  _animal(ANIMAL_AKHLUT);
    _name("akhlut");
    _name_plural("akhlut");
    _temp_range     (  0,  30);
    _altitude_range (-20,  20);
    _rainfall_range (  0,  80);
    _percentage(10);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(15);
    _max_pack_size(2);
    _danger(40);
    _difficulty(1300);
    _hp(130);
    _flee_chance(40);
    _reproduction(3800);
    _hardiness(9000);
    _food_killed(320);
    _carnivore();
    _tameness(2);
    _size(50);
    _resource_killed(RES_FUR,     10);
    _resource_killed(RES_LEATHER, 10);

  _animal(ANIMAL_MANTICORE);
    _name("manticore");
    _name_plural("manticores");
    _temp_range     ( 50, 110);
    _altitude_range ( 60, 100);
    _rainfall_range (  0,  40);
    _percentage(10);
    _default_action(ANIMAL_ACT_FLEE);
    _danger(42);
    _difficulty(1600);
    _hp(150);
    _flee_chance(25);
    _reproduction(3600);
    _hardiness(7500);
    _food_killed(400);
    _carnivore();
    _tameness(5);
    _size(100);

  _animal(ANIMAL_UNICORN);
    _name("unicorn");
    _name_plural("unicorns");
    _temp_range     ( 30,  65);
    _altitude_range (  0,  95);
    _rainfall_range ( 40,  60);
    _percentage(5);
    _default_action(ANIMAL_ACT_FLEE);
    _danger(32);
// Unicorns are notoriously difficulty to find...
    _difficulty(3500);
    _hp(120);
    _flee_chance(70);
    _reproduction(6000);
    _hardiness(9000);
    _food_killed(200);
    _tameness(10);
    _size(25);
    _resource_killed(RES_UNICORN_HORN, 1);

  _animal(ANIMAL_BASILISK);
    _name("basilisk");
    _name_plural("basilisks");
    _temp_range     ( 60, 110);
    _altitude_range (  0, 100);
    _rainfall_range (  0,  40);
    _percentage(8);
    _default_action(ANIMAL_ACT_FLEE);
    _pack_chance(15);
    _max_pack_size(3);
    _danger(50);
// They're fairly reclusive
    _difficulty(500);
    _hp(70);
    _flee_chance(92);
    _reproduction(6000);
    _hardiness(8000);
    _food_killed(100);
    _tameness(0);
    _size(10);

  _animal(ANIMAL_WYVERN);
    _name("wyvern");
    _name_plural("wyverns");
    _temp_range     ( 30,  90);
    _altitude_range ( 70, 100);
    _rainfall_range ( 10,  70);
    _percentage(3);
    _default_action(ANIMAL_ACT_FLEE);
    _danger(60);
    _difficulty(6000);
    _hp(300);
    _flee_chance(15);
    _reproduction(7000);
    _hardiness(12000);
    _food_killed(1600);
    _carnivore();
    _tameness(1);
    _size(250);

}
