/*
 * Eric Vaughan
 * February 24, 2024
 * Lab 1: Jgraph
 *
 * This program visualizes hockey shots and goals.
 */

#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include "nsm.h"
using namespace std;

/* NHL team abbreviations */

set<string> teams = {
  "ANA", "ARI", "BOS", "BUF", 
  "CAR", "CBJ", "CGY", "CHI", 
  "COL", "DAL", "DET", "EDM", 
  "FLA", "LA",  "MIN", "MTL", 
  "NJ",  "NSH", "NYI", "NYR", 
  "OTT", "PHI", "PIT", "SEA", 
  "SJ",  "STL", "TB",  "TOR", 
  "VAN", "VGK", "WPG", "WSH"
};

/* For checking the format of the date */

regex date_format{ "....-..-.." };

int main(int argc, char* argv[])
{
  string team_abbrev;    /* Abbreviation of the team being searched */
  string date;           /* Date of the game being searched */
  ostringstream oss;     /* For executing commands */
  ifstream fin;          /* File stream */
  int game_id;        /* Identifies the game being searched */

  /* Read in and error check the team and game date */

  if (argc != 3) {
    printf("usage: ./nsm [team-abbreviation] [date-of-game:(YYYY-MM-DD)]\n");
    return 1;
  }

  team_abbrev = argv[1];
  date = argv[2];

  if (teams.find(team_abbrev) == teams.end()) {
    printf("ERROR: %s is not the abbreviation of an NHL team\n", team_abbrev.c_str());
    return 1;
  }
  
  if (date.length() != 10 || !regex_search(date, date_format)) {
    printf("ERROR: %s is not a valid date of the format YYYY-MM-DD\n", date.c_str());
    return 1;
  }

  /* Obtain the game id to later get the game data */

  printf("Finding the game...\n");
  oss << "curl -S -s -o games.json https://api-web.nhle.com/v1/schedule/" << date;
  system(oss.str().c_str());

  fin.open("games.json");
  if (fin.fail()) {
    printf("ERROR: Could not find any NHL games on %s\n", date.c_str());
    return 1;
  }

  game_id = find_game_id(fin, team_abbrev);
  if (game_id == -1) {
    printf("ERROR: %s did not have a game on %s\n", team_abbrev.c_str(), date.c_str());
    return 1;
  }
  
  /* Obtain the game's play-by-play stats for shots and goals */
  
  return 0;
}