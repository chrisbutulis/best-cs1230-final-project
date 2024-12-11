#ifndef PLAYER_H
#define PLAYER_H

class player
{
public:
    enum class PlayerType {
        Fisherman,
        Fish
    };
    PlayerType playerType;
    player();
    player(PlayerType t);

};

#endif // PLAYER_H
