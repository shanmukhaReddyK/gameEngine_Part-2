#include<Game.h>

#include<iostream>
#include<cstdlib>
#include<ctime>

Game::Game(const std::string& config) {
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
    assert(players.size()==1); //TODO : learn about assert and why are we using this line
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
        sLifespan();

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

    //add collision 
    entity->add<CCollision>(30.0f);
}

int Game::sRandnum(int min, int max) {
    srand(time(0));
    return min + (rand() % (max-min+1));
}

//spawn enemy at random position
void Game::spawnEnemy() {
    //* to get random number r u can use r = min + (rand()%(1+max-min))
    // enemmy must be spawned completely within the bounds of the window
    auto entity = m_entities.addEntity("enemy");
   
    //the entity shape will have shape radius, random sides, random fill , and white outline of thickness 2
    //TODO: implement better way to get random color because it gives dame color becaused it is called in same second (using time(0) for thats why)
    entity->add<CShape>(28.0f,sRandnum(3,6), sf::Color(sRandnum(0,255),sRandnum(0,26),sRandnum(0,157)), sf::Color(255,0,0), 2.0f);
    
    //get radius to set limits in pos 
    int radius = entity->get<CShape>().circle.getRadius();

    float randangle=(sRandnum(0,360)*(3.14159f/180));

    //give entity a transform so that it spawns at random pos with  random velocity (within the constraint) and angle 0
    entity->add<CTransform>(Vec2f(sRandnum(0+radius,1280-radius),sRandnum(0+radius,720-radius)), Vec2f(4*std::cos(randangle),4*std::sin(randangle)), 0);

    //add collision
    entity->add<CCollision>(26.0f);

    //record when the most recent enemy was spawned
    m_lastEnenmySpawnTime=m_currentFrame;
}

//spawn small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies( std::shared_ptr<Entity> be) {
    //when we create smaller enemy, we have to read the values of the orginal enemy
    //- spawn a number of small enemies equal to vertices of original enemy
    //- set each small enemy to the same color as the original, half the size
    //- small enemies are worth double the points 

    auto& parentCircle=be->get<CShape>().circle;
    auto& parenrtTrans=be->get<CTransform>();
    int n = parentCircle.getPointCount();
    float anglefraction = (3.14159f*2)/n;
    while(n--) {
        auto entity = m_entities.addEntity("smallenemy");
        //!check if entity->add<CShape>()=be->get<CShape>() works;
        entity->add<CShape>(parentCircle.getRadius()/2,parentCircle.getPointCount(),parentCircle.getFillColor(),parentCircle.getOutlineColor(),parentCircle.getOutlineThickness()/2);
        entity->add<CTransform>(parenrtTrans.pos,Vec2f(4*std::cos((n)*anglefraction),4*std::sin((n)*anglefraction)),0);
        entity->add<CCollision>((parentCircle.getRadius()/2)-1);
        //add lifespan
        entity->add<CLifespan>(90);
    }
}

//spawn a bullet from a given entity to a target location 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target) {
    //- bullet speed is given as a scaler speed 
    //- set the correspondng velocity by calculating
    auto e = m_entities.addEntity("bullet");
    Vec2f direction = target-Vec2f(entity->get<CTransform>().pos);
    direction.normalize();
    Vec2f velocity= direction*20;
    e->add<CTransform>(Vec2f(entity->get<CTransform>().pos), velocity, 0.0f);
    e->add<CShape>(6.0f, 32, sf::Color(255,255,255), sf::Color(0,0,0), 0.5f);

    //add lifespan
    e->add<CLifespan>(90);
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

    for (auto &e:m_entities.getEntities()) {
        auto&lifespan = e->get<CLifespan>();

        if(lifespan.exists) {
            if(lifespan.remaining>0)
                lifespan.remaining--;
            
            //setup alpha channel
            if(e->isAlive()) {
                sf::Color c = e->get<CShape>().circle.getFillColor();
                e->get<CShape>().circle.setFillColor({c.r,c.g,c.b,std::uint8_t((255*lifespan.remaining)/lifespan.lifespan)});
                e->get<CShape>().circle.setOutlineColor({255,0,0,std::uint8_t((255*lifespan.remaining)/lifespan.lifespan)});
            }

            if(lifespan.remaining<=0) {
                e->destroy();
            }
        }
    }
}

void Game::sCollision() {
    // be sure to use the collection radius, NOT the shape radius
    for (auto &b:m_entities.getEntities("bullet")) {

        for (auto &e:m_entities.getEntities("enemy")) {
        
            if(b->get<CTransform>().pos.dist(e->get<CTransform>().pos)<b->get<CCollision>().radius+e->get<CCollision>().radius) {
                e->destroy();
                b->destroy();
                spawnSmallEnemies(e);
                std::cout<<"booom\n";
            }

        }   
        
        for (auto &se:m_entities.getEntities("smallenemy")) {
        
            if(b->get<CTransform>().pos.dist(se->get<CTransform>().pos)<b->get<CCollision>().radius+se->get<CCollision>().radius) {
                se->destroy();
                b->destroy();
                std::cout<<"booom\n";
            }

        }
    }

    for (auto &e:m_entities.getEntities()) {
        auto& tran=e->get<CTransform>();
        float radius=e->get<CCollision>().radius;

        if(tran.pos.x<radius || tran.pos.x+radius>1280){
            tran.velocity.x*=-1;
        }

        if(tran.pos.y<radius || tran.pos.y+radius>720){
            tran.velocity.y*=-1;
        }
    }

}

void Game::sEnemySpawner() {
    //call enemy spawner every time interval mentioned in enemyconfig file
    if(m_currentFrame==m_lastEnenmySpawnTime+60) {
        spawnEnemy();
    }
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