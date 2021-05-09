#ifndef CC_GAME_TYPE_H
#define CC_GAME_TYPE_H

namespace CcWolf
{
    enum class GameType
    {
        wolf3d,
        spear,
    };

    extern GameType get_game_type();
    extern void     set_game_type();
}

#endif // CC_GAME_TYPE_H
