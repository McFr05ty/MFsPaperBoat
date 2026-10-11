#ifndef PIT_OF_TRIALS_H
#define PIT_OF_TRIALS_H

#ifdef __cplusplus
extern "C" {
#endif

// Asks the game thread to jump to the Pit test room. Safe to call from the menu.
void pit_request_enter(void);

// Called once per game frame. Starts the map change if a request is waiting.
void pit_poll(void);

#ifdef __cplusplus
}
#endif

#endif
