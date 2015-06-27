#include "Entity.h"
#include "GameManager.h"
#include <cassert>


//------------------------------------------------------------------------
//
//  Name:   Entity.cpp
//
//  Desc:   Base class for a game object
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------

Entity::Entity()
{
	SetID(EntityMgr->m_iNextValidID);
	parent = 0;
	deletionFlag = 0;
}

//----------------------------- SetID -----------------------------------------
//
//  this must be called within each constructor to make sure the ID is set
//  correctly. It verifies that the value passed to the method is greater
//  or equal to the next valid ID, before setting the ID and incrementing
//  the next valid ID
//-----------------------------------------------------------------------------
void Entity::SetID(int val)
{
	//make sure the val is equal to or greater than the next available ID
	assert ( (val >= EntityMgr->m_iNextValidID) && "<Entity::SetID>: invalid ID");

	m_ID = val;

	EntityMgr->m_iNextValidID = m_ID + 1;

}

Entity::~Entity()
{
	//Remove all components if they exist, and mark them to be deleted by their respective engines
	this->MassacreAllComponents();


	//Remove from container : )
	EntityMgr->GetEntityMap().erase(this->ID());
}

void Entity::addComponent(BaseComponent * component)
{
	component->owner = this;
	ComponentList.push_back(component);
}

void Entity::RemoveComponent(BaseComponent * component)
{
	component->owner = 0;
	ComponentList.remove(component);
}

void Entity::MassacreAllComponents()
{
	std::list<BaseComponent*>::iterator i = ComponentList.begin();

	while(i != ComponentList.end())
	{
		std::list<BaseComponent*>::iterator i2 = i;
		++i;
		(*i2)->owner = 0;
		ComponentList.erase(i2);
	}
}

void Entity::MarkForDeletion()
{
	deletionFlag = true;
	//children of an entity is a list, not a map ;o
	std::list<Entity *>::iterator iter = children.begin();


	if( this->parent && this->parent->isThisMarked() )
	{
		this->parent = NULL;
	}

	while( iter != children.end() )
	{
		(*iter)->MarkForDeletion();      
		++iter;
	}
}

bool Entity::isThisMarked()
{
	return deletionFlag;
}

