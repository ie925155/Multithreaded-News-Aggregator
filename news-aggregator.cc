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
#include "thread-pool.h"
#include "ostreamlock.h"

using namespace std;

// globals
static bool verbose = false;
static RSSIndex index;
mutex m;

static const int kIncorrectUsage = 1;
static const size_t mNumThreads = 6;
static const size_t kNumThreads = 24;

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

  map<string, string> feedsmap = feedList.getFeeds();
  ThreadPool feed_pool(mNumThreads);
  //handle all feeds
  for_each(feedsmap.begin(), feedsmap.end(), [&feed_pool](const pair<string, string> &it) {
    cout << it.first << " => " << it.second << '\n';
    feed_pool.schedule([it] {
      cout << oslock << "schedule feed started " << it.first << endl
          << osunlock;
      RSSFeed feed(it.first);
      feed.parse();
      vector<Article> articleVector = feed.getArticles();
      ThreadPool pool(kNumThreads);
      //handle articles of feed
      for_each(articleVector.begin(), articleVector.end(), [&pool](Article a) {
        //cout << "url=" << a.url << endl;
        //cout << " title=" << a.title << endl;
        pool.schedule([a] {
          cout << oslock << "Parse article \"" << a.url << "\" has started." << endl
               << osunlock;
          struct Article article = {a.url, a.title};
          HTMLDocument document(a.url);
          document.parse();
          m.lock();
          index.add(cref(article), cref(document.getTokens()));
          m.unlock();
          cout << oslock << "Parse article \"" << a.url << "\" ended." << endl
               << osunlock;
        });
      });
      cout << oslock << "sheldon pool.wait" << endl
               << osunlock;
      pool.wait();
      cout << oslock << "sheldon pool.wait ended." << endl
               << osunlock;
    }); // end of feed_pool lambda
  });
  cout << oslock << "sheldon wait feed_pool" << endl
       << osunlock;
  feed_pool.wait();
  cout << oslock << "sheldon feed_pool.wait ended." << endl
      << osunlock;
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
