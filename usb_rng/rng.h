
#ifndef RNG_H
#define RNG_H


#define	getBit()	(PINC & 1)

uint16_t getRnd(void);
void initRnd(void);

#endif //RNG_H
