#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <TFT.h>

struct sprite {
    int16_t xpos;
    int16_t ypos;
    uint16_t width;
    uint16_t height;
    uint8_t transparent;
    int8_t frames;
    int8_t currentframe;
    int8_t animdir;
    int8_t store[8];
    const uint8_t *data;
    struct sprite *next;
};

class Framebuffer : public TFT {
    public:
        Framebuffer();
        Framebuffer(int16_t w, int16_t h, DataStore *b);

        static const uint8_t MirrorH = 0x01;
        static const uint8_t MirrorV = 0x02;
        static const uint8_t Rotate180 = 0x03;

        // Basic drawing primitives
        virtual void setPixel(int16_t x, int16_t y, uint16_t c);

        virtual void drawVerticalLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        virtual void drawHorizontalLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        virtual void fillScreen(uint16_t);

        virtual void setAntiAlias(uint8_t aa);

        virtual void copyRect(int16_t dx, int16_t dy, int16_t sx, int16_t sy, uint16_t w, uint16_t h);

        // Color control
        virtual void setColor(uint8_t color, uint16_t rgb);
        virtual void setColor(uint8_t color, uint8_t r, uint8_t g, uint8_t b);
        virtual void loadPalette(const uint16_t *p);
        virtual void loadPalette(const uint8_t p[256][3]);
        virtual void loadPalette(Framebuffer *fb);
        virtual uint16_t colorAt(int16_t x, int16_t y);
        virtual uint16_t bgColorAt(int16_t x, int16_t y);
        virtual void getScanLine(uint16_t y, uint16_t *data);
        virtual void getScanLine(uint16_t y, uint16_t x, uint16_t w, uint16_t *data);

        // Sprites
        virtual struct sprite * addSprite(const uint8_t *data, uint16_t w, uint16_t h, uint8_t t, uint8_t f);
        virtual void removeSprite(struct sprite *s);
        virtual void moveTo(struct sprite *s, int16_t x, int16_t y);
        virtual void moveBy(struct sprite *s, int16_t dx, int16_t dy);
        virtual struct sprite *spriteAt(int16_t x, int16_t y);
        virtual void animate(struct sprite *s);
        virtual void animatePingPong(struct sprite *s);
        virtual struct sprite *collidesWith(struct sprite *s);
        virtual struct sprite *firstSprite();
        virtual struct sprite *nextSprite();
        virtual int8_t getSprite(struct sprite *s, uint8_t n);
        virtual void setSprite(struct sprite *s, uint8_t n, int8_t v);

        virtual void initializeDevice();
        virtual void displayOn() {};
        virtual void displayOff() {};
        virtual void invertDisplay(boolean i) {};

        DataStore *buffer;
        uint16_t palette[256];
        struct sprite *sprites;
        struct sprite *selectedSprite;
        virtual uint8_t bufferRead(uint32_t addr);
        virtual void inline bufferWrite(uint32_t addr, uint8_t value);
        virtual void setRotation(uint8_t rot);

        virtual uint16_t getWidth();
        virtual uint16_t getHeight();

        virtual uint8_t getClosestColor(uint16_t c);
        void translateCoordinates(int16_t *x, int16_t *y);

        virtual void scroll(int16_t dx, int16_t dy);

        virtual void update(TFT *tft);
        virtual void update(TFT *tft, int16_t x0, int16_t y0);
        virtual void update(TFT &tft) { update(&tft); }
        virtual void update(TFT &tft, int16_t x0, int16_t y0) { update(&tft, x0, y0); }

        int32_t _minX, _minY, _maxX, _maxY;

    protected:
        uint16_t _masterWidth;
        uint16_t _masterHeight;
        uint8_t _antiAlias;
};

#endif
