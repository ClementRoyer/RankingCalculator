#include <bits/stdc++.h>
using namespace std;

#define Match pair<Team, Team>
#define Calendar queue<Match>
#define Team pair<string, pair<int, int>>

unordered_map<string, unordered_map<int, double>> teamResult;
double maxScenario, scenarios;

vector<pair<string, Team>> calculateRanking(unordered_map<string, Team> teams, vector<pair<Match, bool>> results) {
    vector<pair<string, Team>> items(teams.begin(), teams.end());

    std::sort(items.begin(), items.end(),
        [=] (const auto& a, const auto& b) {
            int res = b.second.second.first - a.second.second.first;
            if (res == 0) { // tie break
                for (int i = 0; i < results.size(); i++) {
                    if (results[i].first.first.first == a.second.first && results[i].first.second.first == b.second.first) {
                        return results[i].second;
                    }
                    if (results[i].first.first.first == b.second.first && results[i].first.second.first == a.second.first) {
                        return !results[i].second;
                    }
                }
            }
            return res < 0;
        }
    );
    return items;
}

double lastpr = -1;

void scenarioEnd(unordered_map<string, Team> teams, vector<pair<Match, bool>> results) {
    scenarios++;
    int pr = (scenarios / maxScenario)*100;
    if (pr != lastpr) {
        cout << "\rIn progress : " << pr  << "%";
        lastpr = pr;
    }

    vector<pair<string, Team>> ranking = calculateRanking(teams, results);

    for (int i = 0; i < ranking.size(); i++) {
        Team team = ranking[i].second;
        teamResult[team.first][i]++;
    }
}

void makeMatch(Calendar calendar, unordered_map<string, Team> teams, vector<pair<Match, bool>> results, bool isWin = true) {
    if (calendar.empty()) {
        scenarioEnd(teams, results);
        return;
    }
    if (isWin)
        makeMatch(calendar, teams, results, false);
    Match match = calendar.front();
    calendar.pop();
    
    teams[match.first.first].second.first += isWin;
    teams[match.first.first].second.second += !isWin;
    teams[match.second.first].second.first += !isWin;
    teams[match.second.first].second.second += isWin;
    results.push_back({match, isWin});
    makeMatch(calendar, teams, results, true);
}


int main() {
    Calendar calendar;
    unordered_map<string, Team> teams;
    string name, teama, teamb;
    int victory, defeat, nbTeam, nbMatch;
    cin >> nbTeam;
    
    unordered_map<int, double> ranking;
    for (int i = 0; i < nbTeam; i++) {
        ranking[i] = 0;
    }
    for (int i = 0; i < nbTeam; i++) {
        cin >> name >> victory >> defeat;
        teams[name] = {name, {victory, defeat}};
        teamResult[name] = ranking;
    }
    
    cin >> nbMatch;
    maxScenario = pow(2, nbMatch);
    for (int i = 0; i < nbMatch; i++) {
        cin >> teama >> teamb;
        calendar.push({teams[teama], teams[teamb]});
    }
    
    makeMatch(calendar, teams, {});
    
    for (auto &t : teamResult) {
        cout << "=== Probability of ranking for " << t.first << " :" << endl;
        for (int i = 1; i <= nbTeam; i++) {
            cout << i << ". " << (( t.second[i-1] / maxScenario )*100) << " (" << t.second[i-1] << "/" << maxScenario << ")" << endl;
        }
        cout << endl;
    }
}