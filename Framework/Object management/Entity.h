#ifndef ENTITY_H
#define ENTITY_H
//------------------------------------------------------------------------
//
//  Name:   Entity.h
//
//  Desc:   Base class for a game object
//
//  Author: Anthony Serra
//------------------------------------------------------------------------
#include <string>

#include "Telegram.h"
#include "EntityManager.h"

#include <list>
#include <vector>

class BaseComponent;


//Note: All engines will need to check the base component's owner pointer to 
//      see if said derived component needs to be de-allocated
class Entity
{
private:

	//every entity must have a unique identifying number
	int          m_ID;


	//this must be called within the constructor to make sure the ID is set
	//correctly. It verifies that the value passed to the method is greater
	//or equal to the next valid ID, before setting the ID and incrementing
	//the next valid ID
	void SetID(int val);

	//leave this alone jerks =3
	bool deletionFlag;

	std::list<BaseComponent*> ComponentList;

public:

	Entity();

	virtual ~Entity();

	//all entities must implement an update function
	virtual void  Update()=0;

	//all entities can communicate using messages. They are sent
	//using the MessageDispatcher class
	virtual bool  HandleMessage(const Telegram& msg)=0;

	int           ID()const{return m_ID;}  

	void MarkForDeletion();
	bool isThisMarked();

	void addComponent(BaseComponent * component);
	void RemoveComponent(BaseComponent * component);
	void MassacreAllComponents();

	

	template <typename T>
	void getComponents(std::vector<T*> & newList)
	{
		for(std::list<BaseComponent*>::iterator i = ComponentList.begin(); i != ComponentList.end(); i++)
		{
			if( typeid(T) == typeid(*(*i)) )
				newList.push_back((T*)(*i));
		}
	}

	// Used for Deleting Levels and entity's created in response to a parent
	std::list<Entity *> children;    

	// Stores a pointer to its parent, Used for clearing itself out of parent's children list on delete
	Entity *parent;          

	std::string entity_name;
	
};


class BaseComponent
{
public:
	BaseComponent(){}
	virtual ~BaseComponent(){}

	template <typename T>
	void getOtherComponents(std::vector<T*> & newList)
	{
		owner->getComponents(newList);
	}

	Entity* owner;
};


#endif


