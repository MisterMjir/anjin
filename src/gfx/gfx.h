#ifndef ANJIN_GFX_H
#define ANJIN_GFX_H

int  GFX_init      (void);
void GFX_quit      (void);
void GFX_projection(float, float, float, float, float, float);
void GFX_draw_img  (unsigned int t, float x, float y, float w, float h);
void GFX_draw_imgx (unsigned int t, float x, float y, float w, float h, float tx, float ty, float tw, float th, int flipx, int flipy);

#endif