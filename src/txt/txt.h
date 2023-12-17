#ifndef ANJIN_TXT_H
#define ANJIN_TXT_H

int  TXT_init(void);
void TXT_quit(void);
void TXT_draw(const char *str, float x, float y, float scale);
void TXT_projection(float l, float r, float b, float t, float back, float front);


#endif