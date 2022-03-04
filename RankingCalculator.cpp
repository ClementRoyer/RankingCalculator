#include <bits/stdc++.h>
using namespace std;


class Team
{
private:
public:
    string name;
    int win;
    int win_scnd;
    unordered_map<string, vector<bool>> history;
    vector<int> classment;
    Team(string);
    Team();
    ~Team();
    void match(string,bool);
};

Team::Team(string name): name(name) {
    this->classment = vector<int>(100, 0);
    this->win_scnd = 0;
    this->win = 0;
}
Team::Team() {}
Team::~Team() {}

void Team::match(string opponent, bool isWin) {
    this->history[opponent].push_back(isWin);
    if (this->history[opponent].size() == 2)
        this->win_scnd += isWin;
    this->win += isWin;
}

unordered_map<string, Team> gv_teams;
int gv_maxCalculate, gv_maxScenario;

void readInput(unordered_map<string, Team> &teams, queue<pair<string, string>> &calendar) {
    string teamA, teamB, winner;
    int nbMatch = 0;
    cin >> nbMatch;

    while (nbMatch--) {
        cin >> teamA >> teamB;

        if (teams.find(teamA) == teams.end()) teams.emplace(teamA, Team(teamA));
        if (teams.find(teamB) == teams.end()) teams.emplace(teamB, Team(teamB));

        if (cin.peek() != '\n') {
            cin >> winner;
            teams[teamA].match(teamB, teamA == winner);
            teams[teamB].match(teamA, teamB == winner);
        } else {
            calendar.push({teamA, teamB});
        }
    }
}

void showProgress() {
    static double progress = 0;
    static double last_progress = 0;
    int pr = (++progress / gv_maxCalculate)*100;
    if (pr != last_progress) {
        cout << "\rIn progress : " << pr << "%";
        last_progress = pr;
    }
}

void calculateRanking(unordered_map<string, Team> _teams) {
    vector<pair<string, Team>> teams(_teams.begin(), _teams.end());
    showProgress();
    // Sort by win
    sort(teams.begin(), teams.end(), 
        [] (pair<string, Team> &teamA, pair<string, Team> &teamB) {
            if (teamA.second.win == teamB.second.win) {
                int face_to_face = teamA.second.history[teamB.first][0] + teamA.second.history[teamB.first][1];
                if (face_to_face == 1)
                    return teamA.second.win_scnd > teamB.second.win_scnd;
                return face_to_face == 2;
            }
            return teamA.second.win > teamB.second.win;
        });
    
    // Rank team by win, face to face, win in scnd split
    // Save it as well to the global team variable
    for (int i = 0, rank = 1; i < teams.size(); i++) {
        int r = rank, tie = 0;
        gv_teams[teams[i].first].classment[rank]++; r++;
        for (tie = i; tie+1 < teams.size() && teams[tie+1].second.win == teams[i].second.win; tie++) {};
        if (tie-i == 1) { // 2 way tie
            int face_to_face = teams[i].second.history[teams[i+1].first][0] + teams[i].second.history[teams[i+1].first][1];
            if (face_to_face == 1)
                if (teams[i].second.win_scnd == teams[i+1].second.win_scnd) {
                    // Equal
                    gv_teams[teams[++i].first].classment[rank]++; r++;

                }
        } else if (tie-i > 1) { // multi tie

            // Don't really know how to calculate 50% up on face to face 
            // and then sort on win_scnd with good time complexity.
            // SO we just make them equal for the moment.
            while (++i != tie) {
                gv_teams[teams[i].first].classment[rank]++; r++;
            }
        }

        rank = r;
    }


    // for (int i = 0; i < teams.size(); i++) {
    //     cout << teams[i].second.name << " :" << teams[i].second.win << " " << teams[i].second.history.size() << endl;
    //     gv_teams[teams[i].first].classment[i+1]++;
    // }
    // exit(0);
}

void rankTeams(unordered_map<string, Team> teams, queue<pair<string, string>> calendar, bool isWin = true) {
    if (calendar.empty()) { calculateRanking(teams); return; }
    if (isWin) rankTeams(teams, calendar, false);
    Team teamA = teams[calendar.front().first], teamB = teams[calendar.front().second];
    calendar.pop();

    teams[teamA.name].match(teamB.name, isWin);
    teams[teamB.name].match(teamA.name, !isWin);
    rankTeams(teams, calendar, true);
}

void showStats() {
    for (auto &team : gv_teams) {
        cout << "=== Probability of ranking for " << team.first << " :" << endl;
        for (int i = 1; i <= gv_teams.size(); i++) {
            cout << i << ". " << (((float)team.second.classment[i] / gv_maxCalculate) * 100) << " ( " << team.second.classment[i] << "/" << gv_maxCalculate << ")" << endl;
        }
        cout << endl;
    }
}

int main() {
    unordered_map<string, Team> teams;
    queue<pair<string, string>> calendar;

    readInput(teams, calendar);
    gv_teams = teams;
    gv_maxCalculate = pow(2, calendar.size());
    gv_maxScenario = pow(2, (gv_teams.size()*2));
    rankTeams(teams, calendar);
    showStats();
    return 0;
}