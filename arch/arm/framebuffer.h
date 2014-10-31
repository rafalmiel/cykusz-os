#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

void framebuffer_init(void);

void framebuffer_draw_char(char ch);
void framebuffer_draw_string(const char *str);

#endif // FRAMEBUFFER_H
