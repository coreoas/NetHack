extern "C" {
#include "tile2x11.h"
extern short glyph2tile[]; // from tile.c
}
#include "qt5_port.h"
#include "qt5_icons.h"

#include <QGraphicsPixmapItem>
#include <QBitmap>

NHMapWindow::NHMapWindow(QPixmap *tiles, int tile_size, QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::black);
    setScene(scene);
    setMinimumWidth(300);
    setMinimumHeight(400);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    this->tiles = tiles;
    this->tile_size = tile_size;
    petmark = QPixmap(petmark_xpm);

    draw_glyph(-3, -3, objnum_to_glyph(S_stone));
    max_x = 0;
    max_y = 0;
}

void NHMapWindow::clear()
{
    scene->clear();
    max_x = 0;
    max_y = 0;
    draw_glyph(-3, -3, cmap_to_glyph(S_stone));
}

void NHMapWindow::ensure_visible(int x, int y)
{
    ensureVisible(x * tile_size, y * tile_size, tile_size, tile_size, 10 * tile_size, 5 * tile_size);
}

void NHMapWindow::draw_glyph(int x, int y, int glyph)
{
    if (x > max_x || y > max_y) {
        // add an empty glyph to ensure margins
        clear_glyph(max_x + 5, max_y + 5);
        QGraphicsPixmapItem *new_boundary_glyph = scene->addPixmap(tiles[glyph2tile[cmap_to_glyph(S_stone)]]);
        new_boundary_glyph->setPos((std::max(x, max_x) + 5) * tile_size, (std::max(y, max_y) + 5) * tile_size);
        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
    }

    // First clear any existing glyph
    clear_glyph(x, y);

    // Then draw the new one, optionnaly with the pet mark
    QGraphicsPixmapItem *new_glyph = scene->addPixmap(tiles[glyph2tile[glyph]]);
    new_glyph->setPos(x * tile_size, y * tile_size);
    if (glyph_is_pet(glyph)) {
        QGraphicsPixmapItem *new_petmark = scene->addPixmap(petmark);
        new_petmark->setPos((x + 1) * tile_size - petmark.width(), y * tile_size);
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
