extern "C" {
#include "tile2x11.h"
extern short glyph2tile[]; // from tile.c
}

#include "qt5_port.h"

#include <QGraphicsPixmapItem>
#include <QBitmap>

NHMapWindow::NHMapWindow(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::black);
    setScene(scene);

    QPixmap tilemap("x11tiles", "XPM");
    pet_mark = new QPixmap(pet_mark_xpm);
    tile_size = tilemap.width() / 40;

    int i, j;
    for (j = 0; j < tilemap.height() / tile_size; j++) {
        for (i = 0; i < TILES_PER_ROW; i++) {
            tiles[i + TILES_PER_ROW * j] = tilemap.copy(i * tile_size,
                                                        j * tile_size,
                                                        tile_size,
                                                        tile_size);
        }
    }
}

void NHMapWindow::clear_glyph(int x, int y)
{
    QList<QGraphicsItem *> old_items = scene->items(x * tile_size,
                                                    y * tile_size,
                                                    tile_size,
                                                    tile_size,
                                                    Qt::ContainsItemShape,
                                                    Qt::AscendingOrder);

    while (!old_items.isEmpty()) {
        QGraphicsItem *old_item = old_items.takeFirst();
        scene->removeItem(old_item);
        delete old_item;
    }
}

void NHMapWindow::draw_glyph(int x, int y, int glyph)
{
    // First clear any existing glyph
    clear_glyph(x, y);

    // Then draw the new one, optionnaly with the pet mark
    QGraphicsPixmapItem *new_glyph = scene->addPixmap(tiles[glyph2tile[glyph]]);
    new_glyph->setPos(x * tile_size, y * tile_size);
    if (glyph_is_pet(glyph)) {
        QGraphicsPixmapItem *new_pet_mark = scene->addPixmap(*pet_mark);
        new_pet_mark->setPos((x + 1) * tile_size - pet_mark->width(), y * tile_size);
    }
}
