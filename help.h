#ifndef _HELP_H_
#define _HELP_H_

#include <string>
#include <vector>
#include <map>

/* Data structures and functions for in-game help.
 *
 * Help pages have a title, a type, and a body of text.
 *
 * The title is what the page refers to, e.g. "Masonry".  Searching for titles
 * is not case sensitive.
 *
 * The type is the category of the page.  Since it's a string you can use
 * whatever you want, but it's best to restrict it to an existing type:
 * Building, Area, Terrain, Map Type, Resource, Crop, Mineral, Animal, Race
 *
 * The body of text is the content of the article.  It uses normal color
 * formatting tags (i.e. "<c=COLOR> ... <c=/>").  You can add links to your page
 * by using the "<link=TARGET> ... </link>" tag.  For example, if you put
 * "<link=area>areas</link>" in your page, then the word "areas" will show up in
 * light blue and will allow the player to jump to the help page on "area".
 */

struct Help_link
{
  Help_link(std::string _target, std::string _text) :
    target (_target), text (_text) { }

  std::string target; // The name of the article we link to
  std::string text;   // The text displayed.
};

struct Help_article
{
  Help_article(std::string _name = "", std::string _type = "",
               std::string _text = "");

// set_text() looks for the <link=...> </link> tags and adds them as links.
  void set_text(std::string _text);

  bool is_redirect();

  std::string name;
  std::string type; // The type of article, e.g. "race" "building" etc.
  std::string text;
  std::vector<Help_link> links;

// If redirect is not empty, then loading this article will automatically
// redirect to the article with this name.
  std::string redirect;
};

enum Help_result_type
{
  HELP_RESULT_NULL = 0,
  HELP_RESULT_EXACT, // Matches the title exactly (ignoring upper/lower case)
  HELP_RESULT_TITLE, // Matches part of the title
  HELP_RESULT_TEXT,  // Matches something in the text
  HELP_RESULT_MAX
};

struct Help_result
{
  Help_result(std::string N = "", std::string AT = "",
              Help_result_type T = HELP_RESULT_NULL, bool IR = false) :
    article_name (N), article_type (AT), type (T), is_redirect(IR) { }

  std::string article_name;
  std::string article_type;
  Help_result_type type;
  bool is_redirect;
};

struct Help_database
{
  Help_database();
  ~Help_database();

// Adds it to our map; returns false if that name already exists
  bool add_article(Help_article article);
  bool add_article(Help_article* article);

// process_categories() looks at all articles and sets up our categories map
// (see private section).  Must be ran after all articles are in the system!
  void process_categories();

  int num_articles();
  int num_categories();
// Returns a list of search results, ordered by relevance; exact matches, then
// title matches, then text matches (if those are turned on).
  std::vector<Help_result> search(std::string term,
                                  bool non_exact = true, bool content = true);

// Returns the proper (i.e. capitalized) name of a lookup name (or an empty
// string if the article does not exist).
  std::string get_article_name(std::string term);
// Ditto, but for the article's type.
  std::string get_article_type(std::string term);

// Returns the article with this lookup name, or NULL if it doesn't exist.
  Help_article* get_article(std::string term);

  std::vector<std::string> get_categories();  // Returns all category names

  std::vector<Help_article*> get_articles_in_category(std::string category);
  std::vector<std::string> get_titles_in_category(std::string category);


private:
// Maps the (lower-cased) name of an article to the article itself
  std::map<std::string,Help_article*> articles;

// Maps the name of categories to a vector with all articles of that category
  std::map< std::string,std::vector<Help_article*> > categories;
};

void init_help(); // Sets up all the help articles; see help_data.cpp

#endif
