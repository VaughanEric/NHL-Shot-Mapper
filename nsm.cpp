#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "json.hpp"
#include "nsm.h"
using namespace std;
using json = nlohmann::json;

/* Maps the id of a team to its name */

map<int, string> abbrevs = {
  {1, "NJD"},
  {2, "NYI"},
  {3, "NYR"},
  {4, "PHI"},
  {5, "PIT"},
  {6, "BOS"},
  {7, "BUF"},
  {8, "MTL"},
  {9, "OTT"},
  {10, "TOR"},
  {12, "CAR"},
  {13, "FLA"},
  {14, "TBL"},
  {15, "WSH"},
  {16, "CHI"},
  {17, "DET"},
  {18, "NSH"},
  {19, "STL"},
  {20, "CGY"},
  {21, "COL"},
  {22, "EDM"},
  {23, "VAN"},
  {24, "ANA"},
  {25, "DAL"},
  {26, "LAK"},
  {28, "SJS"},
  {29, "CBJ"},
  {30, "MIN"},
  {52, "WPG"},
  {53, "ARI"},
  {54, "VGK"},
  {55, "SEA"}
};

/* Maps the id of a team to its colors */

map<int, string> colors = {
  {1, "1 0 0"},
  {2, "0 0 1"},
  {3, "0 0 1"},
  {4, "1 0.33 0"},
  {5, "0 0 0"},
  {6, "1 0.75 0.11"},
  {7, "0 0.16 0.41"},
  {8, "1 0 0"},
  {9, "1 0 0"},
  {10, "0 0 1"},
  {12, "1 0 0"},
  {13, "1 0 0"},
  {14, "0 0 1"},
  {15, "1 0 0"},
  {16, "1 0 0"},
  {17, "1 0 0"},
  {18, "1 0.75 0.11"},
  {19, "0 0 1"},
  {20, "1 0 0"},
  {21, "0.51 0.16 0.24"},
  {22, "1 0.33 0"},
  {23, "0 0.16 0.41"},
  {24, "1 0.33 0"},
  {25, "0 0.6 0.28"},
  {26, "0 0 0"},
  {28, "0 0.6 0.6"},
  {29, "0 0.16 0.41"},
  {30, "0 0.6 0.28"},
  {52, "0 0.16 0.41"},
  {53, "0.51 0.16 0.24"},
  {54, "0.3 0.3 0.3"},
  {55, "0 0.6 0.6"}
};

/*
 * @name find_game_id.
 * @description Find the game id of the game being searched.
 * @param[in] fin The json being read.
 * @param[in] team_abbrev The team being searched.
 * @return The game id of the game being searched. If not found, return -1.
 */
int find_game_id(ifstream &fin, string team_abbrev)
{
  json sched_info;       /* Info about the games on a given date */
  json games;            /* The games played on a given date */
  json::iterator git;    /* Iterator */    

  /* Find the game id of the game being searched */

  sched_info = json::parse(fin);
  games = sched_info["games"];
  for (git = games.begin(); git != games.end(); git++) {
    if ((*git)["homeTeam"]["abbrev"] == team_abbrev || 
        (*git)["awayTeam"]["abbrev"] == team_abbrev) {
          return (*git)["id"].get<int>();
    }
  }
  return -1;
}

/*
 * @name find_events.
 * @description Obtains info about the goals and shots that will be graphed.
 * @param[in] fin The json being read.
 * @return Info about the goals and shots that will be graphed.
 */
vector<Event*> find_events(ifstream &fin)
{
  vector<Event*> events;            /* Info about the goals and shots that will be graphed */
  Event* event;                     /* Info about an event that is a goal or shot */
  json game_info;                   /* Info about the specified game */
  json plays;                       /* The play-by-play from the specified game */
  json::iterator pit;               /* Iterator */
  int id, home_id, away_id;         /* Team ids */
  string home_color, away_color;    /* Team colors */

  /* Obtain team colors */
  /* If two teams have the same color, change the away team's color to gray */

  game_info = json::parse(fin);
  home_id = game_info["homeTeam"]["id"];
  home_color = colors[home_id];
  away_id = game_info["awayTeam"]["id"];
  away_color = colors[away_id];
  if (home_color == away_color) {
    away_color = "0.3 0.3 0.3";
  }

  /* Obtain info about the goals and shots on goal that will be graphed */

  plays = game_info["plays"];
  for (pit = plays.begin(); pit != plays.end(); pit++) {
    if ((*pit)["typeDescKey"] == "shot-on-goal" || (*pit)["typeDescKey"] == "goal") {
      id = (*pit)["details"]["eventOwnerTeamId"];
      event = new Event;
      event->team = abbrevs[id];
      event->color = (id == home_id) ? home_color : away_color;
      event->marktype = ((*pit)["typeDescKey"] == "goal") ? "circle" : "x";
      event->x = (*pit)["details"]["xCoord"];
      event->y = (*pit)["details"]["yCoord"];
      events.push_back(event);
    }
  }
  return events;
}

/*
 * @name create_graph.
 * @description Creates the Jgraph file that will plot the hockey rink and shots.
 * @param[in] fin The Jgraph file that contains how to graph the hockey rink.
 * @param[in] fout The Jgraph file that will contain the hockey rink and shots information.
 * @param[in] events Information about goals and shots on goal.
 * @return Info about the goals and shots that will be graphed.
 */
void create_graph(ifstream &fin, ofstream &fout, vector<Event*> events)
{
  string line;     /* Used to read lines in from hockey rink Jgraph file */
  size_t i;        /* Iterator */
  Event* event;    /* Goal or shot-on-goal event */

  /* Add the hockey rink graphing instructions to the new Jgraph file */

  while (getline(fin, line)) {
    fout << line << "\n";
  }

  /* Add the goal and shot-on-goal events to the Jgraph file */

  for (i = 0; i < events.size(); i++) {
    event = events[i];
    fout << "newline marktype " << event->marktype;
    fout << " color " << event->color;
    fout << "\npts " << event->x << " " << event->y << "\n\n";
  }

  /* Create the legend */

  fout << "newline marktype circle color " << events[0]->color << " linetype none label : " << events[0]->team << " Goal\n";
  fout << "newline marktype x color " << events[0]->color << " linetype none label : " << events[0]->team << " Shot on Goal\n";
  
  for (i = 0; i < events.size()-1 && events[i]->team == events[0]->team; i++);
  event = events[i+1];

  fout << "newline marktype circle color " << event->color << " linetype none label : " << event->team << " Goal\n";
  fout << "newline marktype x color " << event->color << " linetype none label : " << event->team << " Shot on Goal\n";
  fout << "legend defaults font Times-Italic fontsize 12 hjc vjt\n";
  return;
}
