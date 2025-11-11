    #include "engine/tile/tile.h"   

    Tile::Tile(Texture* tex, int x, int y, int z)
        : texture(tex), x(x), y(y), z(z)
    {}
    void Tile::render(int tileWidth, int tileHeight, int offsetX, int offsetY) {
        if (!texture) return;

        int baseX = (x - y) * tileWidth / 2;
        int baseY = (x + y) * 5;

        int texWidth  = texture->getWidth();
        int texHeight = texture->getHeight();

        // center texture horizontally over tile footprint
        int screenX = baseX + offsetX - texWidth / 2 + tileWidth / 2;

        // align bottom of texture to base of diamond footprint
        int screenY = baseY + offsetY - texHeight + tileHeight - z * 21;

        texture->setTransform(screenX, screenY);
        texture->render();
    }

