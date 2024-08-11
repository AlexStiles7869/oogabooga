entity_t *setup_player(entity_t *player_en)
{
    player_en->arch = PLAYER;
    player_en->sprite_type = SPRITE_PLAYER;
    return player_en;
}
