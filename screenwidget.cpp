#include "screenwidget.h"
#include <QPainter>
#include <algorithm>
static const QColor s_palette[2][16]{
        {
        QColor(  0,   0,   0),
        QColor(  0,   0, 192),
        QColor(192,   0,   0),
        QColor(192,   0, 192),
        QColor(  0, 192,   0),
        QColor(  0, 192, 192),
        QColor(192, 192,   0),
        QColor(192, 192, 192),

        QColor(  0,   0,   0),
        QColor( 96,  96, 255),
        QColor(255,  96,  96),
        QColor(255,  96, 255),
        QColor( 96, 255,  96),
        QColor( 96, 255, 255),
        QColor(255, 255,  96),
        QColor(255, 255, 255)},
        {
        QColor(  0,   0,   0),
        QColor( 96,  96, 159),
        QColor(159,  96,  96),
        QColor(159,  96, 159),
        QColor( 96, 159,  96),
        QColor( 96, 159, 159),
        QColor(159, 159,  96),
        QColor(96, 96, 96),

        QColor(  0,   0,   0),
        QColor(159, 159, 255),
        QColor(253, 159, 159),
        QColor(255, 159, 255),
        QColor(159, 255, 159),
        QColor(159, 255, 255),
        QColor(255, 255, 159),
        QColor(159, 159, 159)}
    };


ScreenWidget::ScreenWidget(QWidget *parent) : QWidget(parent)
{

}


void ScreenWidget::setBusInterface(const BusInterface *bi)
{
    _bi = bi;
}

void ScreenWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.fillRect(rect(),s_palette[_palette][_bi->border()]);

    int nw, nh, n;
    nw = (width() - 2 * BORDER_MIN_WIDTH) / SCREEN_WIDTH;
    nh = (height() - 2 * BORDER_MIN_HEIGHT) / SCREEN_HEIGHT;
    n = std::min(nw, nh);
    if (n < 1) n = 1;

    int ox, oy;
    ox = (width()-SCREEN_WIDTH * n )/2;
    oy = (height()-SCREEN_HEIGHT * n) /2;

    auto fb = _bi->frameBuffer();
    const uint8_t * pixelData = &fb[0];
    const uint8_t * attrData = &fb[SCREEN_WIDTH *
                                            SCREEN_HEIGHT / 8];

    int x, y, ax, ay;
    for(y = 0; y < SCREEN_HEIGHT; ++y){
        for (ax = 0; ax < SCREEN_WIDTH / 8; ++ax){
            ay = y / 8;

            uint8_t attr = attrData[ay * SCREEN_WIDTH/8 + ax];
            uint8_t ink = attr & 0b00000111;
            uint8_t paper = (attr & 0b00111000) >> 3;
            uint8_t bright = (attr & 0b01000000) >> 6;
            uint8_t flash = (attr & 0b10000000) >> 7;


            int block_addr = y * SCREEN_WIDTH / 8 + ax;
            int a4a0 = block_addr & 0b0000000000011111;
            int a10a8 = (block_addr & 0b0000011100000000) >> 8;
            int a7a5 = (block_addr & 0b0000000011100000) >> 5;
            int a12a11 = (block_addr & 0b0001100000000000) >> 11;
            int video_addr =
                    a4a0 + (a10a8 << 5) +
                    (a7a5 << 8) + (a12a11 << 11);
            uint8_t data = pixelData[video_addr];
            for (int px = 0; px < 8; ++px){
                x = ax * 8 + px;

                QRect pr(   ox + x * n,
                            oy + y * n,
                            n, n);

                bool pixel = (data & (0b10000000 >> px));
                if (pixel xor (flash_state & flash)){
                    p.fillRect(pr, s_palette[_palette][ink + 8 * bright]);
                }
                else
                    p.fillRect(pr, s_palette[_palette][paper]);
            }

        }
    }



}
