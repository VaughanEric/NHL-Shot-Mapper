using namespace std;

/* Contains info about the goals and shots on goal that will be graphed */

struct Event {
  string team;
  string color;
  string marktype;
  int x;
  int y;
};

int find_game_id(ifstream &fin, string team_abbrev);
vector<Event*> find_events(ifstream &fin);
void create_graph(ifstream &fin, ofstream &fout, vector<Event*> events);
