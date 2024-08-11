typedef struct
{
    Gfx_Image *image;
    Vector2 size;
} sprite_t;

typedef enum
{
    SPRITE_NIL,
    SPRITE_PLAYER,
    SPRITE_TREE,
    SPRITE_MAX,
} sprite_type_t;

// ewww
sprite_t sprites[SPRITE_MAX];

sprite_t *get_sprite(sprite_type_t id)
{
    if (id >= 0 && id < SPRITE_MAX)
    {
        return &sprites[id];
    }

    return &sprites[0];
}

typedef enum
{
    NIL = 0,
    PLAYER = 1,
    ROCK = 2,
    TREE = 3,
} entity_archetype_t;

typedef struct
{
    bool is_valid;
    entity_archetype_t arch;
    Vector2 pos;

    bool render_sprite;
    sprite_type_t sprite_type;
} entity_t;

typedef struct
{
    entity_t entities[MAX_ENTITY_COUNT];
} world_t;

entity_t *create_entity(world_t *world)
{
    // Find next free entity and return
    entity_t *entity_found = 0;
    for (int i = 0; i < MAX_ENTITY_COUNT; i++)
    {
        entity_t *existing_entity = &(world->entities[i]);
        if (!existing_entity->is_valid)
        {
            entity_found = existing_entity;
            break;
        }
    }
    assert(entity_found, "No more free entities!");

    entity_found->is_valid = true;

    return entity_found;
}

void destroy_entity(entity_t *entity)
{
    // Just zero the memory for now
    memset(entity, 0, sizeof(entity_t));
}