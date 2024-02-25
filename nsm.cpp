#include <fstream>
#include <string>
#include "json.hpp"
#include "nsm.h"
using namespace std;
using json = nlohmann::json;

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
  games = sched_info["gameWeek"][0]["games"];
  for (git = games.begin(); git != games.end(); git++) {
    if ((*git)["homeTeam"]["abbrev"] == team_abbrev || 
        (*git)["awayTeam"]["abbrev"] == team_abbrev) {
          return (*git)["id"].get<int>();
    }
  }
  return -1;
}