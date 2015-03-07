#include "help.h"
#include "globals.h"

#define _article(n) \
  cur_article = new Help_article( (n) ); \
  HELP->add_article(cur_article)

#define _type(n) \
  cur_article->type = (n)

#define _text(n) \
  cur_article->set_text( (n) )

void init_help()
{
  Help_article* cur_article;

// Start with articles that AREN'T based on game data; these are all manual.

  _article("City");
  _type("Game Concept");
  _text("\
A city is an area where people live and work together.  A large part of \
Edigotia lies in managing your city and helping it to grow.\n\
\n\
The <link=world map>world</link> is dotted with cities, each of which belongs \
to a certain <link=kingdom>kingdom</link> and <link=race>race</link>.  These \
cities are often connected by <link=road>roads</link> to facilitate trade.  \
Inside the cities there is a number of <link=citizens>citizens</link>, \
<link=building>buildings</link>, and a <link=city map>map</link> which has \
<link=area>areas</link> built on it.  Cities may also house <link=nobles>nobles\
</link> and <link=military>armies</link>.\
");

  _article("Kingdom");
  _type("Game Concept");
  _text("\
Kingdoms are loosely affiliated sets of <link=city>cities</link>.  Usually, \
kingdoms are tied to <link=race>races</link>; each race has one kingdom \
associated with it, and a kingdom is made up entirely of that race.  This \
makes the concept of \"race\" and \"kingdom\" closely related, and you may see \
them used somewhat interchangably.  However, while \"race\" refers to the \
particular attributes of <link=nobles>nobles</link> and <link=citizens>citizens\
</link>, \"kingdom\" refers to a political structure.\n\
\n\
Kingdoms are based on a political hierarchy.  Each kingdom has one capital \
city, the home of the <link=king>king or queen</link> of the kingdom.  The \
rest of the kingdom is split into several <link=duchy>duchies</link>, each of \
which has its own capital city, which is the home of the <c=ltgreen>duke or \
duchess<c=/>.  Within each duchy are several cities, run by <c=ltgreen>mayors\
<c=/>.\n\
\n\
Each mayor is expected to obey the rule of its duke, and each duke is \
duty-bound to the king.  The most concrete form of this fealty is seen in \
taxes; all cities owe a certain monthly sum to their duke, while each duchy \
must pay monthly taxes to the king.  The end result is that the kingdom's \
capital city enjoys great wealth.\n\
\n\
In addition to taxes, cities may be asked to come to the aid of their lord, \
and of their duchy in general.  This may involve sending <link=food>food\
</link> to cities in need, <link=conscription>conscripting</link> citizens for \
service in the royal <link=army>army</link>, or changing their laws to match \
the demands of the lord.  If a city performs well in the eyes of their lord, \
they will gain royal <link=relations>favor</link>, improving their political \
standing.  Favors are a two-way street; if your city is in need, whether it be \
a food shortage, impending military invasion, or other issue, asking your lord \
for help is often the best solution.  However, asking for help too often will \
damage your favor, and your lord may eventually cut you off altogether.\n\
\n\
While you start the game in control of a normal city, this is not necessarily \
your eternal fate.  There are ways to climb the political ladder, advancing to \
the status of a duke and even king or queen.  This simplest - and most \
dangerous - is a military invasion, taking the duchy seat as your own.  This \
will almost always provoke a response of force from the king, unless you have \
their support.  A safer option would be political maneuvering.  If you can \
gain great favor with the king, while simultaneously turning the king against \
your duke, they may instate you as a new duke.  Alternatively, if the duke \
dies and leaves no heir, they may select a very close friend to inherit their \
dynasty.\n\
\n\
Similarly to how cities have often-complex relationships, different kingdoms \
have their own relationships with each other.  Kingdoms that share a border \
may develop tensions, and issues of trade and war can further strain them.  If \
two kingdoms end up going to war, the individual cities inside them will be \
expected to cut off all trade with each other, and may be asked to contribute \
soldiers to the war effort.\
");

  _article("Race");
  _type("Game Concept");
  _text("\
<c=magenta>For a list of races, see <link=list of races>list of races.</link>\n\
\n\
The world of Edigotia is populated by various races.  Members of each race \
will band together to form a <link=kingdom>kingdom</link>, forming the world's \
political structures.  Each <link=city>city</link> in the world belongs to a \
single race, though it may have visitors or <link=slaves>slaves</link> of \
other races.  The kingdoms of various races are built differently; some races \
prefer a large number of low-population cities, while others gather in \
relatively few metropolises.\n\
\n\
Races can have wildly different attributes which greatly impact the way the \
city they live in runs.  The most notable attribute is their \
<link=skills>skills</link>.  These are the race's aptitudes with various tasks \
that befall its <link=citizens>citizens</link>.  These determine the output of \
various <link=area>areas</link> and <link=building>buildings</link>, along \
with their abilities in <link=trade>trade</link> and <link=war>warfare\
</link>.  For a list of skills, see <link=list of skills>list of skills\
</link>.\n\
\n\
Races have other attributes as well.  For instance, their ability to travel \
across or fight in various terrains may vary.  They also have different \
<link=combat>combat</link> attributes, like base skill, <link=hit points>hit \
points</link>, and skills with various <link=weapon>weapons</link>.\n\
Some races consume <link=food>food</link> much faster than others, making food \
a more pressing concern.  The citizens and <link=resources>resources</link> \
each race starts with is different.  Birth rate and citizen class ratios may \
differ as well, as well as the citizens' <link=morale>morale</link> demands \
and acceptable <link=taxes>tax rates</link>.\
");

  _article("Area");
  _type("Game Concept");
  _text("\
Areas are city improvements which are placed on the <link=city map>city \
map</link>.  They function similarly to <link=building>buildings</link>; they \
can employ <link=citizens>citizens</link> and may produce <link=resources>\
resources</link>.  Since the number of areas you can place is limited by your \
<link=city radius>city radius</link>, they tend to be more useful or important \
than buildings.\n\
\n\
Your city always starts with one area - the <link=keep>keep</link>.  The keep \
appears at the center of your city, and provides you with the basic support to \
start growing.  While you can close or destroy the keep, it is not \
recommended!\n\
\n\
Many areas interact with the city map to reap the resources it contains.  It \
is important to carefully consider the placement of your areas in order to \
maximize their effectiveness.\n\
\n\
Areas can be built by selecting the \"Build Area\" option from the \
<link=main screen>main screen</link>.  They are organized into several \
categories.  Areas cost a certain amount of resources to build (usually \
<link=gold>gold</link>, <link=stone>stone</link>, and/or <link=wood>wood\
</link>), and will usually require several days of construction before they \
can be used.\n\
\n\
Once open, some areas will automatically employ citizens, while others must be \
interacted with via the <link=building status screen>building status screen\
</link> in order to hire citizens.  Others do not employ workers at all.  Open \
areas will often charge a maintenance fee every day.  The only way to stop \
paying this fee is to <link=closing areas and buildings>close</link> the \
area, or <link=destroying areas>destroying</link> it altogether.  Closing an \
area is free, but it will usually cost some gold to re-open it.  Destroying an \
area costs money as well, and is irreversible.\n\
\n\
Many areas produce resources, such as <link=farm>farms</link> and \
<link=mine>mines</link>.  Others provide <link=housing>housing</link> for your \
citizens, and still more are supportive infrastructure; for instance, \
<link=pasture>pastures</link> provide space to hold livestock, while \
<link=park>parks</link> provide a space for your citizens to relax, improving \
their morale.  One final use for areas is to provide space in which to place \
buildings, like <link=plaza>plazas</link> and <link=marketplace>marketplaces\
</link> do.\n\
\n\
Not all areas are available from the start of the game.  You may need to meet \
certain <link=unlockables>unlock conditions</link> in order to build a certain \
area.\
");

  _article("Building");
  _type("Game Concept");
  _text("\
Buildings are a core part of your <link=city>city</link>.  Adding buildings to \
your city allow you to create new resources, improve various areas of city \
administration, and train special <link=citizens>citizens</link>.\n\
\n\
In order to add buildings to your city, you will need one or more \
<link=area>areas</link> which support buildings.  These areas are essentially \
ground upon which buildings may be placed.  Fortunately, your city always \
starts with a <link=keep>keep</link>, which supports up to 6 buildings.  The \
most common way to support more buildings is by adding a \
<link=plaza>plaza</link> or <link=marketplace>marketplace</link>, both of \
which support buildings.\n\
\n\
Buildings fill a variety of purposes.  The most common is to employ citizens \
to produce resources.  This production is either a raw output which each \
worker creates, or a form of manufacturing which converts one or more \
resources into something new.  Employed citizens almost always need to be paid \
wages; most buildings also have a daily maintenance cost which must be paid.  \
You can stop paying these fees by <link=closing areas and buildings>closing\
</link> the building down, which will fire all citizens and halt any \
production.  This is free, but re-opening the building may cost some gold.  \
You can also destroy the building altogether; this is irreversible.\n\
\n\
You can build a building via the Buildings drop-down menu.  This will usually \
require an expenditure of resources, most commonly <link=gold>gold</link>, \
<link=stone>stone</link> and/or <link=wood>wood</link>.  Buildings usually \
take several days to finish construction; a <link=messages>message</link> will \
alert you when it's finished.  You can see the building's progress in the \
<link=building status screen>building status screen</link>.  The building will \
also appear in this screen after it's finished.  You can use this screen to \
hire or fire citizens, change the production queue of a building, or close and \
reopen buildings.\n\
\n\
Not all buildings are available from the start of the game.  You may need to \
meet certain <link=unlockables>unlock conditions</link> in order to build a \
certain building.\
");

  _article("City Map");
  _type("Game Concept");
  _text("\
The city map is a 9 x 9 set of <link=city map tile>terrain tiles</link> which \
represents the land your <link=city>city</link> is built on.  The terrain \
found in your city map is largely dependent on what <link=world map>world map \
tile</link> you choose to place your city on.\n\
\n\
Your main interaction with the city map will be to build <link=area>areas\
</link> on it.  Areas are buildings or other improvements made to the \
terrain.  Many of them directly exploit the terrain, such as <link=farm>farms\
</link>, <link=mine>mines</link> or <link=sawmill>sawmills</link>, so it is \
important to take care when choosing a location for your area.  By moving your \
cursor over the map in the <link=main screen>main screen</link>, you can view \
information on each tile.  If you select an area to build, this information \
will change to be more relevant to that area.\n\
\n\
You may notice that only a small portion of the map is displayed in color, \
while the rest is grayed out.  This is due to your <link=city radius>city \
radius</link>, an area of control outside of which you may not build areas.  \
You can turn off the grayed-out coloring in the main screen; you can also \
toggle the display of areas versus only showing terrain.\
");

  _article("Citizens");
  _type("Game Concept");
  _text("\
The people who live in your <link=city>city</link> are known as your \
citizens.  They are divided into three main classes: <link=peasants>peasants\
</link>, <link=merchants>merchants</link> and <link=burghers>burghers</link>.  \
Each class provides different services and fills different jobs.  Maintaining \
a mixture of all three is critical for a city to grow successfully.  Peasants \
provide physical labor, working on <link=farm>farms</link> and <link=mine>mines\
</link>, merchants provide skilled labor and help manufacture base resources \
into more valuable ones, and burghers are artisans and service workers, \
making a city stand out amongst its peers.\n\
\n\
The primary purpose of citizens is to fill jobs in <link=area>areas</link> and \
<link=building>buildings</link>.  These jobs are all tailored to a specific \
class, so some areas or buildings may be useless if you don't have available \
citizens of that class.  When citizens aren't working a job, they aren't \
idle!  They are working from their homes as seamstresses, tinkers, or other \
small odd jobs.  This produces a small amount of income which may be \
<link=taxes>taxed</link>.  Merchants generate more income than peasants, and \
burghers produce more than merchants.  Taxes are an important source of \
revenue for most cities, so it's important to balanced how many citizens you \
employ.\n\
\n\
Taxation also forms the basis of your citizens' <link=morale>morale</link>.  \
Morale is the overall happiness of your citizens, and is affected by a wide \
range of sources.  Middling morale will usually have no effects, but when it's \
very or very low the effects can be dramatic.  The most common ways to improve \
morale is by reducing taxation or giving your citizens <link=luxuries>luxuries\
</link>.\n\
\n\
Before a citizen can work a job or produce income, they have to have a home.  \
Each class lives in a different kind of housing, and the housing for higher \
classes is more expensive than that for lower classes.  Housing may only be \
built as an <link=area>area</link>, so you will need to plan carefully to \
anticipate increase population!  Any homeless citizens will be unable to hold \
a job or produce income, but will continue to consume food and resources.\n\
\n\
The most important resource your citizens need is food.  The amount of food \
each citizen consumes depends on your race.  Addtionally, higher-class \
citizens eat more than lower-class ones.  If you cannot feed your citizens, \
they will begin to starve and, if the food deficit continues for long enough, \
may die.\n\
\n\
If your citizens are well-fed, they will eventually start giving birth.  A \
citizen will be born into the highest class available.  Citizen classes have \
to maintain a certain ratio.  The exact numbers depend on your race; for \
humans, there must be 10 peasants for every merchant, and 10 merchants for \
every burgher.  Addtionally, a class's morale must be above a certain level \
for the class above it to be born.\
");

  _article("Plaza");
  _type("Area");
  _text("Just a demo");

  _article("Humans");
  _type("Race");
  _text("Just a demo");

  HELP->process_categories();
}
