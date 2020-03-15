//=========================================================================
/*
 * sin/pls gen
 * LZs,2011
 */
//=========================================================================

#ifndef CFG_H
#define CFG_H

void write_word(uint16_t p,uint16_t d);
uint16_t read_word(uint16_t p,uint16_t min_v,uint16_t max_v);

void save_params();
void load_params();


#endif //CFG_H

