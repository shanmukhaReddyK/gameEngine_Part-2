#pragma once

#include"Components.hpp"
#include<string>
#include<tuple>

class EntityManager;

using ComponentTyple = std::tuple<CTransform,CShape,CCollision,CScore,CLifespan,CInput>;

class Entity {
  friend class EntityManager;
  
  ComponentTyple    m_components;
  bool              m_alive=true;
  std::string       m_tag="default";
  size_t            m_id=0;

  Entity(const size_t& id, const std::string& tag) :  m_id(id), m_tag(tag) {}

  public:

  bool isAlive() const {
    return m_alive;
  }

  void destroy() {
    m_alive=false;
  }

  size_t id() const {
    return m_id;
  }

  const std::string& tag() const {
    return m_tag;
  }

  template <typename T>
  bool has() const {
    return get<T>().exists;
  }

  template <typename T, typename... TArgs>
  T& add(TArgs&&...mArgs) {
    auto& component = get<T>();
    component = T(std::forward<TArgs>(mArgs)...);
    component.exists=true;
    return component;
  }

  template <typename T>
  T& get() {
    return std::get<T>(m_components);
  }

  template <typename T>
  const T& get() const {
    return std::get<T>(m_components);
  }

  template <typename T>
  void remove() {
    get<T>() = T();
  }
  
};