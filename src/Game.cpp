#include<Game.h>

#include<iostream>
#include<cstdlib>
#include<ctime>

Game::Game(const std::string& config) {
    init(config);
    srand(time(0));
}

void Game::init(const std::string& path) {
    // read in config file here
    // use premade playerconfig, enemyconfig, bulletconfig
    std::ifstream fin(path);

    if(!fin.is_open()) {
        std::cerr<<"Error opening file "<< path <<"\n";
    }

    std::string word;

    while(fin >> word) {
        if(word == "Window") {
            fin >> m_windowConfig.W >> m_windowConfig.H >> m_windowConfig.FL >> m_windowConfig.FS;
        }

        else if(word == "Font") {
            fin >> m_fontConfig.fontPath >> m_fontConfig.S >> m_fontConfig.FR >> m_fontConfig.FG >> m_fontConfig.FB;
            
            //initialize font and text
            m_font.openFromFile(m_fontConfig.fontPath);
            m_text = sf::Text(m_font);
            m_text->setCharacterSize(m_fontConfig.S);
            m_text->setFillColor(sf::Color(std::uint8_t(m_fontConfig.FR),std::uint8_t(m_fontConfig.FG), std::uint8_t(m_fontConfig.FB)));
        }

        else if(word=="Player") {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }

        else if(word=="Enemy") {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }

        else if(word=="Bullet") {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }

        else{}
    }

    //set up default window parameters
    if(m_windowConfig.FS) {
        sf::VideoMode desktop=sf::VideoMode::getDesktopMode();
        m_window.create(desktop,"SFML_Window");
        m_windowConfig.W=desktop.size.x;
        m_windowConfig.H=desktop.size.y;
    }
    else {
        m_window.create(sf::VideoMode({uint(m_windowConfig.W),uint(m_windowConfig.H)}),"SFML_Window");
    }
    
    m_window.setFramerateLimit(m_windowConfig.FL);

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

        sUserInput();
        if(m_setSpawning)
            sEnemySpawner();
        
        if(m_setMovement)
            sMovement();
        
        if(m_setCollision)
            sCollision();
        
        if(m_setGui)
            sGUI();
        
        sRender();

        if(m_setLifespan)
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

    //we creater every entity by calling EntityManager.addEntity(tag);
    //this returns a std::shared_ptr<Enity>, so we use auto to save typing
    auto entity = m_entities.addEntity("player");

    //give entity a transform so that it spawns at(200,200) with velocity (1,1) and angle 0
    entity->add<CTransform>(Vec2f(200.0f,200.0f), Vec2f(0.0f,0.0f), 0.0f);
    
    //the entity shape will have radius 32, 8 sides, dark grey fill , and red outline of thickness 4
    entity->add<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color((std::uint8_t(m_playerConfig.FR),std::uint8_t(m_playerConfig.FG), std::uint8_t(m_playerConfig.FB))), sf::Color(std::uint8_t(m_playerConfig.OR),std::uint8_t(m_playerConfig.OG), std::uint8_t(m_playerConfig.OB)),m_playerConfig.OT);
    // entity->add<CShape>(32.0f, 8, sf::Color(10,10,10), sf::Color(255,0,0), 4.0f);
    //add a input component to the player so we can use inputs
    entity->add<CInput>();

    //add collision 
    entity->add<CCollision>(m_playerConfig.CR);
}

int Game::sRandnum(int min, int max) {
    return min + (rand() % (max-min+1));
}

//spawn enemy at random position
void Game::spawnEnemy() {
    //* to get random number r u can use r = min + (rand()%(1+max-min))
    // enemmy must be spawned completely within the bounds of the window
    auto entity = m_entities.addEntity("enemy");
   
    //the entity shape will have shape radius, random sides, random fill , and white outline of thickness 2
    entity->add<CShape>(m_enemyConfig.SR,sRandnum(m_enemyConfig.VMIN,m_enemyConfig.VMAX), sf::Color(sRandnum(0,255),sRandnum(0,255),sRandnum(0,255)), sf::Color(std::uint8_t(m_enemyConfig.OR),std::uint8_t(m_enemyConfig.OG),std::uint8_t(m_enemyConfig.OB)), m_enemyConfig.OT);
    
    //get radius to set limits in pos 
    int radius = m_enemyConfig.SR;

    float randangle=(sRandnum(0,360)*(3.14159f/180));

    //give entity a transform so that it spawns at random pos with  random velocity (within the constraint) and angle 0
    m_enemyConfig.S = sRandnum(m_enemyConfig.SMIN,m_enemyConfig.SMAX);
    entity->add<CTransform>(Vec2f(sRandnum(0+radius,m_windowConfig.W-radius),sRandnum(0+radius,m_windowConfig.H-radius)), Vec2f(m_enemyConfig.S*std::cos(randangle),m_enemyConfig.S*std::sin(randangle)), 0);

    //add collision
    entity->add<CCollision>(m_enemyConfig.CR);

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
        entity->add<CShape>(m_enemyConfig.SR/2,parentCircle.getPointCount(),parentCircle.getFillColor(),parentCircle.getOutlineColor(),m_enemyConfig.OT/2);
        entity->add<CTransform>(parenrtTrans.pos,Vec2f(m_enemyConfig.S*std::cos((n)*anglefraction),m_enemyConfig.S*std::sin((n)*anglefraction)),0);
        entity->add<CCollision>((m_enemyConfig.SR/2)-1);
        //add lifespan
        entity->add<CLifespan>(m_enemyConfig.L);
    }
}

//spawn a bullet from a given entity to a target location 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target) {
    //- bullet speed is given as a scaler speed 
    //- set the correspondng velocity by calculating
    auto e = m_entities.addEntity("bullet");
    Vec2f direction = target-Vec2f(entity->get<CTransform>().pos);
    direction.normalize();
    Vec2f velocity= direction*m_bulletConfig.S;
    e->add<CTransform>(Vec2f(entity->get<CTransform>().pos), velocity, 0.0f);
    e->add<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(std::uint8_t(m_bulletConfig.FR), std::uint8_t(m_bulletConfig.FG), std::uint8_t(m_bulletConfig.FB)), sf::Color(std::uint8_t(m_bulletConfig.OR), std::uint8_t(m_bulletConfig.OG), std::uint8_t(m_bulletConfig.OB)), m_bulletConfig.OT);

    //add lifespan
    e->add<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e) {
    //TODO:implement a special weapon or ability
}

void Game::sMovement() {
    //- you should read the m_player->CInput component to determine if the player is  moving or not
    player()->get<CTransform>().velocity=Vec2f(player()->get<CInput>().right-player()->get<CInput>().left,player()->get<CInput>().down-player()->get<CInput>().up);
    player()->get<CTransform>().velocity.normalize();
    player()->get<CTransform>().velocity*=m_playerConfig.S; //scale it with speed to get velocity;

    for(auto &e : m_entities.getEntities()) {

      auto& transform = e->get<CTransform>();

        if(transform.exists) {
         transform.pos += transform.velocity;
        }
    }

    //prevent player moving out of window
    if(player()->get<CTransform>().pos.x<m_playerConfig.SR){
        player()->get<CTransform>().pos.x=m_playerConfig.SR;
    }

    if(player()->get<CTransform>().pos.x+m_playerConfig.SR>m_windowConfig.W) {
        player()->get<CTransform>().pos.x=m_windowConfig.W-m_playerConfig.SR;
    }

    if(player()->get<CTransform>().pos.y<m_playerConfig.SR){
        player()->get<CTransform>().pos.y=m_playerConfig.SR;
    }

    if(player()->get<CTransform>().pos.y+m_playerConfig.SR>m_windowConfig.H) {
        player()->get<CTransform>().pos.y=m_windowConfig.H-m_playerConfig.SR;
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
                sf::Color oc = e->get<CShape>().circle.getOutlineColor();
                e->get<CShape>().circle.setFillColor({c.r,c.g,c.b,uint8_t((255*lifespan.remaining)/lifespan.lifespan)});
                e->get<CShape>().circle.setOutlineColor({oc.r,oc.g,oc.b, uint8_t((255*lifespan.remaining)/lifespan.lifespan)});
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

        if(tran.pos.x<radius || tran.pos.x+radius>m_windowConfig.W){
            tran.velocity.x*=-1;
        }

        if(tran.pos.y<radius || tran.pos.y+radius>m_windowConfig.H){
            tran.velocity.y*=-1;
        }
    }

}

void Game::sEnemySpawner() {
    //call enemy spawner every time interval mentioned in enemyconfig file
    if(m_currentFrame>=m_lastEnenmySpawnTime+m_enemyConfig.SI) {
        spawnEnemy();
    }
}

void Game::sGUI() {

    ImGui::Begin("Geometry Wars");

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
            if (ImGui::BeginTabItem("Systems")) {
                    ImGui::Checkbox("Movement",&m_setMovement);
                    ImGui::Checkbox("Lifespan",&m_setLifespan);
                    ImGui::Checkbox("Collision",&m_setCollision);
                    ImGui::Checkbox("GUI (hotkey G)",&m_setGui);

                    ImGui::Checkbox("Spawning",&m_setSpawning);
                    ImGui::Indent();
                    if(ImGui::Button("Manual Spawn")) {
                        spawnEnemy();
                    }
                    ImGui::SliderInt("Spawn Interval",&m_enemyConfig.SI,10,100);
                    ImGui::Unindent();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Entities")) {

                    if (ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_None)) {
                        ImGui::Indent();

                        for(auto& [tag,entityVec] : m_entities.getEntityMap() ) {
                             if (ImGui::CollapsingHeader(tag.c_str(), ImGuiTreeNodeFlags_None)){
                                ImGui::Indent();
                                for(auto& e:entityVec ) {
                                    sf::Color c = e->get<CShape>().circle.getFillColor();
                                    ImGui::PushID(e->id());
                                    ImGui::PushStyleColor(ImGuiCol_Button,c);
                                    if(ImGui::Button("D"))
                                        e->destroy();
                                    ImGui::PopStyleColor();
                                    ImGui::PopID();
                                    ImGui::SameLine();
                                    ImGui::Text(" %d  %s (%.2f,%.2f)",e->id(),e->tag().c_str(),e->get<CTransform>().pos.x,e->get<CTransform>().pos.y);
                                }
                                ImGui::Unindent();
                            }
                        }
                        ImGui::Unindent();
                    }

                    if (ImGui::CollapsingHeader("All Entities", ImGuiTreeNodeFlags_None)) {
                        ImGui::Indent();

                            for(auto& e:m_entities.getEntities()) {
                                sf::Color c = e->get<CShape>().circle.getFillColor();
                                ImGui::PushID(e->id());
                                ImGui::PushStyleColor(ImGuiCol_Button,c);
                                if(ImGui::Button("D"))
                                    e->destroy();
                                ImGui::PopStyleColor();
                                ImGui::PopID();
                                ImGui::SameLine();
                                ImGui::Text(" %d  %s (%.2f,%.2f)",e->id(),e->tag().c_str(),e->get<CTransform>().pos.x,e->get<CTransform>().pos.y);
                                }
                        ImGui::Unindent();
                    }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
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
                
                case sf::Keyboard::Scancode::G:
                    std::cout<<"G key is pressed!\n";
                    m_setGui = true;
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