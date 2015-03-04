#include "help.h"
#include "stringfunc.h" // For no_caps()
#include "window.h" // For debugmsg()

// _type and _text default to ""
Help_article::Help_article(std::string _name, std::string _type,
                           std::string _text)
{
  name = _name;
  type = _type;

// Call set_text() so it can set up links
  set_text(_text);
}

void Help_article::set_text(std::string _text)
{
  text = _text;

// Search for links.
  size_t next_link = 0;
  do {  // while (next_link != std::string::npos)

    next_link = text.find("<link=", next_link);

    if (next_link != std::string::npos) {
      size_t target_start = next_link + 6;
      size_t target_stop  = text.find(">", target_start);
      size_t link_stop = text.find("</link>", target_stop);

      next_link = link_stop;  // Next iteration will search after this point.

// Make sure our link is well-formed!
      if (target_stop == std::string::npos) {
        debugmsg("Unclosed <link> tag in help article '%s'.", name.c_str());
        return;
      } else if (link_stop == std::string::npos) {
        debugmsg("<link> tag without matching </link> in help article '%s'.",
                 name.c_str());
        return;
      }

      std::string link_target =
        text.substr(target_start, target_stop - target_start);
      std::string link_text =
        text.substr(target_stop + 1, link_stop - target_stop - 1);

      links.push_back( Help_link( link_target, link_text ) );

    } // if (next_link != std::string::npos)

  } while (next_link != std::string::npos);

// Now we need to replace all the instances of "<link=...>" with "<c=ltblue>"
// and "</link>" with "<c=/>"

  size_t open_link;
  do {
    open_link = text.find("<link");

    if (open_link != std::string::npos) {
      size_t link_end = text.find(">", open_link);
      size_t length = link_end - open_link + 1;

      text.replace(open_link, length, "<c=ltblue>");
    }

  } while (open_link != std::string::npos);

  size_t close_link;
  do {
    close_link = text.find("</link>");

    if (close_link != std::string::npos) {
      text.replace(close_link, 7, "<c=/>");
    }
  } while (close_link != std::string::npos);

}

Help_database::Help_database()
{
}

Help_database::~Help_database()
{
  for (std::map<std::string,Help_article*>::iterator it = articles.begin();
       it != articles.end();
       it++) {
    delete (it->second);
  }
}

bool Help_database::add_article(Help_article article)
{
  Help_article* new_article = new Help_article;
  *new_article = article;

  return add_article(new_article);
}

bool Help_database::add_article(Help_article* article)
{
  if (article->name.empty()) {
    debugmsg("Attempted to add a nameless article to the help database!");
    return false;
  }

  std::string lookup_name = no_caps(article->name);

  if (articles.count(lookup_name)) {
    debugmsg("Attempted to add article '%s' to the help database, but it \
already exists!", lookup_name.c_str());
    return false;
  }

  articles[lookup_name] = article;

  return true;
}

std::vector<Help_result> Help_database::search(std::string term)
{
  term = no_caps(term);
  std::vector<Help_result> all_matches, title_matches, text_matches;

// First, check for an exact match.
  if (articles.count(term)) {
    Help_result exact_match( get_article_name(term), HELP_RESULT_EXACT );
    all_matches.push_back( exact_match );
  }

// Next, check for articles where the title is a *partial* match, or the text
// matches.
  for (std::map<std::string,Help_article*>::iterator it = articles.begin();
       it != articles.end();
       it++) {
    std::string lookup_name = it->first;
    std::string* text = &(it->second->text);

    if (it->first != term) {  // Skip the exact match we already used.

      if (lookup_name.find(term) != std::string::npos) {
        Help_result title_match( it->second->name, HELP_RESULT_TITLE );
        title_matches.push_back( title_match );

      } else if (text->find(term) != std::string::npos) {
        Help_result text_match( it->second->name, HELP_RESULT_TEXT );
        text_matches.push_back( text_match );
      }

    }
  }

// Now, put it all together in the same vector.
  for (int i = 0; i < title_matches.size(); i++) {
    all_matches.push_back( title_matches[i] );
  }
  for (int i = 0; i < text_matches.size(); i++) {
    all_matches.push_back( text_matches[i] );
  }

  return all_matches;
}

std::string Help_database::get_article_name(std::string term)
{
  term = no_caps(term);

  if (articles.count(term)) {
    return articles[term]->name;
  }

  return std::string();
}

Help_article* Help_database::get_article(std::string term)
{
  term = no_caps(term);

  if (articles.count(term)) {
    return articles[term];
  }

  return NULL;
}
