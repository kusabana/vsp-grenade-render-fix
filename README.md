<div align="center">
  <h3><a href="https://github.com/kusabana">
    ~kusabana/</a>vsp-grenade-render-fix
  </h3>

Fix for grenades not rendering at the start of their lifetime
</div>

#### The problem
`CBaseCSGrenadeProjectile::DrawModel`[^1]
```cpp
int CBaseCSGrenadeProjectile::DrawModel( int flags )
{
    // During the first half-second of our life, don't draw ourselves if he's
    // still playing his throw animation.
    // (better yet, we could draw ourselves in his hand).
    if ( GetThrower() != C_BasePlayer::GetLocalPlayer() )
    {
        if ( gpGlobals->curtime - m_flSpawnTime < 0.5 )
        {
            C_CSPlayer *pPlayer = dynamic_cast<C_CSPlayer*>( GetThrower() );
            if ( pPlayer && pPlayer->m_PlayerAnimState->IsThrowingGrenade() )
            {
                return 0;
            }
        }
    }

    return BaseClass::DrawModel( flags );
}
```

This is however not desireable for some community servers running custom gamemodes that make use of the grenade projectiles, like Trikz.[^2]

#### The solution
There are three variables that control whether or not the game will wait before rendering the grenade projectile:
- `m_flSpawnTime`
- `m_hThrower`
- `m_PlayerAnimState`

The `m_flSpawnTime` variable is set by the client. Since the goal is to resolve the issue without any client-side modifications, this option isn't viable.

While `m_PlayerAnimState` might technically be usable, it would require interfering with the animation playback.

This leaves `m_hThrower`. Initially, i considered placing a hook where the server sends the netvar updates to players, and updating the variable for each player to fulfill the local player check. However, this approach is  unnecessarily complex when we can simply set it to an invalid handle, which skips the override due to the check in the innermost if statement.

[^1]: `game/shared/cstrike/basecsgrenade_projectile.cpp` Line 73
[^2]: A cooporative movement gamemode. Although there doesn't exist any good resources describing Trikz as a whole, there are a lot of videos online showcasing it like [this](https://www.youtube.com/watch?v=rM4t9vF9nRk)
