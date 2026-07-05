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

This is however not desireable for some community servers running custom gamemodes that make use of the grenade projectiles, like Trikz.

[^1]: `game/shared/cstrike/basecsgrenade_projectile.cpp` Line 73
