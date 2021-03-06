//	GFX code to get Adafruit 1.8" TFT shield working with chipKIT uc32 and UNO32
//	This code derived from Adafruit_GFX library. See bottom of .h file for their license stuff.
//  This port to chipKIT written by Chris Kelley of ca-cycleworks.com  (c) ? Sure, ok same license thing, whatever
////////////////////////////////////////////////////////////////////////////////
#include <TFT.h>

uint16_t rgb(uint32_t c) {
    uint8_t r = c >> 16;
    uint8_t g = c >> 8;
    uint8_t b = c;

    r = r >> 3;
    g = g >> 2;
    b = b >> 3;
    return ((r << 11) | (g << 5) | b);
}

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    r = r >> 3;
    g = g >> 2;
    b = b >> 3;
    return ((r << 11) | (g << 5) | b);
}

TFT::TFT() {
    _comm = NULL;
    cursor_y = cursor_x = 0;
    textcolor = 0xFFFF;
    textbgcolor = 0;
    wrap = true;
    font = NULL; //Fonts::Default;
}

TFT::TFT(TFTCommunicator *comm) {
    _comm = comm;
    cursor_y = cursor_x = 0;
    textcolor = 0xFFFF;
    textbgcolor = 0;
    wrap = true;
    font = NULL; //Fonts::Default;
}

TFT::TFT(TFTCommunicator &comm) {
    _comm = &comm;
    cursor_y = cursor_x = 0;
    textcolor = 0xFFFF;
    textbgcolor = 0;
    wrap = true;
    font = NULL; //Fonts::Default;
}


// draw a circle outline
void TFT::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    setPixel(x0, y0+r, color);
    setPixel(x0, y0-r, color);
    setPixel(x0+r, y0, color);
    setPixel(x0-r, y0, color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        setPixel(x0 + x, y0 + y, color);
        setPixel(x0 - x, y0 + y, color);
        setPixel(x0 + x, y0 - y, color);
        setPixel(x0 - x, y0 - y, color);
        setPixel(x0 + y, y0 + x, color);
        setPixel(x0 - y, y0 + x, color);
        setPixel(x0 + y, y0 - x, color);
        setPixel(x0 - y, y0 - x, color);
    }
}

void TFT::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            setPixel(x0 + x, y0 + y, color);
            setPixel(x0 + y, y0 + x, color);
        } 
        if (cornername & 0x2) {
            setPixel(x0 + x, y0 - y, color);
            setPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            setPixel(x0 - y, y0 + x, color);
            setPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            setPixel(x0 - y, y0 - x, color);
            setPixel(x0 - x, y0 - y, color);
        }
    }
}

void TFT::fillCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
    int32_t r2 = radius * radius;
    for (int y1 = 0-radius; y1 <= 0; y1++) {
        int32_t y12 = y1 * y1;
        for (int x1 = 0-radius; x1 <= 0; x1++) {
            if (x1 * x1 + y12 <= r2) {
                drawHorizontalLine(x0 + x1, y0 + y1, 2 * (0-x1), color);
                drawHorizontalLine(x0 + x1, y0 - y1, 2 * (0-x1), color);
                break;
            }
        }
    }
}


// used to do circles and roundrects!
void TFT::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            drawVerticalLine(x0+x, y0-y, 2*y+0+delta, color);
            drawVerticalLine(x0+y, y0-x, 2*x+0+delta, color);
        }
        if (cornername & 0x2) {
            drawVerticalLine(x0-x, y0-y, 2*y+0+delta, color);
            drawVerticalLine(x0-y, y0-x, 2*x+0+delta, color);
        }
    }
}

// bresenham's algorithm - thx wikpedia
void TFT::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            setPixel(y0, x0, color);
        } else {
            setPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// draw a rectangle
void TFT::drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawHorizontalLine(x, y, w, color);
    drawHorizontalLine(x, y+h-1, w, color);
    drawVerticalLine(x, y, h, color);
    drawVerticalLine(x+w-1, y, h, color);
}

void TFT::fillRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            setPixel(x + j, y + i, color);
        }
    }
}


void TFT::fillScreen(uint16_t color) {
    fillRectangle(0, 0, getWidth(), getHeight(), color);
}

// draw a rounded rectangle!
void TFT::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    drawHorizontalLine(x+r  , y    , w-2*r, color); // Top
    drawHorizontalLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawVerticalLine(  x    , y+r  , h-2*r, color); // Left
    drawVerticalLine(  x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// fill a rounded rectangle!
void TFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    fillRectangle(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// draw a triangle!
void TFT::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// fill a triangle!
void TFT::fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        std::swap(y0, y1); std::swap(x0, x1);
    }
    if (y1 > y2) {
        std::swap(y2, y1); std::swap(x2, x1);
    }
    if (y0 > y1) {
        std::swap(y0, y1); std::swap(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawHorizontalLine(a, y0, b-a+1, color);
        return;
    }

    int16_t
        dx01 = x1 - x0,
        dy01 = y1 - y0,
        dx02 = x2 - x0,
        dy02 = y2 - y0,
        dx12 = x2 - x1,
        dy12 = y2 - y1,
        sa   = 0,
        sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) std::swap(a,b);
        drawHorizontalLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) std::swap(a,b);
        drawHorizontalLine(a, y, b-a+1, color);
    }
}

void TFT::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t i, j, byteWidth = (w + 7) / 8;

    for(j=0; j<h; j++) {
        for(i=0; i<w; i++ ) {
            if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                setPixel(x+i, y+j, color);
            }
        }
    }
}

void TFT::drawRGB(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    int16_t i, j;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            setPixel(x+i, y+j, bitmap[j * w + i]);
        }
    }
}

void TFT::drawRGBA(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint16_t trans) {
    int16_t i, j;
    uint16_t col;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            col = bitmap[j * w + i];
            if (col != trans) {
                setPixel(x+i, y+j, col);
            }
        }
    }
}

uint16_t TFT::stringWidth(char *text) {
    uint16_t w = 0;
    if (font == NULL) {
        return 0;
    }
    FontHeader *header = (FontHeader *)font;

    for (char *t = text; *t; t++) {
        char c = *t;
        if (c >= header->startGlyph && c <= header->endGlyph) {
            uint8_t co = c - header->startGlyph;
            uint32_t charstart = (co * ((header->linesPerCharacter * header->bytesPerLine) + 1)) + sizeof(FontHeader); // Start of character data
            uint8_t charwidth = font[charstart++];
            w += charwidth;
        }
    }
    return w;
}
        
uint16_t TFT::stringHeight(char *text) {
    uint16_t w = 0;
    if (font == NULL) {
        return 0;
    }
    FontHeader *header = (FontHeader *)font;

    return header->linesPerCharacter;
}
        

#if ARDUINO >= 100
size_t TFT::write(uint8_t c) {
    if (font == NULL) {
        return 0;
    }
#else
void TFT::write(uint8_t c) {
    if (font == NULL) {
        return;
    }
#endif
    FontHeader *header = (FontHeader *)font;

    if (c == '\n') {
        cursor_y += header->linesPerCharacter;
        cursor_x = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        if (c >= header->startGlyph && c <= header->endGlyph) {
            uint8_t co = c - header->startGlyph;
            uint32_t charstart = (co * ((header->linesPerCharacter * header->bytesPerLine) + 1)) + sizeof(FontHeader); // Start of character data
            uint8_t charwidth = font[charstart++];
            if (wrap && (cursor_x > (getWidth() - charwidth))) {
                cursor_y += header->linesPerCharacter;
                cursor_x = 0;
            }
            cursor_x += drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor);
        }
    }
#if ARDUINO >= 100
    return 1;
#endif
}

// draw a character
uint8_t TFT::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg) {

    if (font == NULL) {
        return 0;
    }

    FontHeader *header = (FontHeader *)font;

    if (c < header->startGlyph || c > header->endGlyph) {
        return 0;
    }

    c = c - header->startGlyph;

    // Start of this character's data is the character number multiplied by the
    // number of lines in a character (plus one for the character width) multiplied
    // by the number of bytes in a line, and then offset by 4 for the header.
    uint32_t charstart = (c * ((header->linesPerCharacter * header->bytesPerLine) + 1)) + sizeof(FontHeader); // Start of character data
    uint8_t charwidth = font[charstart++]; // The first byte of a block is the width of the character

    uint32_t bitmask = (1 << header->bitsPerPixel) - 1;

    for (int8_t lineNumber = 0; lineNumber < header->linesPerCharacter; lineNumber++ ) {
//        uint64_t line = 0;
//        for (int8_t j = 0; j < header->bytesPerLine; j++) {
//            line <<= 8;
//            line |= font[charstart + (i * header->bytesPerLine) + j];
//        }
        uint8_t lineData = 0;

        int8_t bitsLeft = -1;
        uint8_t byteNumber = 0;

        for (int8_t pixelNumber = 0; pixelNumber < charwidth; pixelNumber++) {
            if (bitsLeft <= 0) {
                bitsLeft = 8;
                lineData = font[charstart + (lineNumber * header->bytesPerLine) + (header->bytesPerLine - byteNumber - 1)];
                byteNumber++;
            }
            uint32_t pixelValue = lineData & bitmask;
            if (pixelValue > 0) {
                uint16_t bgc = bg;
                if (bg == color) {
                    bgc = colorAt(x+pixelNumber, y+lineNumber);
                }
                setPixel(x+pixelNumber, y+lineNumber, mix(bgc, color, 255 * pixelValue / bitmask));
            } else if (bg != color) {
                setPixel(x+pixelNumber, y+lineNumber, bg);
            }
            lineData >>= header->bitsPerPixel;
            bitsLeft -= header->bitsPerPixel;
        }
    }
    return charwidth;
}

void TFT::setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

int16_t TFT::getCursorX() {
    return cursor_x;
}

int16_t TFT::getCursorY() {
    return cursor_y;
}

int16_t TFT::getCursor(boolean x) {
    if( x )
        return cursor_x;
    return cursor_y;
}

void TFT::setTextColor(uint16_t c) {
    textcolor = c;
}

void TFT::setTextColor(uint16_t fg, uint16_t bg) {
   textcolor = fg;
   textbgcolor = bg;
}

void TFT::invertTextColor( ){
	setTextColor( textbgcolor, textcolor );
}

void TFT::setTextWrap(boolean w) {
  wrap = w;
}

uint16_t TFT::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void TFT::setFont(const uint8_t *f) {
    font = f;
}

uint16_t TFT::getTextColor() {
    return textcolor;
}

point3d TFT::rgb2xyz(uint16_t rgb) {
    uint8_t red = rgb >> 11;
    uint8_t green = rgb >> 5 & 0b111111;
    uint8_t blue = rgb & 0b11111;

    point3d xyz;

    red = red << 3;
    green = green << 2;
    blue = blue << 3;

    float r = red / 255.0;
    float g = green / 255.0;
    float b = blue / 255.0;

    if (r > 0.04045) {
        r  = pow(((r + 0.055) / 1.055), 2.4);
    } else {
        r  = r / 12.92;
    }

    if (g > 0.04045) {
        g  = pow(((g + 0.055) / 1.055), 2.4);
    } else {
        g  = g / 12.92;
    }

    if (b > 0.04045) {
        b  = pow(((b + 0.055) / 1.055), 2.4);
    } else {
        b  = b / 12.92;
    }

    r = r * 100.0;
    g = g * 100.0;
    b = b * 100.0;

    xyz.x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    xyz.y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    xyz.z = r * 0.0193 + g * 0.1192 + b * 0.9505;
    return xyz;
}

point3d TFT::xyz2lab(point3d xyz) {
    point3d lab;

    float x = xyz.x / 100.0;
    float y = xyz.y / 100.0;
    float z = xyz.z / 100.0;
    
    if (x > 0.008856) {
        x = pow(x, 1.0/3.0);
    } else {
        x = (7.787 * x) + (16.0 / 116.0);
    }

    if (y > 0.008856) {
        y = pow(y, 1.0/3.0);
    } else {
        y = (7.787 * y) + (16.0 / 116.0);
    }

    if (z > 0.008856) {
        z = pow(z, 1.0/3.0);
    } else {
        z = (7.787 * z) + (16.0 / 116.0);
    }

    lab.x = (116.0 * y) - 16.0;
    lab.y = 500.0 * (x - y);
    lab.z = 200.0 * (y - z);
    return lab;
}

float TFT::deltaE(point3d labA, point3d labB) {
    return sqrt(
        (pow(labA.x - labB.x, 2.0))
        + (pow(labA.y - labB.y, 2.0))
        + (pow(labA.z - labB.z, 2.0))
    );
}


uint32_t TFT::deltaOrth(uint16_t c1, uint16_t c2) {
    uint32_t hsv1 = rgb2hsv(c1);
    uint32_t hsv2 = rgb2hsv(c2);

    uint8_t h1 = (hsv1 >> 16) & 0xFF;
    uint8_t h2 = (hsv2 >> 16) & 0xFF;
    uint8_t s1 = (hsv1 >> 8) & 0xFF;
    uint8_t s2 = (hsv2 >> 8) & 0xFF;
    uint8_t v1 = hsv1 & 0xFF;
    uint8_t v2 = hsv2 & 0xFF;

    int32_t hd = h1 - h2;
    int32_t sd = s1 - s2;
    int32_t vd = v1 - v2;

    uint32_t sos = (hd * hd) + (sd * sd) + (vd * vd);
    return sos;
}

uint32_t TFT::rgb2hsv(uint16_t rgb)
{
    uint8_t r = rgb >> 11;
    uint8_t g = rgb >> 5 & 0b111111;
    uint8_t b = rgb & 0b11111;

    r <<= 3;
    g <<= 2;
    b <<= 3;

    uint8_t h, s, v;

    unsigned char rgbMin, rgbMax;

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    v = rgbMax;
    if (v == 0)
    {
        h = 0;
        s = 0;
        return (h << 16) | (s << 8) | v;
    }

    s = 255 * long(rgbMax - rgbMin) / v;
    if (s == 0)
    {
        h = 0;
        return (h << 16) | (s << 8) | v;
    }

    if (rgbMax == r)
        h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

    return (h << 16) | (s << 8) | v;
}

uint16_t TFT::colorAt(int16_t x, int16_t y) {
    return Color::Black;
}

uint16_t TFT::bgColorAt(int16_t x, int16_t y) {
    return Color::Black;
}

void TFT::openWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    winx0 = x0;
    winy0 = y0;
    winx1 = x1;
    winy1 = y1;
    winpx = 0;
    winpy = 0;
}

void TFT::windowData(uint16_t d) {
    setPixel(winx0 + winpx, winy0 + winpy, d);
    winpx++;
    if (winpx + winx0 >= winx1) {
        winpx = 0;
        winpy++;
    }
    if (winpy + winy0 >= winy1) {
        winpy = 0;
    }
}

void TFT::windowData(uint16_t *d, uint32_t l) {
    for (uint32_t i = 0; i < l; i++) {
        windowData(d[i]);
    }
}

void TFT::closeWindow() {
}

uint16_t TFT::mix(uint16_t a, uint16_t b, uint8_t pct) {
    Color565 col_a;
    Color565 col_b;
    Color565 col_out;
    col_a.value = a;
    col_b.value = b;
    uint32_t temp;
    temp = (((int32_t)col_a.r * (255-pct)) / 255) + (((uint32_t)col_b.r * pct) / 255);
    col_out.r = temp;
    temp = (((int32_t)col_a.g * (255-pct)) / 255) + (((uint32_t)col_b.g * pct) / 255);
    col_out.g = temp;
    temp = (((int32_t)col_a.b * (255-pct)) / 255) + (((uint32_t)col_b.b * pct) / 255);
    col_out.b = temp;
    return col_out.value;
}

boolean TFT::clipToScreen(int16_t &x, int16_t &y, int16_t &w, int16_t &h) {
    if (x < 0) {
        w += x;
        x = 0;
        if (w <= 0) {
            return false;
        }
    }

    if (y < 0) {
        h += y;
        y = 0;
        if (h <= 0) {
            return false;
        }
    }

    if (x >= _width) {
        return false;
    }

    if (y >= _height) {
        return false;
    }

    if (x + w >= _width) {
        w = _width-x;
        if (w <= 0) {
            return false;
        }
    }

    if (y + h >= _height) {
        h = _height-y;
        if (h <= 0) {
            return false;
        }
    }
    return true;
}
