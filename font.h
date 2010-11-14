#ifndef __FONT_H__
#define __FONT_H__

Font *        font_load           ( const char *filename, int width, int height );
void          font_unload         ( Font *font );
int           font_get_text_width ( Font *font, const char *text );
SDL_Surface * font_draw_text      ( Font *font, const char *text );

#endif /* __FONT_H__ */
