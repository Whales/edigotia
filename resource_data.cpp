#include "resource.h"

Resource_datum* Resource_data [RES_MAX];
Crop_datum*     Crop_data     [CROP_MAX];
Mineral_datum*  Mineral_data  [MINERAL_MAX];

// RESOURCE MACROS
#define _resource(n) \
  cur_id = (n)
#define _rname(s) \
  Resource_data[cur_id]->name = (s)
#define _rcolor(c) \
  Resource_data[cur_id]->color = (c)
#define _rvalue(n) \
  Resource_data[cur_id]->value = (n)
#define _luxury(t) \
  Resource_data[cur_id]->luxury_type = (t)
#define _demand(n) \
  Resource_data[cur_id]->demand = (n)
#define _morale(n) \
  Resource_data[cur_id]->morale = (n)
#define _meta() \
  Resource_data[cur_id]->meta = true
#define _description(s) \
  Resource_data[cur_id]->description = (s)

// CROP MACROS
#define _crop(n) \
  cur_id = (n)
#define _cname(s) \
  Crop_data[cur_id]->name = (s)
// percentage is the frequency of placement; what % of terrain should it cover
#define _cpercent(s) \
  Crop_data[cur_id]->percentage = (s)
#define _temperature(a, b) \
  Crop_data[cur_id]->min_temp = (a); \
  Crop_data[cur_id]->max_temp = (b)
#define _altitude(a, b) \
  Crop_data[cur_id]->min_altitude = (a); \
  Crop_data[cur_id]->max_altitude = (b)
#define _rainfall(a, b) \
  Crop_data[cur_id]->min_rainfall = (a); \
  Crop_data[cur_id]->max_rainfall = (b)
#define _food(s) \
  Crop_data[cur_id]->food = (s)
#define _bonus(t, a) \
  Crop_data[cur_id]->bonus_resources.push_back( Resource_amount ( (t), (a) ) )
#define _type(s) \
  Crop_data[cur_id]->type = (s)
#define _cdescription(s) \
  Crop_data[cur_id]->description = (s)

// MINERAL MACROS
#define _mineral(n) \
  cur_id = (n)
#define _mpercent(s) \
  Mineral_data[cur_id]->percentage = (s)
#define _mvalue(s) \
  Mineral_data[cur_id]->value = (s)
#define _mname(s) \
  Mineral_data[cur_id]->name = (s)
#define _mcolor(c) \
  Mineral_data[cur_id]->color = (c)
#define _mhidden() \
  Mineral_data[cur_id]->hidden = true
#define _mdescription(s) \
  Mineral_data[cur_id]->description = (s)


void init_resource_data()
{
  for (int i = 0; i < RES_MAX; i++) {
    Resource_data[i] = new Resource_datum();
  }
  for (int i = 0; i < CROP_MAX; i++) {
    Crop_data[i] = new Crop_datum();
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral_data[i] = new Mineral_datum();
  }


  int cur_id = 0;

  _resource(RES_NULL);
    _rname("any resource");
    _rcolor(c_dkgray);

  _resource(RES_GOLD);
    _rname("gold");
    _rvalue(100);
    _rcolor(c_yellow);
    _description("\
Gold is the universally accepted currency of Edigotia.  This is its processed \
form, as opposed to the <link=mineral>ore</link> mined out of the ground \
(which can be <link=smeltery>smelted</link> to make usable gold).  While gold \
is not required for trade, it does greatly facilitate it.  Also, it is a \
necessary <link=resources>resource</link> for building many <link=area>areas\
</link> and <link=building>buildings</link>, as well as for paying the wages \
of workers.  Gold can be acquired from your citizens via <link=taxes>taxation\
</link>.\
");

  _resource(RES_FOOD);
    _rname("food");
    _rvalue(20);
    _rcolor(c_green);
    _description("\
Food is one of the most basic needs for a <link=city>city</link> to thrive.  \
Without an adequate and continuous supply of food, your <link=citizens>citizens\
</link> will go hungry, their health will suffer, and eventually they will die \
of starvation.  Fortunately, there are several sources of food; \
<link=farming>farming</link> and <link=hunting>hunting</link> are available to \
any city with the proper <link=city map tile>terrain</link> (so long as your \
<link=race>race</link> is competent at one or both of these).  As food is \
relatively inexpensive, even a modest economy can <link=trade>trade</link> for \
it.\
");

  _resource(RES_WOOD);
    _rname("wood");
    _rvalue(30);
    _rcolor(c_ltred);
    _description("\
Wood is an important building material, and most new <link=city>cities</link> \
start with a considerable supply of it.  Both <link=area>areas</link> and \
<link=building>buildings</link> frequently require wood to be built.  Once \
your starting supply of wood runs out, you'll either need to <link=trade>trade\
</link> to get more, or <link=logging>harvest</link> some from the \
<link=city map tile>land</link> using a <link=sawmill>sawmill</link>.  \
<link=forest (terrain)>Forest</link> and <link=jungle (terrain)>jungle</link> \
contain the most wood, with <link=swamp (terrain)>swamps</link> not far \
behind, but even of map tiles will often have at least a few trees on them \
which can be cut down.\n\
Wood can also be used in a <link=woodworking shop>woodworking shop</link> to \
craft a variety of goods, including several <link=luxuries>luxuries</link>.\
");

  _resource(RES_STONE);
    _rname("stone");
    _rvalue(10);
    _rcolor(c_ltgray);
    _description("\
Stone is an important building material, and most new <link=city>cities</link> \
start with a considerable supply of it.  Both <link=area>areas</link> and \
<link=building>buildings</link> frequently require stone to be built.  Once \
your starting supply of stone runs out, you'll either need to <link=trade>trade\
</link> to get more, or <link=mining>mine</link> some from the \
<link=city map tile>land</link> using a <link=mine>mine</link>.  Fortunately \
stone is the most common <link=mineral>mineral</link>, and is available in \
essentially-infinite amounts in <link=hill (terrain)>hills</link> and \
<link=mountain (terrain)>mountains</link>.  After mining stone, you will have \
to cut it into usable blocks at a <link=masonry>masonry</link>.\
");

  _resource(RES_TIN);
    _rname("tin");
    _rvalue(15);
    _rcolor(c_ltcyan);
    _description("\
Tin is the most common and least valuable metal.  It can be <link=mining>mined\
</link> from the ground in the form of <link=tin ore>tin ore</link>, and then \
<link=smeltery>smelted</link> into this usable form.  Tin is too soft to be \
used for <link=weapon>weapons</link>, but can be made into various \
<link=luxury>luxuries</link> at a <link=smith>smith</link>.\
");

  _resource(RES_COPPER);
    _rname("copper");
    _rvalue(30);
    _rcolor(c_ltred);
    _description("\
Copper is a shiny red metal which is relatively common.  It can be \
<link=mining>mined</link> from the ground in the form of \
<link=copper ore>copper ore</link>, then <link=smeltery>smelted</link> into \
this usable form.  Copper can be used to make <link=weapon>weapons</link>, \
though as it is rather soft they will not be of a high quality.  It can also \
be fashioned into a variety of <link=luxury>luxuries</link> at a \
<link=smith>smith</link>.\
");

  _resource(RES_IRON);
    _rname("iron");
    _rvalue(50);
    _rcolor(c_cyan);
    _description("\
Iron is a hard, gray metal with a wide variety of uses.  It can be \
<link=mining>mined</link> from the gorund in the form of \
<link=iron ore>iron ore</link>, and then <link=smeltery>smelted</link> into \
this usable form.  Iron is the best metal for making <link=weapon>weapons\
</link>, and is also used for making heavy <link=armor>armor</link>.  Iron can \
also be made into a variety of <link=luxury>luxuries</link> at a \
<link=smith>smith</link>.\
");

  _resource(RES_FIBER);
    _rname("fiber");
    _rvalue(14);
    _rcolor(c_ltgray);
    _description("\
Fiber refers to any of a variety of materials suitable for making \
<link=clothing>clothing</link>.  Some <link=crops>crops</link> such as \
<link=hemp>hemp</link> provide a source of fiber, and some \
<link=livestock>livestock</link> <link=animal>animals</link>, such as \
<link=sheep>sheep</link>, provide a wool which is an excellent fiber.  Fiber \
can be sewn into clothing at a <link=tailor>tailor</link>, or used to make \
paper for <link=book>books</link> at a <link=scribery>scribery</link>.\
");

  _resource(RES_FUR);
    _rname("fur");
    _rvalue(16);
    _rcolor(c_brown);
    _description("\
Fur is the fine, long-haired pelt of any of a variety of <link=animal>animals\
</link>.  Fur cannot be harvested without killing the animal, either in a \
<link=hunting>hunt</link> or by slaughtering <link=livestock>livestock\
</link>.  Fur is used to make <link=fur coats>fur coats</link> at a \
<link=tailor>tailor</link>.\
");

  _resource(RES_LEATHER);
    _rname("leather");
    _rvalue(16);
    _rcolor(c_brown);
    _description("\
Leather is the tough, hairless pelt of any of a variety of <link=animal>animals\
</link>.  Leather cannot be harvested without killing the animal, either in a \
<link=hunting>hunt</link> or by slaughtering <link=livestock>livestock\
</link>.  Leather is used to make <link=leather coats>leather coats</link> at \
a <link=tailor>tailor</link>, and can also be used to make light \
<link=armor>leather armor</link>.\
");

  _resource(RES_UNICORN_HORN);
    _rname("unicorn horn");
    _rvalue(5000);
    _rcolor(c_pink);
    _description("\
Unicorn horns are extremely rare and valuable.  They can only be obtained by \
killing a <link=unicorn>unicorn</link>, one of the rarest <link=animal>beasts\
</link> in all the land.  As killing a unicorn is considered a sin by many \
<link=god>gods</link>, possessing a unicorn horn is both taboo and a great \
status symbol.  Unicorn horns are a vital component in a variety of \
<link=spell>spells</link>, particularly in the <link=magical school>schools\
</link> of <link=life magic>life</link> and <link=death magic>death</link>.\
");

  _resource(RES_SALT);
    _rname("salt");
    _rvalue(35);
    _rcolor(c_white);
    _luxury(LUX_NULL);  // Salt's technical a spice, but it's universally wanted
    _demand(150);
    _morale(8);
    _description("\
Salt is an extremely popular spice and an excellent preservative.  Its \
universal appeal means that it is demanded by every citizen, and does not \
compete with other spice <link=luxury>luxuries</link>.  It is the only luxury \
to have a standard demand of more than 1 unit per citizen.  Salt can be \
<link=mining>mined</link> from the ground in certain locations; in this case, \
it is brought to the surface in a coarse form, mixed with other minerals, and \
must be processed in a <link=kitchen>kitchen</link> prior to being eaten.\
");

  _resource(RES_PEPPER);
    _rname("pepper");
    _rvalue(28);
    _rcolor(c_dkgray);
    _luxury(LUX_SPICE);
    _demand(80);
    _morale(5);
    _description("\
<c=magenta>For the crop, see <link=pepper>pepper</link><c=magenta>.<c=/>\n\
\n\
Pepper is a small fruit, dried and crushed for use as a slightly-hot \
<link=luxury>spice</link>.  It is highly enjoyed by most.  As a spice, it \
competes for popularity with <link=cinnamon (resource)>cinnamon</link>, \
<link=cumin (resource)>cumin</link> and <link=paprika (resource)>paprika\
</link>.\
");

  _resource(RES_CINNAMON);
    _rname("cinnamon");
    _rvalue(26);
    _rcolor(c_brown);
    _luxury(LUX_SPICE);
    _demand(40);
    _morale(3);
    _description("\
<c=magenta>For the crop, see <link=cinnamon>cinnamon</link><c=magenta>.<c=/>\n\
\n\
Cinnamon is a <link=luxury>spice</link> obtained by drying the inner bark of \
a certain tree.  It has a pungent flavor which enjoys moderate popularity.  As \
a spice, it competes for popularity with <link=pepper (resource)>pepper\
</link>, <link=cumin (resource)>cumin</link> and \
<link=paprika (resource)>paprika</link>.\
");

  _resource(RES_CUMIN);
    _rname("cumin");
    _rvalue(45);
    _rcolor(c_brown);
    _luxury(LUX_SPICE);
    _demand(35);
    _morale(5);
    _description("\
<c=magenta>For the crop, see <link=cumin>cumin</link><c=magenta>.<c=/>\n\
\n\
Cumin is a <link=luxury>spice</link> made by drying and crushing the seeds of \
a low-growing plant.  It has a rich, smoky flavor which is greatly enjoyed by \
a relatively small portion of the population. As a spice, it competes for \
popularity with <link=pepper (resource)>pepper</link>, \
<link=cinnamon (resource)>cinnamon</link> and <link=paprika (resource)>paprika\
</link>.\
");

  _resource(RES_PAPRIKA);
    _rname("paprika");
    _rvalue(20);
    _rcolor(c_ltred);
    _luxury(LUX_SPICE);
    _demand(60);
    _morale(3);
    _description("\
<c=magenta>For the crop, see <link=paprika>paprika</link><c=magenta>.<c=/>\n\
\n\
Paprika is a <link=luxury>spice</link> which is made by drying a red pepper \
and grinding it into a powder.  It has a mildly spicy, earthy, slightly-sweet \
flavor which is enjoyed by much of the population.  As a spice, it competes \
for popularity with <link=pepper (resource)>pepper</link>, \
<link=cinnamon (resource)>cinnamon</link> and <link=cumin (resource)>cumin\
</link>.\
");

  _resource(RES_TOBACCO);
    _rname("tobacco");
    _rvalue(30);
    _rcolor(c_brown);
    _luxury(LUX_SMOKABLE);
    _demand(75);
    _morale(5);

  _resource(RES_CANNABIS);
    _rname("cannabis");
    _rvalue(38);
    _rcolor(c_green);
    _luxury(LUX_SMOKABLE);
    _demand(40);
    _morale(5);

  _resource(RES_AMANITAS);
    _rname("amanitas");
    _rvalue(75);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(5);
    _morale(3);

  _resource(RES_AYAHUASCA);
    _rname("ayahuasca");
    _rvalue(80);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(4);
    _morale(3);

  _resource(RES_SPICEREED);
    _rname("spicereed");
    _rvalue(45);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(30);
    _morale(4);

  _resource(RES_CLOTHING);
    _rname("clothing");
    _rvalue(35);
    _rcolor(c_white);
    _demand(20);
    _morale(2);

  _resource(RES_FUR_COATS);
    _rname("fur coats");
    _rvalue(60);
    _rcolor(c_brown);
    _luxury(LUX_COATS);
    _demand(10);
    _morale(3);

  _resource(RES_LEATHER_COATS);
    _rname("leather coats");
    _rvalue(65);
    _rcolor(c_brown);
    _luxury(LUX_COATS);
    _demand(8);
    _morale(3);

  _resource(RES_WOOD_HOUSEWARES);
    _rname("wood housewares");
    _rvalue(70);
    _rcolor(c_ltred);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(4);

  _resource(RES_TIN_HOUSEWARES);
    _rname("tin housewares");
    _rvalue(85);
    _rcolor(c_ltcyan);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(5);

  _resource(RES_COPPER_HOUSEWARES);
    _rname("copper housewares");
    _rvalue(90);
    _rcolor(c_ltred);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(6);

  _resource(RES_IRON_HOUSEWARES);
    _rname("iron housewares");
    _rvalue(110);
    _rcolor(c_cyan);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(7);

  _resource(RES_FURNITURE);
    _rname("furniture");
    _rvalue(130);
    _rcolor(c_ltred);
    _demand(8);
    _morale(8);

  _resource(RES_JEWELRY);
    _rname("jewelry");
    _rvalue(200);
    _rcolor(c_magenta);
    _demand(5);
    _morale(12);

  _resource(RES_BLANK_BOOK);
    _rname("blank book");
    _rvalue(50);
    _rcolor(c_white);

  _resource(RES_FARMING);
    _rname("farming");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_MINING);
    _rname("mining");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_HUNTING);
    _rname("hunting");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_LOGGING);
    _rname("logging");
    _rcolor(c_magenta);
    _meta();

// _food() is per 100 units of the crop!
  _crop(CROP_NULL);
    _cname("any crop");

  _crop(CROP_WHEAT);
    _cname("wheat");
    _cpercent(90);
    _temperature(25, 90);
    _altitude(0, 90);
    _rainfall(10, 70);
    _type(CROPTYPE_FOOD);
    _food(100);

  _crop(CROP_CABBAGE);
    _cname("cabbage");
    _cpercent(70);
    _temperature(10, 75);
    _altitude(0, 80);
    _rainfall(5, 100);
    _type(CROPTYPE_FOOD);
    _food(120);

  _crop(CROP_GRAPES);
    _cname("grapes");
    _cpercent(30);
    _temperature(30, 70);
    _altitude(15, 90);
    _rainfall(15, 60);
    _type(CROPTYPE_FOOD);
    _food(50);

  _crop(CROP_MELON);
    _cname("melon");
    _cpercent(40);
    _temperature(35, 90);
    _altitude(0, 60);
    _rainfall(20, 100);
    _type(CROPTYPE_FOOD);
    _food(80);

  _crop(CROP_RICE);
    _cname("rice");
    _cpercent(90);
    _temperature(50, 100);
    _altitude(0, 45);
    _rainfall(45, 100);
    _type(CROPTYPE_FOOD);
    _food(110);

  _crop(CROP_CACTUS);
    _cname("cactus");
    _cpercent(90);
    _temperature(60, 100);
    _altitude(0, 80);
    _rainfall(0, 15);
    _type(CROPTYPE_FOOD);
    _food(60);
    _bonus(RES_FIBER, 20);

  _crop(CROP_PEPPER);
    _cname("pepper");
    _cpercent(40);
    _temperature(60, 90);
    _altitude(0, 60);
    _rainfall(10, 60);
    _type(CROPTYPE_SPICE);
    _food(15);
    _bonus(RES_PEPPER, 90);

  _crop(CROP_CINNAMON);
    _cname("cinnamon");
    _cpercent(20);
    _temperature(65, 90);
    _altitude(15, 80);
    _rainfall(10, 60);
    _type(CROPTYPE_SPICE);
    _bonus(RES_CINNAMON, 100);

  _crop(CROP_CUMIN);
    _cname("cumin");
    _cpercent(8);
    _temperature(70, 100);
    _altitude(10, 65);
    _rainfall(40, 100);
    _type(CROPTYPE_SPICE);
    _bonus(RES_CUMIN, 80);

  _crop(CROP_PAPRIKA);
    _cname("paprika");
    _cpercent(45);
    _temperature(40, 70);
    _altitude(0, 50);
    _rainfall(20, 60);
    _type(CROPTYPE_SPICE);
    _bonus(RES_PAPRIKA, 100);

  _crop(CROP_TOBACCO);
    _cname("tobacco");
    _cpercent(30);
    _temperature(50, 85);
    _altitude(0, 50);
    _rainfall(20, 50);
    _type(CROPTYPE_DRUG);
    _bonus(RES_TOBACCO, 100);

  _crop(CROP_AMANITAS);
    _cname("amanitas");
    _cpercent(10);
    _temperature(0, 40);
    _altitude(0, 70);
    _rainfall(20, 100);
    _type(CROPTYPE_DRUG);
    _bonus(RES_AMANITAS, 100);

  _crop(CROP_AYAHUASCA);
    _cname("ayahuasca");
    _cpercent(10);
    _temperature(65, 100);
    _altitude(20, 70);
    _rainfall(60, 100);
    _type(CROPTYPE_DRUG);
    _bonus(RES_AYAHUASCA, 100);

  _crop(CROP_SPICEREED);
    _cname("spicereed");
    _cpercent(8);
    _temperature(70, 100);
    _altitude(0, 30);
    _rainfall(0, 15);
    _type(CROPTYPE_DRUG);
    _bonus(RES_SPICEREED, 100);

  _crop(CROP_DEATHCAP);
    _cname("deathcap");
    _cpercent(7);
    _temperature(20, 70);
    _altitude(0, 70);
    _rainfall(45, 100);
    _type(CROPTYPE_POISON);

  _crop(CROP_VIPERVINE);
    _cname("vipervine");
    _cpercent(10);
    _temperature(65, 100);
    _altitude(20, 70);
    _rainfall(40, 100);
    _type(CROPTYPE_POISON);

  _crop(CROP_SCORPICON);
    _cname("scorpicon");
    _cpercent(7);
    _temperature(60, 100);
    _altitude(0, 70);
    _rainfall(0, 10);
    _type(CROPTYPE_POISON);

  _crop(CROP_COTTON);
    _cname("cotton");
    _cpercent(70);
    _temperature(60, 90);
    _altitude(0, 80);
    _rainfall(10, 50);
    _type(CROPTYPE_FIBER);
    _bonus(RES_FIBER, 100);

  _crop(CROP_HEMP);
    _cname("hemp");
    _cpercent(60);
    _temperature(25, 90);
    _altitude(0, 100);
    _rainfall(5, 100);
    _type(CROPTYPE_FIBER);
    _bonus(RES_FIBER, 80);
    _bonus(RES_CANNABIS, 20);

// Minerals

  _mineral(MINERAL_NULL);
    _mname("any mineral");
    _mcolor(c_dkgray);

  _mineral(MINERAL_STONE);
    _mname("stone");
    _mpercent(98);
    _mvalue(5);
    _mcolor(c_white);

  _mineral(MINERAL_TIN);
    _mname("tin ore");
    _mpercent(70);
    _mvalue(5);
    _mcolor(c_ltcyan);
    _mhidden();

  _mineral(MINERAL_COPPER);
    _mname("copper ore");
    _mpercent(50);
    _mvalue(10);
    _mcolor(c_ltred);
    _mhidden();

  _mineral(MINERAL_IRON);
    _mname("iron ore");
    _mpercent(70);
    _mvalue(30);
    _mcolor(c_cyan);
    _mhidden();

  _mineral(MINERAL_SALT);
    _mname("salt");
    _mpercent(50);
    _mvalue(28);
    _mcolor(c_white);
    _mhidden();

  _mineral(MINERAL_GEMS);
    _mname("gems");
    _mpercent(3);
    _mvalue(100);
    _mcolor(c_pink);
    _mhidden();

// The value of gold is less than 100 because it takes labor and fuel to
// convert it into spendable currency!
  _mineral(MINERAL_GOLD);
    _mname("gold");
    _mpercent(4);
    _mvalue(80);
    _mcolor(c_yellow);
    _mhidden();

  _mineral(MINERAL_COAL);
    _mname("coal");
    _mpercent(20);
    _mvalue(50);
    _mcolor(c_dkgray);
    _mhidden();
}
