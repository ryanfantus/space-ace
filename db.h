#ifndef DB_H
#define DB_H

#include <SQLiteCpp/SQLiteCpp.h>

#include <map>
#include <string>
#include <vector>

typedef struct {
  time_t date;
  std::string user;
  int hand;
  int score;
  int won;
} scores_data;

class DBData {
  SQLite::Database db;
  void create_tables(void);
  std::string user;
  /*
  I thought some of my performance problems were from the prepared statement
  calls.  It was not the case, there's a weird delay when I save/hit the
  database.  This didn't fix it, but they are still present.
  Prepared statements, prepared ahead of time, are a good thing.
   */
  std::unique_ptr<SQLite::Statement> stmt_getSet;
  std::unique_ptr<SQLite::Statement> stmt_setSet;

public:
  DBData();
  virtual ~DBData();

  void setUser(std::string currentUser) { user = currentUser; };
  void clearUser(void) { user.clear(); };
  std::string getSetting(const std::string &setting, std::string ifMissing);
  void setSetting(const std::string &setting, const std::string &value);
  void saveScore(time_t when, time_t date, int hand, int won, int score);

  std::vector<scores_data> getScoresOnDay(time_t date);
  std::map<time_t, std::vector<scores_data>> getScores(void);
  void expireScores(void);

  int handsPlayedOnDay(time_t day);
};

void normalizeDate(time_t &tt, int hour = 2);
std::string convertDateToDateScoreFormat(time_t tt);

#endif