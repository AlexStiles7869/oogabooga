#define MAX_ENTITY_COUNT 1024
#define APPLICATION_LOGGING 1

#include "core/frame_timer.c"

#include "entity/entity.c"
#include "entity/player.c"
#include "entity/tree.c"

variable_frame_timer_t frame_timer = {0};

world_t *world = 0;

void setup_window()
{
    window.title = STR("My Game");
    window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
    window.scaled_height = 720;
    window.x = 200;
    window.y = 90;
    window.clear_color = hex_to_rgba(0x000000ff);
}

void setup_sprites()
{
    sprites[SPRITE_PLAYER] = (sprite_t){.image = load_image_from_disk(fixed_string("assets/trees/Moss_tree2.png"), get_heap_allocator()), .size = v2(64.0, 64.0)};
    sprites[SPRITE_TREE] = (sprite_t){.image = load_image_from_disk(fixed_string("assets/trees/Moss_tree3.png"), get_heap_allocator()), .size = v2(64.0, 64.0)};
}

void create_world_environment(world_t *world)
{
    seed_for_random = os_get_current_cycle_count();

    for (int i = 0; i < 10; i++)
    {
        entity_t *tree_en = create_entity(world);
        setup_tree(tree_en);
        tree_en->pos = v2(get_random_float32_in_range(-250, 250), get_random_float32_in_range(-250, 250));
    }
}

Vector2 handle_movement_keys()
{
    Vector2 input_axis = v2(0, 0);
    if (is_key_down('A'))
    {
        input_axis.x -= 1.0;
    }
    if (is_key_down('D'))
    {
        input_axis.x += 1.0;
    }
    if (is_key_down('S'))
    {
        input_axis.y -= 1.0;
    }
    if (is_key_down('W'))
    {
        input_axis.y += 1.0;
    }

    input_axis = v2_normalize(input_axis);
    return input_axis;
}

void handle_player_movement(entity_t *player_en, Vector2 input_axis, float64 delta_t)
{
    player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, 500 * delta_t));
}

Gfx_Font *font = 0;
const u32 font_height = 16;

// Will be going into another file at some point
void render_sprite(entity_t *en, Matrix4 xform)
{
    sprite_t *en_sprite = get_sprite(en->sprite_type);

    xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
    xform = m4_translate(xform, v3(en_sprite->size.x * -0.5, 0.0, 0.0));

    draw_image_xform(en_sprite->image, xform, en_sprite->size, COLOR_WHITE);

    // debug
    draw_text(font, sprintf(get_temporary_allocator(), "%f %f", en->pos.x, en->pos.y), font_height, en->pos, v2(1, 1), COLOR_WHITE);
}

void render_world(world_t *world)
{
    // render entities
    for (int i = 0; i < MAX_ENTITY_COUNT; i++)
    {
        entity_t *en = &world->entities[i];
        if (en->is_valid)
        {
            Matrix4 xform = m4_scalar(1.0);

            // for future flexibility
            switch (en->arch)
            {
            case PLAYER:
                break;
            default:
                break;
            }

            render_sprite(en, xform);
        }
    }
}

bool almost_equals(float a, float b, float epsilon)
{
    return fabs(a - b) <= epsilon;
}

bool animate_f32_to_target(float *value, float target, float delta_t, float rate)
{
    *value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
    if (almost_equals(*value, target, 0.001f))
    {
        *value = target;
        return true; // reached
    }
    return false;
}

void animate_v2_to_target(Vector2 *value, Vector2 target, float delta_t, float rate)
{
    animate_f32_to_target(&(value->x), target.x, delta_t, rate);
    animate_f32_to_target(&(value->y), target.y, delta_t, rate);
}

int entry(int argc, char **argv)
{
    font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), get_heap_allocator());

    // :window (os-level) init
    setup_window();

    // :hardcoded sprite definitions
    setup_sprites();

    // :memory alloc for world
    world = alloc(get_heap_allocator(), sizeof(world_t));

    // :entity creation
    create_world_environment(world);

    // :player (special entity) creation
    entity_t *player_en = setup_player(create_entity(world));

    // :frame timing
    init_frame_timer(&frame_timer);

    // :camera
    Vector2 camera_pos = v2(0, 0);

    while (!window.should_close)
    {
        // :random os shit
        os_update();
        reset_temporary_storage();

        // :start frame timing
        start_frame_time(&frame_timer);

        // :draw frame

        draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);
        float zoom = 1.6;
        Vector2 target_pos = player_en->pos;
        // camera_pos = player_en->pos;
        animate_v2_to_target(&camera_pos, target_pos, frame_timer.delta_t, 15.0f);
        draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
        draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(camera_pos.x, camera_pos.y, 0)));
        draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0 / zoom, 1.0 / zoom, 1.0)));

        // :meta controls
        if (is_key_just_pressed(KEY_ESCAPE))
        {
            window.should_close = true;
        }

        log("%f %f", input_frame.mouse_x, input_frame.mouse_y);

        // :player positioning
        Vector2 input_axis = handle_movement_keys();
        handle_player_movement(player_en, input_axis, frame_timer.delta_t);

        // :render world entities
        render_world(world);

        end_frame_time(&frame_timer);

        gfx_update();
    }

    return 0;
}