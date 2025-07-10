#pragma once

#include<fstream>
#include"EntityManager.hpp"
#include<SFML/Graphics.hpp>

#include"imgui.h"
#include"imgui-SFML.h"

struct PlayerConfig {int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig {int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI ; float SMAX, SMIN; };
struct BulletConfig {int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
struct WindowConfig {int W, H, FL; bool FS; };
struct FontConfig {std::string fontPath; int S, FR, FG, FB; };

class Game {
    sf::RenderWindow        m_window;    //the window we will draw
    EntityManager           m_entities;  //vectors of enities to maintain
    sf::Font                m_font;      //font we will use to draw
    std::optional<sf::Text> m_text;      //the score text to be drawn to the screen
    PlayerConfig            m_playerConfig;
    EnemyConfig             m_enemyConfig;
    BulletConfig            m_bulletConfig;
    WindowConfig            m_windowConfig;
    FontConfig              m_fontConfig;
    sf::Clock               m_deltaClock;
    int                     m_score=0;
    int                     m_currentFrame=0;
    int                     m_lastEnenmySpawnTime=0;
    bool                    m_paused=false;
    bool                    m_running=true;
    
    void init(const std::string &config); //initialize game from a config file
    void setPaused(bool paused);

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> e);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    void sMovement();
    void sLifespan();
    void sCollision();
    void sEnemySpawner();
    void sGUI();
    void sRender();
    void sUserInput();
    int sRandnum(int min,int max);

    std::shared_ptr<Entity> player();

    public:

    Game(const std::string& connfig);

    void run();

};