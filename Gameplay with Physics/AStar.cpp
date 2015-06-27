
#include "AStar.h"
#include "../../Physics/Physics.h"
#include "../../Framework/Entity.h"
#include "../../ModulePtrs.h"
#include "../../Graphics/header/commonFunctions.h"
#include "../Scripting/IMLLoader.h"


AStarManager* AStarManager::instance_;



void AStarManager::initializeNewPath()
{
	AStarInfo* currentPath = paths_.front();
	aStarInitialize(currentPath);
}

void AStarManager::updateCurrentPath(AStarInfo* currentPath)
{
	// Update the pathfinding the specified number of times
	// per frame, and break out if we finish early.
	for( unsigned int i=0; i<updatesPerFrame_; ++i )
	{
		if( aStarUpdate(currentPath) )
			break;
	}
	aStarCurrentBestPath(currentPath);
}

void AStarManager::removeFinishedPath()
{
	// Remove the first element in the list
	paths_.pop_front();
}

void AStarManager::Update()
{
	if( paths_.size() > 0 )
	{
		if( firstTime_ )
		{
			initializeNewPath();
			firstTime_ = false;
		}

        AStarInfo* currentPath = *(paths_.begin());	
		if( currentPath->finished )
		{
			removeFinishedPath();
			if( paths_.size() > 0 )
				initializeNewPath();
			else
				firstTime_ = true;
		}
		else
		{
			updateCurrentPath(currentPath);
		}
	}
}

void AStarManager::RequestPath(AStarInfo* data)
{
	paths_.push_back(data);
}

AStarManager* AStarManager::Instance()
{
	if( !instance )
	{
		instance_ = new AStarManager();
	}
	return instance_;
}

AStarManager::AStarManager() : updatesPerFrame_(50), firstTime_(true)
{
	
}

sortedListIter AStarManager::findNode(sortedList& list, AiWaypoint* node)
{
	sortedListIter itr	= list.begin();
	for( ; itr != list.end(); ++itr )
	{
		if( (*itr)->index == node->index )
		{
			return itr;
		}
	}
	return list.end();
}


void AStarManager::aStarInitialize(AStarInfo* data)
{	
	// Assert that we're given a non-zero data pointer
	assert(data);

	// Get the instance of the AiWorld
	AiWorld& iW = AiWorld::Instance();

	if( static_cast<unsigned>( data->startNode ) >= iW.WaypointCount() ||
		static_cast<unsigned>( data->endNode )   >= iW.WaypointCount() )
		return;

	// Clear all the weights
    iW.ClearWeights();
	iW[data->endNode].path		= true;
	iW[data->startNode].open	= true;

	// Have not found a path yet
	data->foundPath	= false;
	data->finished	= false;

	// Reset the path color
	for( unsigned int i=0; i < data->path.size(); ++i )
	{
		data->path[i]->closed	= false;
		data->path[i]->path		= false;
	}

	// Clear all the list in the info struct
	data->closedList.clear();
	data->openList.clear();
	data->path.clear();

	// Insert the start node into the openlist
	AiWaypoint* pWaypoint = &iW[data->startNode];
	data->openList.insert(pWaypoint);
}


bool AStarManager::aStarUpdate(AStarInfo* data)
{
	assert(data);

	// Get access to the AiWorld instance
	AiWorld& iW = AiWorld::Instance();

	// If the open list is empty then we are finished and there is no path
	if( !data->openList.size() )
	{
		data->foundPath	= false;
		data->finished	= true;
		return true;
	}

	// Get access to the best possible node from the open list
	AiWaypoint* pBestNode = *(data->openList.begin());

	// If the best node happens to be our endpoint, then we have found the path so create it!
	if( pBestNode->index == data->endNode )
	{
		// The first node's previous value is set to 0
		// so we just loop all the way back and push each node onto the 
		// path vector until the current node is 0
		while( pBestNode )
		{
			pBestNode->path = true;
			data->path.push_back(pBestNode);
			pBestNode = pBestNode->prev;
		}

		// We found the path
		data->foundPath	= true;
		data->finished	= true;

		// We're done
		return true;
	}


	
	// For every node that is adjacent to the BestNode, that is not in the closed list
	AiIndexIter i = pBestNode->neighbors.begin();
	for( ; i != pBestNode->neighbors.end(); ++i )
	{
		int index			= *i;
		AiWaypoint* pWay	= &iW[index];

		// If the neighbor is on the closed list then skip it
		if( pWay->closed )
			continue;

		// Check if the node is already in the open list
		sortedListIter found = findNode(data->openList, pWay);

		// Calculate the new score to this neighbor
		float dist		= GetDistanceBetweenIDF(pWay->location, pBestNode->location);
		float newScore	= pBestNode->G + dist;

		// If the neighbor was not found on the open list
		if( found == data->openList.end() )
		{
			// Set the G value
			pWay->G		= newScore;
			// Calculate the h value
			pWay->H		= GetDistanceBetweenIDF(pWay->location, iW[data->endNode].location );

			// Set the nodes previous pointer
			pWay->prev	= pBestNode;

			// Put the node into the openlist
			pWay->open	= true;
			data->openList.insert(pWay);
		}
		else
		{
			// If the open list's G score is greater then change it
			// and re-assign the last node
			if( (*found)->G > newScore )
			{
				(*found)->G = newScore;
				(*found)->prev = pBestNode;
			}
		}
	}

	// Move the best node to the closed list
	pBestNode->closed = true;
	pBestNode->open = false;
	pBestNode->path = false;
	data->closedList.insert(pBestNode);


	// Find the node to remove from the open list
	sortedListIter found = findNode(data->openList, pBestNode);
    // Remove the best node from the open list
	data->openList.erase(found);

	// Still not finished
	return false;
}

void AStarManager::aStarCurrentBestPath(AStarInfo* data)
{
	unsigned int size = data->path.size();
	if( size > 0 )
	{	
		for( unsigned int i=0; i<size; ++i )
		{	
			data->path[i]->path = false;
		}
		data->path.clear();
	}

	AiWaypoint* pNode;
	if( data->openList.size() > 0 )
	{
		pNode = *(data->openList.begin());
	}
	else if( data->closedList.size() > 0 )
	{
		 pNode = *(data->closedList.begin());
	}

	while( pNode )
	{
		pNode->path = true;
		data->path.push_back(pNode);
		pNode = pNode->prev;
	}

}
