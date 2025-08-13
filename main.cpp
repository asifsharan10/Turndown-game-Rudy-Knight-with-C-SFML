#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

const int TILE_SIZE = 16;
const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 38;

// 0 = grass, 1 = wall
int level[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
};

bool isWallAt(float x, float y) {
    int tileX = static_cast<int>(x) / TILE_SIZE;
    int tileY = static_cast<int>(y) / TILE_SIZE;

    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT)
        return true;

    return level[tileY][tileX] == 1;
}

struct Enemy {
    sf::Sprite sprite;
    sf::Vector2f velocity;

    Enemy(const sf::Texture& texture, const sf::Vector2f& startPos) {
        sprite.setTexture(texture);
        sprite.setScale(0.2f, 0.2f);
        sprite.setPosition(startPos);
    }

    void update(float dt, const sf::Vector2f& playerPos) {
        sf::Vector2f pos = sprite.getPosition();
        sf::Vector2f diff = playerPos - pos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist < 200.f) {
            sf::Vector2f direction = diff / dist;
            velocity = direction * 40.f;
        } else {
            velocity = { 0.f, 0.f };
        }

        sf::Vector2f nextPos = pos + velocity * dt;
        if (!isWallAt(nextPos.x + 8, nextPos.y + 8))
            sprite.setPosition(nextPos);
    }
};

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 608;

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Top-Down Adventure Game");
    window.setFramerateLimit(60);

    sf::Texture grassTexture, wallTexture, playerTexture, enemyTexture;
    if (!grassTexture.loadFromFile("assets/grass.png") ||
        !wallTexture.loadFromFile("assets/wall.png") ||
        !playerTexture.loadFromFile("assets/player.png") ||
        !enemyTexture.loadFromFile("assets/enemy.png")) {
        // If texture fails, just show red circle
        sf::CircleShape fallback(50);
        fallback.setFillColor(sf::Color::Red);
        fallback.setPosition(200, 200);

        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            window.draw(fallback);
            window.display();
        }

        return -1;
    }

    sf::Sprite grass(grassTexture);
    sf::Sprite wall(wallTexture);

    sf::Sprite player(playerTexture);
    player.setScale(0.1f, 0.1f);
    player.setPosition(64, 64);

    float speed = 100.0f;
    sf::Clock clock;
    sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

    // Spawn enemy near player
    Enemy enemy(enemyTexture, sf::Vector2f(120, 120));

    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Player input
        sf::Vector2f movement(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += speed;

        sf::Vector2f nextPos = player.getPosition() + movement * dt.asSeconds();
        sf::FloatRect nextBounds = player.getGlobalBounds();
        nextBounds.left = nextPos.x + 8.f;
        nextBounds.top = nextPos.y + 8.f;
        nextBounds.width -= 16.f;
        nextBounds.height -= 16.f;

        bool blocked = (
            isWallAt(nextBounds.left, nextBounds.top) ||
            isWallAt(nextBounds.left + nextBounds.width, nextBounds.top) ||
            isWallAt(nextBounds.left, nextBounds.top + nextBounds.height) ||
            isWallAt(nextBounds.left + nextBounds.width, nextBounds.top + nextBounds.height)
        );

        if (!blocked)
            player.setPosition(nextPos);

        // Enemy AI
        enemy.update(dt.asSeconds(), player.getPosition());

        // View follows player
        sf::Vector2f center = player.getPosition() + sf::Vector2f(player.getGlobalBounds().width / 2, player.getGlobalBounds().height / 2);
        view.setCenter(center);
        window.setView(view);

        // Draw map
        window.clear();
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                sf::Sprite& tile = (level[y][x] == 1) ? wall : grass;
                tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(tile);
            }
        }

        window.draw(player);
        window.draw(enemy.sprite);
        window.display();
    }

    return 0;
}
