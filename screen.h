#ifndef __SCREEN_H__
#define __SCREEN_H__

/**
 * Initializes the screen.
 * @return Returns 0 if successful.
 */
int scr_init(void);

/**
 * Shuts the screen down.
 */
void scr_shutdown(void);

/**
 * Updates the screen.
 */
void scr_update(void);

/**
 * Sets the dirty flag.
 */
void scr_dirty(void);

#endif /*__SCREEN_H__*/
