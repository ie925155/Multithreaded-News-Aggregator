#include <iostream>
#include <string>
#include <vector>
#include <libxml/parser.h>
#include <libxml/catalog.h>
#include "html-document.h"
#include "html-document-exception.h"
using namespace std;

static void printUsage(const string& executableName) {
  cerr << "Usage: " << executableName << " <html-url>" << endl;
}

static void listTokenCount(const string& url) {
  HTMLDocument document(url);
  try {
    document.parse();
  } catch (const HTMLDocumentException& hde) {
    cerr << "Problem encountered while pulling document " 
         << "content from \"" << url << "\"." << endl;
    cerr << "Specific problem: " << hde.what() << endl;
    return;
  }
  
  const vector<string>& tokens = document.getTokens();
  size_t numTokens = tokens.size();;
  cout << "Document contains " << numTokens << " token" 
       << (numTokens == 1 ? "n" : "s") << "." << endl;
}

static const int kIncorrectArgumentCount = 1;
int main(int argc, const char *argv[]) {
  if (argc != 2) {
    cerr << "Error: wrong number of arguments." << endl;
    printUsage(argv[0]);
    return kIncorrectArgumentCount;
  } 
  
  xmlInitParser();
  xmlInitializeCatalog();
  listTokenCount(argv[1]);
  xmlCatalogCleanup();
  xmlCleanupParser();
  return 0;
}
