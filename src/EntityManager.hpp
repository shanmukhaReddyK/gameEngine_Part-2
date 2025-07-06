#pragma once

#include "Entity.hpp"
#include<map>
#include<algorithm>

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager {
    EntityVec                        m_entities;
    EntityVec                        m_entitiesToAdd;
    std::map<std::string, EntityVec> m_entityMap;
    size_t                           m_totalEntities=0;   

    void removeDeadEntities(EntityVec& vec){
        //remove all the dead entities
        //this called by update() function
        auto ne = std::remove_if(vec.begin(), vec.end(),[](std::shared_ptr<Entity> e) { return !e->isAlive();});
        vec.erase(ne,vec.end());
    }

    public:

    EntityManager() = default;

    void update()  {
        //add entities from m_entitesToAdd the proper location(s)
        // -add them to the vector of allentities
        // -add them to the vector inside the map, with tag as a key
        for(auto& e : m_entitiesToAdd ) {
            addEntity(e->m_tag);
        }

        //remove all the dead from the vector of allentities
        removeDeadEntities(m_entities);

        //remove dead entities from each vector in the enity map
        //C++20 way of iterating through [key,value] pairs in a map
        for(auto& [tag,entityVec] : m_entityMap ) {
            removeDeadEntities(entityVec);
        }
    }

    std::shared_ptr<Entity> addEntity(const std::string& tag) {

        //create the entity shared pointer
        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++,tag));

        //add it to vec of all entities
        m_entities.push_back(entity);

        //add it to entity map
        if(m_entityMap.find(tag)==m_entityMap.end())
            m_entityMap[tag]==EntityVec();
        
        m_entityMap[tag].push_back(entity);

        return entity;
    }

    const EntityVec& getEntities() {
        return m_entities;
    }

    const EntityVec& getEntities(const std::string& tag){
        if(m_entityMap.find(tag)==m_entityMap.end())
            m_entityMap[tag]=EntityVec();

        return m_entityMap[tag];
    }

    const std::map<std::string, EntityVec>& getEntityMap () {
        return m_entityMap;
    }
};