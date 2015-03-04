#include "help.h"

#define _article(n) \
  cur_article = new Help_article( (n) ); \
  HELP.add_article(cur_article)

#define _type(n) \
  cur_article->type = (n)

#define _text(n) \
  cur_article->set_text( (n) )

void init_help()
{
  Help_article* cur_article;
}
