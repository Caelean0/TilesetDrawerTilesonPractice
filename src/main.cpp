#include <cstdlib>

#include "raylib.h"

#include "config.h"
#include "tileson.h"

void DrawNineSlice(Texture2D texture, Rectangle totalSize, float borderSize);

int main() {
    // Raylib initialization
    // Project name, screen size, fullscreen mode etc. can be specified in the config.h.in file
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(Game::ScreenWidth, Game::ScreenHeight, Game::PROJECT_NAME);
    SetTargetFPS(60);

#ifdef GAME_START_FULLSCREEN
    ToggleFullscreen();
#endif

    // Your own initialization code here
    // ...
    // ...
    Texture2D tilesetGraphic = LoadTexture("assets/graphics/Grassland_Tileset.png");
    Texture2D closeButton = LoadTexture("assets/graphics/closebutton.png");
    Texture2D nineSlice = LoadTexture("assets/graphics/nineslice.png");
    tson::Tileson tileson;

    RenderTexture2D canvas = LoadRenderTexture(960, 540);
    auto MapPtr = tileson.parse("assets/data/test_corrected.tmj");
    tson::Map& Map = *MapPtr;
    Camera2D cam = {};
    cam.zoom = 1.0f;
    float renderScale{};
    Rectangle renderRec{};
    Rectangle sourceRec{};

    if (Map.getStatus() != tson::ParseStatus::OK) {
        std::cout << "Failed to parse map, error: " << Map.getStatusMessage() << std::endl;
    }

    std::vector<tson::Layer> layers = Map.getLayers();

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) { //Fullscreen logic.
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(Game::ScreenWidth,Game::ScreenHeight);
            } else {
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
                ToggleFullscreen();
            }
        }
        float scale = 1;
        // Updates that are made by frame are coded here
        // ...
        // ...
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (GetMouseX() > GetScreenWidth() - 48 && GetMouseY() < 48) {
                CloseWindow();
            }
        }
        //camera movement
        if (IsKeyPressed(KEY_LEFT)){
            cam.target.x -= 96;
        }
        if (IsKeyPressed(KEY_RIGHT)){
            cam.target.x += 96;
        }
        if (IsKeyPressed(KEY_UP)){
            cam.target.y -= 96;
        }
        if (IsKeyPressed(KEY_DOWN)){
            cam.target.y += 96;
        }

        if (IsKeyPressed(KEY_V))
        {
            SetWindowSize(960, 540);
        }



        BeginDrawing();
        // You can draw on the screen between BeginDrawing() and EndDrawing()
        // ...
        // ...
        BeginTextureMode(canvas);
        {
            BeginMode2D(cam);
            {
                Color bgColor;
                bgColor.a = 255;
                bgColor.r = Map.getBackgroundColor().r;
                bgColor.g = Map.getBackgroundColor().g;
                bgColor.b = Map.getBackgroundColor().b;
                ClearBackground(bgColor);
                //start of map drawing
                const int tileSize = Map.getTileSize().x;
                const int currentFrame = int(GetTime() * 30) % 4;
                for (tson::Layer layer: Map.getLayers())
                {

                    /**
                     * @attention: this code does nothing, just to show how to use group layers
                     */
                    if (layer.getType() == tson::LayerType::Group)
                    {
                        std::vector<tson::Layer> groupLayers = layer.getLayers();
                        for (tson::Layer groupLayer : groupLayers)
                        {
                            if (groupLayer.getType() == tson::LayerType::TileLayer)
                            {
                                continue;
                            }
                        }
                    }



                    if (layer.getType() == tson::LayerType::TileLayer)
                    {
                        if (!layer.isVisible())
                        {
                            continue;
                        }
                        for (int y = 0; y < layer.getSize().y; ++y)
                        {
                            for (int x = 0; x < layer.getSize().x; ++x)
                            {

                                //this is how to get the data for a tile
                                tson::Tile* tileptr = layer.getTileData(x,y);
                                if (tileptr == nullptr)
                                {
                                    continue;
                                }
                                tson::Tile tile = *tileptr;

                                //this is an alternate and more complicated way to get layer data
                                // (gets onty the tile coordinates in tileset, not full data)
                                int data = layer.getData()[y * layer.getSize().x + x];
                                data--;
                                if (data < 0)
                                {
                                    continue;
                                }

                                Color color = WHITE;
                                if (tile.getClassType() == "Wall")
                                {
                                    color = {230,41,55,100 };
                                }


                                tson::Rect tileRect = tile.getDrawingRect();
                                Rectangle tileSetRec = {static_cast<float>(tileRect.x), static_cast<float>(tileRect.y), static_cast<float>(tileRect.width), static_cast<float>(tileRect.height)};

//                               This is the approach with the data int:
//                               Rectangle tileSetRec = {(float) (data % (tilesetGraphic.width / tileSize) * tileSize),
//                                                        (float) (data / (tilesetGraphic.width / tileSize) * tileSize),
//                                                        (float) tileSize, (float) tileSize};
                                Rectangle destRec = {(float) (x * tileSize * scale), (float) (y * tileSize * scale),
                                                     (float) tileSize * scale,
                                                     (float) tileSize * scale};

                                /**
                                 * @info: this is a way to handle animation, assuming animation frames are listed after
                                 * one another in the tileset:
                                 */

                                /*if (data >= 0xE0)
                                {
                                    tileSetRec.x += (float) (currentFrame * tileSize);
                                }*/
                                DrawTexturePro(tilesetGraphic, tileSetRec, destRec, {0}, 0, color);
                                if (IsKeyDown(KEY_I))
                                {
                                    //print tile id
                                    std::string text = std::to_string(tile.getId());
                                    float width = MeasureText(text.c_str(), 5);
                                    DrawRectangle(x * tileSize * scale - 4, y * tileSize * scale, width + 8, 30,
                                                  IsKeyDown(KEY_LEFT_SHIFT) ? WHITE : BLACK);
                                    DrawText(std::to_string(data).c_str(), x * tileSize * scale, y * tileSize * scale,
                                             5,
                                             IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE);
                                }
                            }
                        }
                    } else if (layer.getType() == tson::LayerType::ObjectGroup) {
                        for (tson::Object object: layer.getObjects())
                        {
                            if (object.getObjectType() == tson::ObjectType::Polygon)
                            {
                                int id = object.getId();
                                if (object.getClassType() == "Path")
                                {

                                    tson::Vector2i position = object.getPosition();
                                    std::cout << "Polygon: " << std::endl;
                                    std::vector<tson::Vector2i> polygon = object.getPolygons();
                                    for (int i = 0; i < polygon.size(); ++i) {
                                        tson::Vector2i point = polygon[i];
                                        std::cout << point.x + position.x << "," << point.y + position.y << std::endl;
                                    }
                                }
                            } else if (object.getObjectType() == tson::ObjectType::Template) {
                                if (object.getClassType() == "Enemy")
                                {
                                    tson::PropertyCollection& properties = object.getProperties();
                                    Rectangle hitbox = {
                                            properties.getProperty("x")->getValue<float>(),
                                            properties.getProperty("y")->getValue<float>(),
                                            properties.getProperty("width")->getValue<float>(),
                                            properties.getProperty("height")->getValue<float>()
                                    };

                                    DrawRectangleRec(hitbox, RED);
                                    tson::Vector2i position = object.getPosition();
                                    /*std::string tt = static_cast<std::string>(properties.getProperty("tt") ->getValue<std::string>());

//                                    bool enemy = static_cast<bool>(properties.getProperty("enemy")->getValue<bool>());
//                                    float view = static_cast<float>( properties.getProperty("view")->getValue<float>());
                                    tson::TiledClass cla = properties.getProperty("ello")->getValue<tson::TiledClass>();*/




//                                    std::cout << "Point: " << std::endl;
//                                    std::cout << "Position: " << position.x << "," << position.y << "; Enemy: " << enemy << "; View: " << view << std::endl;
                                }

                            }
                        }
                    }
                }
                //draw a box around the map
                DrawRectangleLinesEx(
                        {-16, -16, Map.getSize().x * tileSize * scale + 32, Map.getSize().y * tileSize * scale + 32},
                        16, IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE
                );
                //end of map drawing

                if (IsKeyDown(KEY_G))
                {
                    for (int y = 0; y < Map.getSize().y; ++y)
                    {
                        for (int x = 0; x < Map.getSize().x; ++x)
                        {
                            Color gridColor = ColorAlpha(IsKeyDown(KEY_LEFT_SHIFT) ? BLACK : WHITE, 0.5f);
                            DrawRectangleLines(x * tileSize * scale, y * tileSize * scale, tileSize * scale,
                                               tileSize * scale, gridColor);

                        }
                    }
                }
            }
            EndMode2D();
        }
        EndTextureMode();

        // Draw a rectangle
        renderScale = std::min(GetScreenHeight() / (float) canvas.texture.height, // Calculates how big or small the canvas has to be rendered.
                               GetScreenWidth()  / (float) canvas.texture.width); // Priority is given to the smaller side.
        renderScale = floorf(renderScale);
        if (renderScale < 1) renderScale = 1; // Ensure that scale is at least 1.
        renderRec.width = canvas.texture.width * renderScale;
        renderRec.height = canvas.texture.height * renderScale;
        renderRec.x = (GetScreenWidth() - renderRec.width) / 2.0f;
        renderRec.y = (GetScreenHeight() - renderRec.height) / 2.0f;
        DrawTexturePro(canvas.texture,
                       Rectangle{0, 0, (float) canvas.texture.width, (float) -canvas.texture.height},
                       renderRec,
                       {}, 0, WHITE);
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)) {
            DrawText(TextFormat("Render scale: %.0f", renderScale), 10, 10, 20, LIGHTGRAY);
        }
        //9-Slice segment
        if (IsKeyDown(KEY_NINE)){
            DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.8f));
            Rectangle destRecs[9] = {};
            Rectangle totalSize = {0,0, (float)GetMouseX(), (float)GetMouseY()};
                DrawNineSlice(nineSlice, totalSize, nineSlice.width);
        }

        // Draw a close button
        if (GetMouseX() > GetScreenWidth() - 48 && GetMouseY() < 48) {
            DrawTexture(closeButton, GetScreenWidth() - 48, 0, WHITE);
        }


        EndDrawing();
    } // Main game loop end

    // De-initialization here
    // ...
    // ...
    UnloadTexture(tilesetGraphic);

    // Close window and OpenGL context
    CloseWindow();

    return EXIT_SUCCESS;
}


void DrawNineSlice(Texture2D texture, Rectangle totalSize, float borderSize) {
    float textureBlockSize = texture.width / 3;
    DrawTexturePro( //top left
            texture,
            {0, 0, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //top center
            texture,
            {textureBlockSize, 0, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y, totalSize.width - borderSize * 2, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //top right
            texture,
            {textureBlockSize * 2, 0, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center left
            texture,
            {0, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y + borderSize, borderSize, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center center
            texture,
            {textureBlockSize, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y + borderSize, totalSize.width - borderSize * 2, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //center right
            texture,
            {textureBlockSize * 2, textureBlockSize, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y + borderSize, borderSize, totalSize.height - borderSize * 2},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom left
            texture,
            {0, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x, totalSize.y + totalSize.height - borderSize, borderSize, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom center
            texture,
            {textureBlockSize, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x + borderSize, totalSize.y + totalSize.height - borderSize, totalSize.width - borderSize * 2, borderSize},
            {0, 0},0, WHITE
            );
    DrawTexturePro( //bottom right
            texture,
            {textureBlockSize * 2, textureBlockSize * 2, textureBlockSize, textureBlockSize},
            {totalSize.x + totalSize.width - borderSize, totalSize.y + totalSize.height - borderSize, borderSize, borderSize},
            {0, 0},0, WHITE
            );
}

