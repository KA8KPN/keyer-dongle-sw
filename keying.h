#if !defined(KEYING_H)
#define KEYING_H

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP, KEY_UP_LONGER } keyer_state_t;

#define MAX_TRANSMITTERS  4

class transmitter {
public:
    transmitter(int ptt_line, int key_out_line, int select_led, unsigned ptt_delay, unsigned ptt_hang, void (*sidetone_action)(int, bool));
    void key_up(void);
    unsigned key_down(void);
    void ptt_push(void);
    void ptt_release(void);
    void update(unsigned long now);
    void select(void);
    void unselect(void);
    ~transmitter(void);

protected:
    typedef enum { keying_idle, keying_ptt_wait, keying_key_down, keying_key_up } keyingState_t;
    void key_down_actual(void);
    void (*m_keyAction)(byte, byte);
    void (*m_pttAction)(byte, byte);
    unsigned m_pttDelay;
    unsigned m_pttHang;
    byte m_pttLine;
    byte m_keyOutLine;
    byte m_selectLed;
    keyingState_t m_state;
    unsigned long m_nextStateChange;
    void (*m_sidetoneAction)(int, bool);
};


void keying_initialize(void);
extern transmitter *transmitters[MAX_TRANSMITTERS];


#define KEYING_INITIALIZE()          keying_initialize()
#define TRANSMITTER_KEY_UP(t)        transmitters[t]->key_up()
#define TRANSMITTER_KEY_DOWN(t)      transmitters[t]->key_down()
#define TRANSMITTER_PTT_PUSH(t)      transmitters[t]->ptt_push()
#define TRANSMITTER_PTT_RELEASE(t)   transmitters[t]->ptt_release()
#define TRANSMITTER_SELECT(t)        transmitters[t]->select()
#define TRANSMITTER_UNSELECT(t)      transmitters[t]->unselect()
#define KEYING_UPDATE(t, x)          transmitters[t]->update(x)
#endif // !defined KEYING_H
