/*
 * Eric Vaughan
 * February 24, 2024
 * Lab 1: Jgraph
 *
 * This program visualizes goals and shots on goal from NHL games with current teams.
 */

#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <vector>
#include "nsm.h"
using namespace std;

/* NHL team abbreviations */

set<string> teams = {
  "ANA", "ARI", "BOS", "BUF", 
  "CAR", "CBJ", "CGY", "CHI", 
  "COL", "DAL", "DET", "EDM", 
  "FLA", "LAK", "MIN", "MTL", 
  "NJD", "NSH", "NYI", "NYR", 
  "OTT", "PHI", "PIT", "SEA", 
  "SJS", "STL", "TBL", "TOR", 
  "VAN", "VGK", "WPG", "WSH"
};

/* For checking the format of the date */

regex date_format{ "....-..-.." };

int main(int argc, char* argv[])
{
  string team_abbrev;       /* Abbreviation of the team being searched */
  string date;              /* Date of the game being searched */
  string output_file;       /* The file name of the resulting NHL shot map jpeg */
  ostringstream oss;        /* For executing commands */
  ifstream fin;             /* Used to read json files */
  int game_id;              /* Identifies the game being searched */
  vector<Event*> events;    /* Info about the goals and shots that will be graphed */
  ofstream fout;            /* Used to create Jgraph file */
  size_t i;                 /* Iterator */

  /* Read in check the team, game date, and output file name */

  if (argc != 4) {
    printf("usage: ./nsm [team-abbreviation] [date-of-game:(YYYY-MM-DD)] [output-file-name]\n");
    return 1;
  }

  team_abbrev = argv[1];
  date = argv[2];
  output_file = argv[3];

  /* Error check the team and game date */
  
  if (teams.find(team_abbrev) == teams.end()) {
    printf("ERROR: %s is not the abbreviation of an NHL team\n", team_abbrev.c_str());
    return 1;
  }
  
  if (date.length() != 10 || !regex_search(date, date_format)) {
    printf("ERROR: %s is not a valid date of the format YYYY-MM-DD\n", date.c_str());
    return 1;
  }

  /* Run curl to obtain the specified team's games from the week with the specified date */

  printf("Finding the game...\n");
  oss << "curl -S -s -o games.json https://api-web.nhle.com/v1/club-schedule/" << team_abbrev << "/week/" << date;
  system(oss.str().c_str());

  /* Obtain the game id of the specific game being searched for */

  fin.open("games.json");
  if (fin.fail()) {
    printf("ERROR: Could not find any NHL games on %s\n", date.c_str());
    return 1;
  }
  game_id = find_game_id(fin, team_abbrev);
  fin.close();
  if (game_id == -1) {
    printf("ERROR: %s did not have a game on %s\n", team_abbrev.c_str(), date.c_str());
    return 1;
  }
  
  /* Run curl to obtain the game's play-by-play events */
  
  printf("Retrieving play-by-play information from the game...\n");
  oss.str("");
  oss << "curl -S -s -o play_by_play.json https://api-web.nhle.com/v1/gamecenter/" << game_id << "/play-by-play";
  system(oss.str().c_str());

  /* Obtain information about all events pertaining to goals and shots on goal */

  fin.open("play_by_play.json");
  if (fin.fail()) {
    printf("ERROR: Could not retrieve information from the game.\n");
    return 1;
  }
  events = find_events(fin);
  fin.close();
  if (events.size() == 0) {
    printf("ERROR: Could not retrieve information from the game.\n");
    return 1;
  }

  /* Create the Jgraph file with information for plotting the rink, goals, and shots on goal */

  printf("Graphing the goals and shots on goal...\n");
  fin.open("rink.jgr");
  if (fin.fail()) {
    printf("ERROR: Could not create the hockey rink on which to plot the data.\n");
    return 1;
  }
  fout.open("plot_shots.jgr");
    if (fin.fail()) {
    printf("ERROR: Could not create the Jgraph file with data.\n");
    return 1;
  }
  create_graph(fin, fout, events);
  fin.close();
  fout.close();

  /* Create the graph */

  oss.str("");
  oss << "./jgraph -P plot_shots.jgr | ps2pdf - | convert -density 300 - -quality 100 " << output_file << ".jpg";
  system(oss.str().c_str());

  /* Remove temporary files and delete events */

  printf("Cleaning up...\n");
  remove("games.json");
  remove("play_by_play.json");
  remove("plot_shots.jgr");
  for (i = 0; i < events.size(); i++) delete events[i];
  printf("Finished: Your NHL shot map (%s.jpg) has been created!\n", output_file.c_str());
  return 0;
}
