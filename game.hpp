/* Created by Darren Otgaar on 2017/01/08. http://www.github.com/otgaard/zap */
#ifndef ZAPEROIDS_GAME_HPP
#define ZAPEROIDS_GAME_HPP

class game {
public:
    game(int lives=3) : players_(1), level_(1) {
        lives_[0] = lives; lives_[1] = lives;
        score_[0] = 0; score_[1] = 0;
    }

    game(int players, int lives=3) : players_(players), level_(1) {
        lives_[0] = lives; lives_[1] = lives;
        score_[0] = 0; score_[1] = 0;
    }

    int players() const { return players_; }

    void set_level(int level) { level_ = level; }
    int get_level() const { return level_; }

    void add_points(int player, int points) { score_[player] += points; }
    int get_points(int player) const { return score_[player]; }

    int lives(int player) const { return lives_[player]; }
    void kill(int player) { --lives_[player]; }

    bool game_over() const { return players_ == 1 ? lives_[0] == 0 : lives_[0] == 0 || lives_[1] == 0; }
    int dead_player() const { return players_ == 2 ? (lives_[0] == 0 ? 0 : 1) : 0; }

private:
    int players_;
    int level_;
    int lives_[2];
    int score_[2];
};

#endif //ZAPEROIDS_GAME_HPP
