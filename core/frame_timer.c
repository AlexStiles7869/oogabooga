typedef struct
{
    // :variable frame counting
    float64 now;
    float64 last_time;
    float64 delta_t;

    // :fps counting
    float64 seconds_counter;
    s32 frame_count;
} variable_frame_timer_t;

void init_frame_timer(variable_frame_timer_t *frame_timer)
{
    // Zero everything
    memset(frame_timer, 0, sizeof(variable_frame_timer_t));

    // Start frame timing
    frame_timer->last_time = os_get_current_time_in_seconds();
}

void start_frame_time(variable_frame_timer_t *frame_timer)
{
    frame_timer->now = os_get_current_time_in_seconds();
    frame_timer->delta_t = frame_timer->now - frame_timer->last_time;
    frame_timer->last_time = frame_timer->now;
}

void end_frame_time(variable_frame_timer_t *frame_timer)
{
    frame_timer->seconds_counter += frame_timer->delta_t;
    frame_timer->frame_count += 1;
    if (frame_timer->seconds_counter > 1.0)
    {

#ifdef APPLICATION_LOGGING
        log("FPS: %i", frame_timer->frame_count);
#endif
        frame_timer->seconds_counter = 0.0;
        frame_timer->frame_count = 0;
    }
}