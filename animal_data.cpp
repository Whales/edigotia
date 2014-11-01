#include "animal.h"

Animal_datum* Animal_data[ANIMAL_MAX];

#define _animal(x) \
  cur_id = (x)

#define _name(n) \
  Animal_data[cur_id]->name = (n)

#define _temp_range(a, b) \
  Animal_data[cur_id]->min_temp = (a); Animal_data[cur_id]->max_temp = (b)

#define _altitude_range(a, b) \
  Animal_data[cur_id]->min_altitude = (a); \
  Animal_data[cur_id]->max_altitude = (b) 

#define _rainfall_range(a, b) \
  Animal_data[cur_id]->min_rainfall = (a); \
  Animal_data[cur_id]->max_rainfall = (b)

#define _danger(n) \
  Animal_data[cur_id]->danger = (n)

#define _reproduction(n) \
  Animal_data[cur_id]->reproduction_rate = (n)

#define _hardiness(n) \
  Animal_data[cur_id]->hardiness = (n)

#define _food_killed(n) \
  Animal_data[cur_id]->food_killed = (n)

#define _food_livestock(n) \
  Animal_data[cur_id]->food_livestock = (n)

#define _food_eaten(n) \
  Animal_data[cur_id]->food_eaten = (n)

#define _carnivore() \
  Animal_data[cur_id]->carnivore = true

#define _tameness(n) \
  Animal_data[cur_id]->tameness = (n)

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
    _temp_range     ( 25,  80);
    _altitude_range (  0,  85);
    _rainfall_range ( 15,  50);
    _reproduction(45);
    _hardiness(1800);
    _food_killed(2);
    _food_livestock(80);
    _food_eaten(10);
    _tameness(100);

  _animal(ANIMAL_PIG);
    _name("pig");
    _temp_range     ( 35,  75);
    _altitude_range (  0,  85);
    _rainfall_range ( 20,  70);
    _reproduction(32);
    _hardiness(2800);
    _food_killed(60);
    _food_eaten(50);
    _tameness(92);

  _animal(ANIMAL_SHEEP);
    _name("sheep");
    _temp_range     ( 10,  60);
    _altitude_range (  0,  95);
    _rainfall_range ( 15,  60);
    _reproduction(30);
    _hardiness(2500);
    _food_killed(50);
    _food_eaten(40);
    _tameness(98);
    _resource_killed(RES_FIBER, 50);
    _resource_livestock(RES_FIBER, 15);

  _animal(ANIMAL_COW);
    _name("cow");
    _temp_range     ( 20,  85);
    _altitude_range (  0,  85);
    _rainfall_range ( 15,  60);
    _reproduction(30);
    _hardiness(3500);
    _food_killed(250);
    _food_livestock(75);
    _food_eaten(100);
    _tameness(100);
    _resource_killed(RES_LEATHER, 10);

  _animal(ANIMAL_HORSE);
    _name("horse");
    _temp_range     ( 20,  80);
    _altitude_range (  0,  80);
    _rainfall_range ( 15,  60);
    _danger(1);
    _reproduction(20);
    _hardiness(4000);
    _food_killed(200);
    _food_eaten(180);
    _tameness(80);
    _resource_killed(RES_LEATHER, 8);


// STANDARD GAME (can also be livestock sometimes!)

  _animal(ANIMAL_HARE);
    _name("hare");
    _temp_range     ( 10,  60);
    _altitude_range (  0,  90);
    _rainfall_range ( 10,  75);
    _reproduction(50);
    _hardiness(800);
    _food_killed(2);
    _food_eaten(12);
    _tameness(75);
    _resource_killed(RES_FUR, 1);

  _animal(ANIMAL_FOX);
    _name("fox");
    _temp_range     (  5,  90);
    _altitude_range (  0,  85);
    _rainfall_range (  5,  60);
    _reproduction(25);
    _hardiness(2000);
    _food_killed(15);
    _food_eaten(50);
    _carnivore();
    _tameness(40);
    _resource_killed(RES_FUR, 5);

  _animal(ANIMAL_BOAR);
    _name("boar");
    _temp_range     ( 35,  90);
    _altitude_range (  0,  60);
    _rainfall_range (  0,  70);
    _danger(5);
    _reproduction(28);
    _hardiness(3400);
    _food_killed(75);
    _food_eaten(50);
    _tameness(30);

  _animal(ANIMAL_DEER);
    _name("deer");
    _temp_range     ( 12,  85);
    _altitude_range (  0,  90);
    _rainfall_range ( 10,  60);
    _reproduction(20);
    _hardiness(3000);
    _food_killed(80);
    _food_eaten(120);
    _tameness(35);
    _resource_killed(RES_LEATHER, 6);


// DANGEROUS GAME

  _animal(ANIMAL_BEAR);
    _name("bear");
    _temp_range     (  0,  55);
    _altitude_range (  5,  80);
    _rainfall_range ( 15,  65);
    _danger(10);
    _reproduction(18);
    _hardiness(3400);
    _food_killed(300);
    _food_eaten(200);
    _tameness(10);
    _resource_killed(RES_FUR, 15);

  _animal(ANIMAL_WOLF);
    _name("wolf");
    _temp_range     (  5,  60);
    _altitude_range (  0,  75);
    _rainfall_range ( 15,  60);
    _danger(8);
    _reproduction(25);
    _hardiness(3000);
    _food_killed(80);
    _food_eaten(100);
    _carnivore();
    _tameness(12);
    _resource_killed(RES_FUR, 10);

  _animal(ANIMAL_LION);
    _name("lion");
    _temp_range     ( 70, 110);
    _altitude_range (  0,  50);
    _rainfall_range (  0,  25);
    _danger(12);
    _reproduction(15);
    _hardiness(3500);
    _food_killed(250);
    _food_eaten(250);
    _carnivore();
    _tameness(5);
    _resource_killed(RES_LEATHER, 12);

// Legit question: could we drink elephant milk?
  _animal(ANIMAL_ELEPHANT);
    _name("elephant");
    _temp_range     ( 65, 100);
    _altitude_range (  0,  60);
    _rainfall_range (  0,  30);
    _danger(16);
    _reproduction(8);
    _hardiness(7000);
    _food_killed(1500);
    _food_eaten(800);
    _tameness(8);
    _resource_killed(RES_LEATHER, 40);


// FANTASY CREATURES

  _animal(ANIMAL_JELLY);
    _name("jelly");
    _temp_range     ( 32,  75);
    _altitude_range (  0,  80);
    _rainfall_range ( 40,  90);
    _danger(2);
    _reproduction(50);
    _hardiness(600);
    _food_killed(0);
    _food_eaten(150);
    _tameness(50);
// TODO: Should jellies provide some kind of resource?  Right now they're
//       worthless.

  _animal(ANIMAL_DIRE_WOLF);
    _name("dire wolf");
    _temp_range     (  2,  60);
    _altitude_range (  0,  90);
    _rainfall_range ( 15,  70);
    _danger(16);
    _reproduction(18);
    _hardiness(5000);
    _food_killed(180);
    _food_eaten(250);
    _carnivore();
    _tameness(4);
    _resource_killed(RES_FUR, 20);

  _animal(ANIMAL_MANTICORE);
    _name("manticore");
    _temp_range     ( 50, 110);
    _altitude_range ( 60, 100);
    _rainfall_range (  0,  40);
    _danger(18);
    _reproduction(15);
    _hardiness(7500);
    _food_killed(400);
    _food_eaten(350);
    _carnivore();
    _tameness(5);

  _animal(ANIMAL_UNICORN);
    _name("unicorn");
    _temp_range     ( 30,  65);
    _altitude_range (  0,  95);
    _rainfall_range ( 40,  60);
    _danger(15);
    _reproduction(5);
    _hardiness(8000);
    _food_killed(200);
    _food_eaten(180);
    _tameness(10);
    _resource_killed(RES_UNICORN_HORN, 1);

  _animal(ANIMAL_BASILISK);
    _name("basilisk");
    _temp_range     ( 60, 110);
    _altitude_range (  0, 100);
    _rainfall_range (  0,  40);
    _danger(20);
    _reproduction(8);
    _hardiness(7000);
    _food_killed(100);
    _food_eaten(100);
    _tameness(0);

  _animal(ANIMAL_WYVERN);
    _name("wyvern");
    _temp_range     ( 30,  90);
    _altitude_range ( 70, 100);
    _rainfall_range ( 10,  70);
    _danger(22);
    _reproduction(15);
    _hardiness(9000);
    _food_killed(1600);
    _food_eaten(1000);
    _carnivore();
    _tameness(2);

}
