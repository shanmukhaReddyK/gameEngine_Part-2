#include<Game.h>

#include<iostream>

Game::Game( const std::string& config) {
    init(config);
}

void Game::init(const std::string& path) {
    //TODO read in config file here
    // use premade playerconfig, enemyconfig, bulletconfig

    //set up default window parameters
    m_window.create(sf::VideoMode({1280,720}),"SFML_Window");
    m_window.setFramerateLimit(60);

    ImGui::SFML::Init(m_window);

    //scale the imgui ui and text size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale=2.0f;

    spawnPlayer();
}

std::shared_ptr<Entity> Game::player() {
    auto& players = m_entities.getEntities("player");
    assert(players.size()==1); //TODO : learn about assert and why are wwe using this line
    return players.front();
}

void Game::run() {
    //TODO: Add pause functionality here
    // some systems should function while paused (rendering)
    // some systems should not (movement/input)

    while(m_running) {
        //update entityManger
        m_entities.update();

        //required update call to ImGui
        ImGui::SFML::Update(m_window,m_deltaClock.restart());

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sGUI();
        sRender();

        //increment the current frame
        //TODO::may be need to move while imlementing pause
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused) {
    //TODO
}

void Game::spawnPlayer() {
    //TODO: finish adding all properties of the player with correct values from config

    //we creater every entity by calling EntityManager.addEntity(tag);
    //this returns a std::shared_ptr<Enity>, so we use auto to save typing
    auto entity = m_entities.addEntity("player");

    //give entity a transform so that it spawns at(200,200) with velocity (1,1) and angle 0
    entity->add<CTransform>(Vec2f(200.0f,200.0f), Vec2f(0.0f,0.0f), 0.0f);
    
    //the entity shape will have radius 32, 8 sides, dark grey fill , and red outline of thickness 4
    entity->add<CShape>(32.0f, 8, sf::Color(10,10,10), sf::Color(255,0,0), 4.0f);

    //add a input component to the player so we can use inputs
    entity->add<CInput>();
}

//spawn enemy at random position
void Game::spawnEnemy() {
    //TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // enemmy must be spawned completely within the bounds of the window

    //record when the most recent enemy was spawned
    m_lastEnenmySpawnTime=m_currentFrame;
}

//spawn small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies( std::shared_ptr<Entity> e) {
    //TODO: spawn small enemies at the location input enemy e

    //when we create smaller enemy, we have to read the values of the orginal enemy
    //- spawn a number of small enemies equal to vertices of original enemy
    //- set each small enemy to the same color as the original, half the size
    //- small enemies are worth double the points 
}

//spawn a bullet from a given entity to a target location 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target) {
    //TODO: implement the spawning of a bullet which travels towards the target
    //- bullet speed is given as a scaler speed 
    //- set the correspondng velocity by calculating
    auto e = m_entities.addEntity("bullet");
    Vec2f direction = target-Vec2f(entity->get<CTransform>().pos);
    direction.normalize();
    Vec2f velocity= direction*15;
    e->add<CTransform>(Vec2f(entity->get<CTransform>().pos), velocity, 0.0f);
    e->add<CShape>(6.0f, 32, sf::Color(255,255,255), sf::Color(0,0,0), 0.5f);
    
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
    //TODO:implement a special weapon or ability
}

void Game::sMovement() {
    //- you should read the m_player->CInput component to determine if the player is  moving or not
    player()->get<CTransform>().velocity=Vec2f(player()->get<CInput>().right-player()->get<CInput>().left,player()->get<CInput>().down-player()->get<CInput>().up);
    player()->get<CTransform>().velocity.normalize();
    player()->get<CTransform>().velocity*=5; //scale it with speed to get velocity;

    for(auto &e : m_entities.getEntities()) {

      auto& transform = e->get<CTransform>();

        if(transform.exists) {
         transform.pos += transform.velocity;
        }
    }
}

void Game::sLifespan() {
    //TODO: implement all lifespan functionality

    //for all entities
    //if entity has no lifespan component skip it
    //if entity has >0 remaining then subtract 1
    //if it has life span and alive 
    //  scale its alpha channel propely
    //if it has lifespan and its time is up
    //  destroy the entity
}

void Game::sCollision() {
    //TODO: implement all proper collisions between entities
    // be sure to use the collection radius, NOT the shape radius
    
    for (auto b:m_entities.getEntities("bullets")) {

        for (auto e:m_entities.getEntities("enemy")) {
        

        }    
    }

}

void Game::sEnemySpawner() {
    //TODO: code which implements enemy spawing should go here
}

void Game::sGUI() {

    ImGui::Begin("Geometry Wars");

    ImGui::Text("shit goes here");

    ImGui::End();
}

void Game::sRender() {
    //  sample drawing of player we created
    m_window.clear();

    
    for(auto& e: m_entities.getEntities()) {
        //set the position of the entity based on entity's transform->pos
        e->get<CShape>().circle.setPosition(e->get<CTransform>().pos);   

        //set the rotation of the shaped based on the entity's transform->angle
        e->get<CTransform>().angle += 3.0f;
        e->get<CShape>().circle.setRotation(sf::degrees(e->get<CTransform>().angle));
        m_window.draw(e->get<CShape>().circle);
    }
    
    // draw ui the last
    ImGui::SFML::Render(m_window);

    m_window.display();

}

void Game::sUserInput() {
    //the movement system will read variables you set in this function

    while (const auto event = m_window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_window, *event);
            
        if (event->is<sf::Event::Closed>())
            m_running=false;

        if(auto const& key_pressed = event->getIf<sf::Event::KeyPressed>()){
         switch(key_pressed->scancode){
                case sf::Keyboard::Scancode::W:
                    std::cout<<"W key is pressed!\n";
                    player()->get<CInput>().up = true;
                    break;

                case sf::Keyboard::Scancode::D:
                    std::cout<<"D key is pressed!\n";
                    player()->get<CInput>().right = true;
                    break;
                
                case sf::Keyboard::Scancode::S:
                    std::cout<<"S key is pressed!\n";
                    player()->get<CInput>().down = true;
                    break;
                
                case sf::Keyboard::Scancode::A:
                    std::cout<<"A key is pressed!\n";
                    player()->get<CInput>().left = true;
                    break;
                
                default :
                    break;
            }
        
        }

        if(auto const& key_pressed = event->getIf<sf::Event::KeyReleased>()){
            
            switch(key_pressed->scancode){
                case sf::Keyboard::Scancode::W:
                    std::cout<<"W key is released!\n";
                    player()->get<CInput>().up = false;
                    break;

                case sf::Keyboard::Scancode::D:
                    std::cout<<"D key is released!\n";
                    player()->get<CInput>().right = false;
                    break;
                
                case sf::Keyboard::Scancode::S:
                    std::cout<<"S key is released!\n";
                    player()->get<CInput>().down = false;
                    break;
                
                case sf::Keyboard::Scancode::A:
                    std::cout<<"A key is released!\n";
                    player()->get<CInput>().left = false;
                    break;
                
                default :
                    break;
            }
        }

        if(event->is<sf::Event::MouseButtonPressed>()) {

            //this line ignores mouse events if ImGui is the thing being clicked
            if(ImGui::GetIO().WantCaptureMouse) {continue;}

            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                std::cout<<"left mouse button is clicked at (" << sf::Mouse::getPosition(m_window).x <<", "<<sf::Mouse::getPosition(m_window).y<<")\n";
                spawnBullet(player(),Vec2f(sf::Mouse::getPosition(m_window).x,sf::Mouse::getPosition(m_window).y));
            }

             if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {

                std::cout<<"right mouse button is clicked at (" << sf::Mouse::getPosition(m_window).x <<", "<<sf::Mouse::getPosition(m_window).y<<")\n";
                //call spawnSpecialweapon here
            }
        }
    }
}