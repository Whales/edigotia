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

// Check if we already have that link.
      bool has_link = false;
      for (int i = 0; !has_link && i < links.size(); i++) {
        if (no_caps(links[i].target) == no_caps(link_target)) {
          has_link = true;
        }
      }

      if (!has_link) {
        links.push_back( Help_link( link_target, link_text ) );
      }

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

bool Help_article::is_redirect()
{
  return !redirect.empty();
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

  std::string category = no_caps(article->type);

  return true;
}

void Help_database::process_categories()
{
  categories.clear(); // Just in case

  for (std::map<std::string,Help_article*>::iterator it = articles.begin();
       it != articles.end();
       it++) {
    Help_article* article = it->second;
    std::string category = no_caps(article->type);
    if (!category.empty()) {  // Redirect articles have no category
// Add the article to the category list, too
      if (categories.count(category) == 0) {
// Make a new vector
        std::vector<Help_article*> tmpvec;
        tmpvec.push_back(article);
        categories[category] = tmpvec;
      } else {
        categories[category].push_back(article);
      }
    }
  }
}


int Help_database::num_articles()
{
  return articles.size();
}

int Help_database::num_categories()
{
  return categories.size();
}

// non_exact and content both default to true
std::vector<Help_result> Help_database::search(std::string term,
                                               bool non_exact, bool content)
{
  term = no_caps(term);
  std::vector<Help_result> all_matches, title_matches, text_matches;

// First, check for an exact match.
  if (articles.count(term)) {
    Help_result exact_match( get_article_name(term), get_article_type(term),
                             HELP_RESULT_EXACT );
    all_matches.push_back( exact_match );
  }

// Next, check for articles where the title is a *partial* match, or the text
// matches.
  if (non_exact || content) {
    for (std::map<std::string,Help_article*>::iterator it = articles.begin();
         it != articles.end();
         it++) {
      std::string lookup_name = it->first;
      std::string* text = &(it->second->text);

      if (it->first != term) {  // Skip the exact match we already used.

        if (non_exact && lookup_name.find(term) != std::string::npos) {
          Help_result title_match( it->second->name, it->second->type,
                                  HELP_RESULT_TITLE, it->second->is_redirect());
          title_matches.push_back( title_match );

        } else if (content && text->find(term) != std::string::npos) {
          Help_result text_match( it->second->name, it->second->type,
                                  HELP_RESULT_TEXT, it->second->is_redirect() );
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

  } // if (non_exact || content)

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

std::string Help_database::get_article_type(std::string term)
{
// Use get_article() in case there's a redirect.
  Help_article* article = get_article(term);
  if (article) {
    return article->type;
  }

  return std::string();
}

Help_article* Help_database::get_article(std::string term)
{
  term = no_caps(term);

  if (articles.count(term)) {
    Help_article* ret = articles[term];
    if (!ret->redirect.empty()) {
      Help_article* redirect = get_article(ret->redirect);
// There may be multiple redirects!
      while (redirect && !redirect->redirect.empty()) {
        ret = redirect;
        redirect = get_article(redirect->redirect);
      }
// If redirect eventually becomes null, we'll use the last-non-null article
      if (redirect) {
        return redirect;
      }
    }
    return ret;
  }

  return NULL;
}

std::vector<std::string> Help_database::get_categories()
{
  std::vector<std::string> ret;

  for (std::map< std::string,std::vector<Help_article*> >::iterator it =
         categories.begin();
       it != categories.end();
       it++) {
    ret.push_back( capitalize(it->first) );
  }

  return ret;
}

std::vector<Help_article*> Help_database::get_articles_in_category(std::string
                                                                   category)
{
  category = no_caps(category);

  if (categories.count(category) == 0) {  // Category does not exist!
    return std::vector<Help_article*>();
  }

  return categories[category];
}

std::vector<std::string> Help_database::get_titles_in_category(std::string
                                                               category)
{
  category = no_caps(category);

  std::vector<std::string> ret;
  
  std::vector<Help_article*> article_list = get_articles_in_category(category);

  for (int i = 0; i < article_list.size(); i++) {
    ret.push_back( article_list[i]->name );
  }

  return ret;
}
