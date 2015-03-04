#ifndef _HELP_H_
#define _HELP_H_

#include <string>
#include <vector>
#include <map>

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

  std::string name;
  std::string type; // The type of article, e.g. "race" "building" etc.
  std::string text;
  std::vector<Help_link> links;
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
  Help_result(std::string N = "", Help_result_type T = HELP_RESULT_NULL) :
    article_name(N), type(T) { }

  std::string article_name;
  Help_result_type type;
};

struct Help_database
{
  Help_database();
  ~Help_database();

// Adds it to our map; returns false if that name already exists
  bool add_article(Help_article article);
  bool add_article(Help_article* article);

// Returns a list of search results, ordered by relevance; exact matches, then
// title matches, then text matches.
  std::vector<Help_result> search(std::string term);

// Returns the proper (i.e. capitalized) name of a lookup name (or an empty
// string if the article does not exist).
  std::string get_article_name(std::string term);

// Returns the article with this lookup name, or NULL if it doesn't exist.
  Help_article* get_article(std::string term);


private:
// Maps the (lower-cased) name of an article to the article itself
  std::map<std::string,Help_article*> articles;
};

void init_help(); // Sets up all the help articles; see help_data.cpp

#endif
