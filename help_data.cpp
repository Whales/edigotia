#include "help.h"
#include "globals.h"
#include "area.h"
#include "building.h"
#include "stringfunc.h" // For capitalize()

#define _article(n) \
  cur_article = new Help_article( (n) ); \
  HELP->add_article(cur_article)

#define _type(n) \
  cur_article->type = (n)

#define _text(n) \
  cur_article->set_text( (n) )

#define _redirect(n) \
  cur_article->redirect = (n)

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
city, the home of the <link=monarch>king or queen</link> of the kingdom.  The \
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

  _article("Queen");
  _redirect("Monarch");

  _article("King");
  _redirect("Monarch");

  _article("Monarch");
  _type("Game Concept");
  _text("\
The monarch (king or queen) is a <link=nobles>noble</link> who rules over a \
<link=kingdom>kingdom</link>.  This honor is generally passed on from \
generation to generation; when the current ruler passes away, their eldest \
heir is named king or queen after them.  If there is no eligable heir, there \
is a <link=succession crisis>succession crisis</link>, and the kingdom will be \
without a leader until one is selected (by any surviving royal family) or \
someone <link=coup>seizes the throne</link>.\n\
\n\
The <link=city>city</link> that the monarch rules over is the de facto \
capital of the kingdom.  This city usually enjoys a great surplus of wealth, \
as all <link=duchy>duchies</link> under its rule must pay taxes to the city.  \
This frees the city up to spend its time and <link=resources>resources</link> \
on matters of state, rather than securing <link=trade>trades</link> for \
wealth.  The monarch is also expected to tend to the needs of its duchies, who \
in turn will come to the aid of their cities, so much of this wealth will wind \
up back in the hands of the kingdom's cities.\n\
\n\
The monarch enjoys the power to declare the <link=laws>laws</link> of the \
kingdom.  They can declare <link=war>war</link> against another kingdom, \
<link=trade embargo>trade embargoes</link> against a kingdom or city, \
<link=prohibition>prohibit</link> the produce, sale or use of various \
resources, or various other laws.  Cities may choose to disobey the law of the \
monarch, but will lose <link=favor>favor</link> or may see their ruler removed \
from power.\n\
\n\
As the ruler of the kingdom, the monarch is often under threat of \
assassination.  Foreign powers may benefit from the chaos caused when a \
monarch dies, and even the subjects of the monarch may want to depose their \
ruler, either to disrupt unfair rule or for a chance to seize the throne.  For \
this reason, most monarch will want to keep a sizable <link=army>army</link> \
to defend their city, and a strong <link=city guard>city guard</link> to \
thwart assassins.\
");

  _article("Duke");
  _redirect("Duchy");

  _article("Duchess");
  _redirect("Duchy");

  _article("Duchy");
  _type("Game Concept");
  _text("\
A duchy is a district of a <link=kingdom>kingdom</link> under the rule of a \
<c=ltgreen>duke or duchess<c=/>.  A duchy consists of several <link=city>cities\
</link>, one of which is home to and ruled by the duke, known as the \
<c=ltgreen>duchy seat<c=/>.  The duchy seat collects monthly taxes from its \
subject cities, allowing it to focus on matters of state rather than securing \
a strong economy.  However, cities in need will often turn to their duke for \
aid, so this tax wealth is often redistributed to the duchy's cities.\n\
\n\
The duke is allowed to write the <link=laws>laws</link> for its subject \
cities.  These can range from general civil or religious laws, including \
<link=prohibition>prohibition</link> of various <link=resources>resources\
</link>, to <link=trade embargo>trade bans</link> or even <link=war>war</link> \
against other cities.  However, the duchy is in turn subject to the laws of \
the <link=monarch>king or queen</link>.\n\
\n\
When a duke or duchess passes away, rule generally passes to their eldest \
heir.  If there is no eligable heir, it falls to the monarch to select a new \
ruler of the duchy.  In most cases, the monarch will pick whatever noble in \
the region has the greatest <link=favor>favor</link> with them.  Dukes may be \
deposed by the monarch if they are disobedient and replaced with the monarch's \
choice of ruler.  Finally, dukes are in danger of assassination by nobles who \
aspire to seize control of the duchy.\
");

  _article("Nobles");
  _type("Game Concept");
  _text("\
Nobles are members of the ruling class in Edigotia.  At the start of a new \
game, you control a single noble of your design, who acts as the ruler of your \
<link=city>city</link>.  This noble <c=white>is<c=/> you!  If they die, and \
you have no other nobles under your control, your game is over.  For this \
reason, keeping your nobles safe, and obtaining more, is very important.\n\
\n\
Nobles fill a wide range of purposes (<c=magenta>for a full list, see \
<link=list of noble roles>list of noble roles</link>).  The most important, \
and the first role your noble will take, is the <link=governor>governor</link> \
of your city.  As you gain more nobles, you can assign them other roles via \
the <link=nobles screen>noble control screen</link>.\n\
\n\
Each noble is unique.  In addition to more-or-less cosmetic details like their \
name, sex, and <link=coat of arms>coat of arms</link>, they have two \
<link=noble traits>traits</link> and a variety of <link=noble skills>skills\
</link>.  The traits are innate to your noble and cannot be changed.  They \
provide some bonuses (or in some cases, penalties) to various areas of the \
game, and may alter how your city grows.  The skills may be increased through \
training, and represent how good your noble is at the various jobs they can \
take on.\n\
\n\
Your noble is an important dignitary.  In addition to overseeing \
<link=diplomacy>diplomatic</link> and <link=trade>trade</link> agreements, \
they can attend various social functions at home or in foreign cities.  These \
events are a good opportunity for your noble to meet a future husband or \
wife.  Marriages can be founded in romance, if both parties are lucky enough \
to be attracted to each other, but more often they are the product of a \
political agreement to bring two cities closer together.\n\
\n\
After your noble has wed a spouse, you can start trying to sire some heirs.  \
Heirs are a crucial mid-game goal, as they allow you to continue your \
<link=dynasty>lineage</link> and continue playing the game after the first \
generation of nobles has passed away.  The children of two nobles will inherit \
a random trait from each one.  There is a small random chance for them to \
instead receive a completely random trait, introducing some change into the \
family.  If the parents are related (second cousins or closer), then there is \
a chance that the child will receive a random <c=ltred>bad<c=/> trait!\
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

  _article("World Map");
  _type("Game Concept");
  _text("\
<c=magenta>For a list of <link=map type>map types</link><c=magenta> and world \
map terrain, see <link=list of map types>list of map types.</link>\n\
\n\
The world map is a flat map of the known world.  The world is flat, and \
exploring beyond the edge of the map will lead to nothing but open seas and \
eventually the edge of the world!\n\
\n\
The world is composed of several continents, separated by ocean.  The world's \
<link=temperature>temperature</link> is colder towards the north and south \
ends, and warmest at the equator.  The continents are covered with a variety \
of <link=mountainous>mountain ranges</link> and long flat areas, and are more \
likely to be flat near the coats.  There are complex patterns of rainfall.  \
Prevailing winds go from west to east.  As the wind travels over ocean, it \
builds up moisture, resulting in higher <link=rainfall>rainfall</link> near \
west coasts.  If the wind hits a mountain range, it will result in high \
rainfall to the west of the mountains, and a much drier climate to the east.  \
For this reason, you will often see areas of <link=desert>desert</link> to the \
east of mountains.\n\
\n\
Each tile of the world map has a different <link=map type>terrain</link>.  \
This terrain determines what the <link=city map>map</link> of a <link=city>city\
</link> built on that tile will look like.  Terrain is determined by altitude, \
temperature and rainfall.  Areas with low altitude and moderate temperature \
and rainfall, like <link=plains>plains</link>, are generally the easiest to \
survive in.  However, all terrains are survivable and offer different types of \
<link=resources>resources</link>, <link=crops>crops</link> and \
<link=animals>wildlife</link> to <link=hunting>hunt</link>.  A city can thrive \
anywhere by exploiting the unique benefits of the terrain!\n\
\n\
Large areas of the world are claimed by various <link=kingdom>kingdoms\
</link>.  Each kingdom is dotted with cities, connected by <link=road>roads\
</link>.  Roads are just one (albeit the most important) of many \
<link=world map improvements>improvements</link> that can be built on the \
world map.  When you start a new game, the part of the world claimed by the \
kingdom you belong to is revealed, along with a small area outside of it.  \
Additionally, any coastlines somewhat close to your kingdom are revealed.  You \
can gain knowledge of more of the world map by exploring with an \
<link=adventurer>adventurer</link> or <link=army>army</link>, or by \
<link=trade>trading</link> maps with another kingdom (though most kingdoms are \
very hesitant to reveal what they know of the world).\n\
\n\
Traveling across the world is fastest via roads.  Traveling off-road is slower \
and may even be dangerous!  For instance, <link=jungle>jungles</link> have \
many hazards to overcome, and the heat and lack of water in deserts will wear \
down an army.  Various <link=race>races</link> fare different in different \
terrains; for instance, <link=elves>elves</link> fight and travel much more \
easily in <link=forest>forests</link> than most races.\n\
\n\
The world is dotted with <link=special location>special locations</link>.  \
These are ruins, monster nests, and other places of interest.  Many of these \
areas can be visited and explored by armies, while others can only be fully \
explored by an adventurer.  Some may require the power of a <link=mage>mage\
</link> to fully exploit!  Be careful, because these special locations often \
contain traps, monsters, and other hazards which may kill those who dare to \
enter them.  But if these challenges can be overcome, there is almost always a \
valuable treasure or magical effect to win!\
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
Each class lives in a different kind of <link=housing>housing</link>, and the \
housing for higher classes is more expensive than that for lower classes.  \
Housing may only be built as an <link=area>area</link>, so you will need to \
plan carefully to anticipate increase population!  Any homeless citizens will \
be unable to hold a job or produce income, but will continue to consume food \
and resources.\n\
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

  _article("World Map Improvements");
  _type("Game Concept");
  _text("\
<c=magenta>See the bottom of this article for a list of improvements.<c=/>\n\
\n\
The <link=world map>world map</link> is mostly static, but it can be changed \
by industrious <link=race>races</link> by placing world map improvements.  \
These are various buildings which make the world a more inhabitable place and \
safer for travel.\n\
\n\
Improvements can be built on the world map by pulling up the world map from \
the <link=main screen>main screen</link> and pressing the button for \
<c=magenta>\"Build Improvement\"<c=/>.  Building an improvement will require \
you to send multiple <link=citizens>citizens</link> outside of your city, \
usually <link=peasants>peasants</link>.  Be careful, because there may be \
<link=world hazards</link>hazards</link> that pose mortal danger to these \
workers!  You may want to send a small <link=army>army</link> with them for \
safety.\n\
\n\
Building a world improvement will also cost some <link=resources>resources\
</link>, including <link=gold>gold</link> to pay the workers and provide them \
with basic supplies for their journey.  The further from your <link=city>city\
</link> the improvement is placed, the more gold you'll need to spend.  Many \
improvements will help secure your city's safety or improve your \
<link=trade>trade</link> options, so over time they will pay for themselves!\n\
\n\
At the start of the game, most cities within any given <link=kingdom>kingdom\
</link> will be connected by roads, though some small or isolated villages may \
be left unconnected.  Often neighboring kingdoms will be connected by road as \
well, facilitating trade.  Sometimes a kingdom's capital city will have some \
watch towers around it.  Particularly industrious kingdoms may wall off some \
of their borders with walls.\n\
\n\
<c=magenta>List of world improvements:</link>\n\
<c=white>*<c=/> <link=road>Road</link> - Makes travel faster and decreases \
trade <link=overhead>overhead</link>.\n\
<c=white>*<c=/> <link=watch tower>Watch Tower</link> - Must be staffed by at \
least one citizen.  Reveals any dangers or armies close to the tower, and can \
signal your city to impending danger more or less instantly.\n\
<c=white>*<c=/> <link=wall>Wall</link> - Makes it very difficult for enemy \
armies to pass.  While they are trying to pass the wall, they are very \
susceptible to attack, particularly by <link=ranged combat>ranged attacks\
</link> like <link=archer>archers</link> or <link=mage>mages</link>, so you \
may want to station such armies on the wall.  Walls take a lot of time, gold, \
and <link=stone>stone</link> to build, so you may want to carefully select \
their placement; for instance, <link=mountainous>mountains</link> are already \
hard to pass, so you can place walls in the gaps between mountains.\
");

// Add all Area_datums to the help database!
  for (int i = 0; i < AREA_MAX; i++) {
    Area_datum* area_dat = Area_data[i];
    _article( capitalize( area_dat->name ) );
    _type("Area");
    _text( area_dat->generate_help_text() );
  }

// Add all non-area Building_datums to the help database!
  for (int i = 0; i < BUILD_MAX; i++) {
    Building_datum* build_dat = Building_data[i];
    if (build_dat->category != BUILDCAT_NULL) { // i.e. it's not an area
      _article( capitalize( build_dat->name ) );
      _type("Building");
      _text( build_dat->generate_help_text() );
    }
  }

  HELP->process_categories();
}
