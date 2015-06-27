#include "EntityManager.h"
#include "Entity.h"

//------------------------------------------------------------------------
//
//  Name:   EntityManager.cpp
//
//  Desc:   Singleton class to handle the  management of Entities.          
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
int EntityManager::m_iNextValidID = 0;

//--------------------------- Instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
EntityManager* EntityManager::Instance()
{
	static EntityManager instance;

	return &instance;
}

EntityManager::EntityMap & EntityManager::GetEntityMap()
{
	return m_EntityMap;
}

//------------------------- GetEntityFromID -----------------------------------
//-----------------------------------------------------------------------------
Entity* EntityManager::GetEntityFromID(int id)const
{
	//find the entity
	EntityMap::const_iterator ent = m_EntityMap.find(id);

	//assert that the entity is a member of the map
	//assert ( (ent !=  m_EntityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

	if ( !(ent != m_EntityMap.end()) )
		return NULL;

	return ent->second;
}

std::string EntityManager::GetStringfromID(int id)const
{
	Entity* ent = GetEntityFromID(id);
	return ent->entity_name;
}

//Erase will call the destructor for the pointer, but it will do nothing. 

/*****************************************************************************/
//! Delete a game entity and its components.
/*****************************************************************************/
void EntityManager::DestroyEntity(Entity* entity)
{
	//Calls entity destructor
	delete entity;

}
/*****************************************************************************/
//! Empty the entity list on deletion of game manager.
/*****************************************************************************/
void EntityManager::EmptyEntityList(void)
{
	if( m_EntityMap.empty() )
		return;
	EntityMap::iterator iter = m_EntityMap.begin();
	// Note: Deletion of components taken care of by engines.


	while( iter != m_EntityMap.end() )
	{
		
		DestroyEntity(iter->second);
	}

	m_EntityMap.clear();
}

//Removes deleted entities from the entity map every frame
void EntityManager::EntityCleanup()
{
	if( m_EntityMap.empty() )
		return;
	//EntityMap is a typedef std::map<int, Entity*> EntityMap;
	EntityMap::iterator iter = m_EntityMap.begin();

	while( iter != m_EntityMap.end() ) 
	{
		//Check if it's marked to be deleted
		if( iter->second->isThisMarked() )
		{
			EntityMap::iterator iter2 = iter;

			//Removes children from parent that will be deleted. They are marked for deletion
			//In the markfordeletion function, then they each go through this process as parents. 
			//Therefore, this effectively works :D
			if( iter->second->parent )
				iter->second->parent->children.remove(iter->second);
			++iter;

			Entity *toDel = iter2->second;
			m_EntityMap.erase(iter2);
			DestroyEntity(toDel);
			
		}
		else
			++iter;
	}
}


//---------------------------- RegisterEntity ---------------------------------
//-----------------------------------------------------------------------------
void EntityManager::RegisterEntity(Entity* NewEntity)
{
	m_EntityMap.insert(std::make_pair(NewEntity->ID(), NewEntity));
}

void EntityManager::Update()
{
	if( m_EntityMap.empty() )
		return;

	for( EntityMap::iterator iter = m_EntityMap.begin(); iter != m_EntityMap.end(); ++iter)
		iter->second->Update();

}
