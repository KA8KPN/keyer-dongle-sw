#if !defined(CONTROLLER_H)
#define CONTROLLER_H

#include "keyer.h"
#include "keying.h"

typedef struct key_list {
    unsigned duration;
    bool key_down;
} key_list;

#define KEY_LIST_SIZE 20
#define HIGH_WATER 15
#define LOW_WATER 5

class remote_controller {
public:
    remote_controller(unsigned transmitter);
    void key(bool key_down, unsigned duration);
    input_mode_t update(unsigned long now, input_mode_t input_mode);
    void wpm(int wpm);
    void kud(int twitches);
    void kdd(int ms);
    int wpm(void) const { return m_wpm; }
    int kud(void) const { return m_kud; }
    int kdd(void) const { return m_kdd; }
    void active(bool flag);

private:
    key_list m_keyList[KEY_LIST_SIZE];
    unsigned m_head, m_tail;
    unsigned m_count;
    unsigned m_transmitter;
    bool m_paused;
    unsigned long m_nextTransition;
    unsigned m_wpm;
    unsigned m_kud;
    unsigned m_kdd;
};

extern remote_controller *controllers[MAX_TRANSMITTERS];
void initialize_controllers(void);

#endif // !defined CONTROLLER_H
