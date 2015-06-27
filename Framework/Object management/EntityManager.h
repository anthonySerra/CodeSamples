#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   EntityManager.h
//
//  Desc:   Singleton class to handle the  management of Entities.          
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------

#include <map>
#include <cassert>
#include <string>
#include "Entity.h"


class Entity;

//provide easy access
#define EntityMgr EntityManager::Instance()


class EntityManager
{
public:

  typedef std::map<int, Entity*> EntityMap;

private:

  //to facilitate quick lookup the entities are stored in a std::map, in which
  //pointers to entities are cross referenced by their identifying number
  EntityMap m_EntityMap;

  EntityManager(){}

  //copy ctor and assignment should be private
  EntityManager(const EntityManager&);
  EntityManager& operator=(const EntityManager&);

public:

  static EntityManager* Instance();

    //this is the next valid ID. Each time a Entity is instantiated
  //this value is updated
  static int  m_iNextValidID;

  //this method stores a pointer to the entity in the std::vector
  //m_Entities at the index position indicated by the entity's ID
  //(makes for faster access)
  void RegisterEntity(Entity* NewEntity);

  //returns a pointer to the entity with the ID given as a parameter
  Entity* GetEntityFromID(int id)const;

  //Retrieve Entity type from ID
  std::string GetStringfromID(int id)const;

  //this method removes the entity from the map and deallocates the components
  void DestroyEntity(Entity* entity);
  void EmptyEntityList(void);
  void EntityCleanup();


  //Get the entity map
  EntityMap & GetEntityMap();

  void Update();
};







#endif