/**
 * File: news-aggregator.cc
 * ------------------------
 * When fully implemented, news-aggregator.cc pulls, parses,
 * and indexes every single news article reachable from some
 * RSS feed in the user-supplied RSS News Feed XML file,
 * and then allows the user to query the index.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <libxml/parser.h>
#include <libxml/catalog.h>
#include <getopt.h>

#include "article.h"
#include "rss-feed-list.h"
#include "rss-feed.h"
#include "rss-index.h"
#include "html-document.h"
#include "html-document-exception.h"
#include "rss-feed-exception.h"
#include "rss-feed-list-exception.h"
#include "news-aggregator-utils.h"
//#include "string-utils.h"
using namespace std;

// globals
static bool verbose = false;
static RSSIndex index;

static const int kIncorrectUsage = 1;

extern string trim(string &str);

static void printUsage(const string &message, const string &executableName)
{
  cerr << "Error: " << message << endl;
  cerr << "Usage: " << executableName << " [--verbose] [--quiet] [--url <feed-file>]" << endl;
  exit(kIncorrectUsage);
}

static const string kDefaultRSSFeedListURL = "small-feed.xml";
static string parseArgumentList(int argc, char *argv[])
{
  struct option options[] = {
      {"verbose", no_argument, NULL, 'v'},
      {"quiet", no_argument, NULL, 'q'},
      {"url", required_argument, NULL, 'u'},
      {NULL, 0, NULL, 0},
  };

  string url = kDefaultRSSFeedListURL;
  while (true)
  {
    int ch = getopt_long(argc, argv, "vqu:", options, NULL);
    if (ch == -1)
      break;
    switch (ch)
    {
    case 'v':
      verbose = true;
      break;
    case 'q':
      verbose = false;
      break;
    case 'u':
      url = optarg;
      break;
    default:
      printUsage("Unrecognized flag.", argv[0]);
    }
  }

  argc -= optind;
  argv += optind;
  if (argc > 0)
  {
    printUsage("Too many arguments", argv[0]);
  }
  return url;
}

static const int kBogusRSSFeedListName = 1;
static void processAllFeeds(const string &feedListURI)
{
  RSSFeedList feedList(feedListURI);
  try
  {
    feedList.parse();
  }
  catch (const RSSFeedListException &rfle)
  {
    cerr << "Ran into trouble while pulling full RSS feed list from \""
         << feedListURI << "\"." << endl;
    cerr << "Aborting...." << endl;
    exit(kBogusRSSFeedListName);
  }

  for (int i = 0; i < feedList.getFeeds().size(); i++)
  {
    printf("test");
  }

  HTMLDocument document("http://www.facebook.com/jerry");
  // note to student
  // ---------------
  // add well-decomposed code to read all of the RSS
  // news feeds from feedList for their news articles,
  // and for each news article URL, process it
  // as an HTMLDocument and add all of the tokens
  // to the master RSSIndex.
}

static const size_t kMaxMatchesToShow = 15;
static void queryIndex()
{
  while (true)
  {
    cout << "Enter a search term [or just hit <enter> to quit]: ";
    string response;
    getline(cin, response);
    response = trim(response);
    if (response.empty())
      break;
    const vector<pair<Article, int>> &matches = index.getMatchingArticles(response);
    if (matches.empty())
    {
      cout << "Ah, we didn't find the term \"" << response << "\". Try again." << endl;
    }
    else
    {
      cout << "That term appears in " << matches.size() << " article"
           << (matches.size() == 1 ? "" : "s") << ".  ";
      if (matches.size() > kMaxMatchesToShow)
        cout << "Here are the top " << kMaxMatchesToShow << " of them:" << endl;
      else if (matches.size() > 1)
        cout << "Here they are:" << endl;
      else
        cout << "Here it is:" << endl;
      size_t count = 0;
      for (const pair<Article, int> &match : matches)
      {
        if (count == kMaxMatchesToShow)
          break;
        count++;
        string title = match.first.title;
        if (shouldTruncate(title))
          title = truncate(title);
        string url = match.first.url;
        if (shouldTruncate(url))
          url = truncate(url);
        string times = match.second == 1 ? "time" : "times";
        cout << "  " << setw(2) << setfill(' ') << count << ".) "
             << "\"" << title << "\" [appears " << match.second << " " << times << "]." << endl;
        cout << "       \"" << url << "\"" << endl;
      }
    }
  }
}

int main(int argc, char *argv[])
{
  string rssFeedListURI = parseArgumentList(argc, argv);
  xmlInitParser();
  xmlInitializeCatalog();
  processAllFeeds(rssFeedListURI);
  xmlCatalogCleanup();
  xmlCleanupParser();
  queryIndex();
  return 0;
}
