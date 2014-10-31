#include "animal.h"

Animal_datum* Animal_data[ANIMAL_MAX];

#define _animal(x) \
  cur_id = (x)

#define _name(n) \
  Animal_data[cur_id]->name = (n)

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
    Animal_data[i] = new Animal_data;
    Animal_data[i]->uid = i;
  }

  int cur_id;

// LIVESTOCK

  _animal(ANIMAL_CHICKEN);
    _name("chicken");
    _reproduction(45);
    _hardiness(1800);
    _food_killed(2);
    _food_livestock(80);
    _food_eaten(10);
    _tameness(100);

  _animal(ANIMAL_PIG);
    _name("pig");
    _reproduction(32);
    _hardiness(2800);
    _food_killed(60);
    _food_eaten(50);
    _tameness(92);

  _animal(ANIMAL_SHEEP);
    _name("sheep");
    _reproduction(30);
    _hardiness(2500);
    _food_killed(50);
    _food_eaten(40);
    _tameness(98);
    _resource_killed(RES_FIBER, 50);
    _resource_livestock(RES_FIBER, 15);

  _animal(ANIMAL_COW);
    _name("cow");
    _reproduction(30);
    _hardiness(3500);
    _food_killed(250);
    _food_livestock(75);
    _food_eaten(100);
    _tameness(100);
    _resource_killed(RES_LEATHER, 10);

  _animal(ANIMAL_HORSE);
    _name("horse");
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
    _reproduction(50);
    _hardiness(800);
    _food_killed(2);
    _food_eaten(12);
    _tameness(75);
    _resource_killed(RES_FUR, 1);

  _animal(ANIMAL_FOX);
    _name("fox");
    _reproduction(25);
    _hardiness(2000);
    _food_killed(15);
    _food_eaten(50);
    _carnivore();
    _tameness(40);
    _resource_killed(RES_FUR, 5);

  _animal(ANIMAL_BOAR);
    _name("boar");
    _danger(5);
    _reproduction(28);
    _hardiness(3400);
    _food_killed(75);
    _food_eaten(50);
    _tameness(30);

  _animal(ANIMAL_DEER);
    _name("deer");
    _reproduction(20);
    _hardiness(3000);
    _food_killed(80);
    _food_eaten(120);
    _tameness(35);
    _resource_killed(RES_LEATHER, 6);


// DANGEROUS GAME

  _animal(ANIMAL_BEAR);
    _name("bear");
    _danger(10);
    _reproduction(18);
    _hardiness(3400);
    _food_killed(300);
    _food_eaten(200);
    _tameness(10);
    _resource_killed(RES_FUR, 15);

  _animal(ANIMAL_WOLF);
    _name("wolf");
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
    _danger(2);
    _reproduction(50);
    _hardiness(600);
    _food_killed(0);
    _food_eaten(150);
    _tameness(
