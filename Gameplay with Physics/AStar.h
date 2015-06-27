//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//
//	This pathfinding system was written in less than two days by myself
//	in order to meet requirements for our junior game project. 
//
//	The system works well and allows searching of arbitratily placed and connected
//	nodes in 3D. This file was paired with a node building system that created
//	the node graph for the AI units dynamically.
//
//------------------------------------------------------------------------

#pragma once

#include <vector>
#include <set>
#include <list>
#include "WorldData.h"

class Entity;

struct lessThan
{
	bool operator() (const AiWaypoint* x, const AiWaypoint* y) const
	{
		return (x->G + x->H) < (y->G + y->H);
	}
};

typedef std::set<AiWaypoint*, lessThan>	sortedList;
typedef sortedList::iterator			sortedListIter;


// Information structure used to keep track of an incremental AStar search 
struct AStarInfo
{

	bool					foundPath;
	bool					finished;

	int						startNode;
	int						endNode;

	sortedList				openList;
	sortedList				closedList;
	AiWaypointVector		path;
};



class AStarManager
{
public:
	static AStarManager*	Instance();

public:
	void					Update();
	void					RequestPath(AStarInfo* data);

private:
							AStarManager();

	void					initializeNewPath();
	void					updateCurrentPath(AStarInfo* currentPath);
	void					removeFinishedPath();

	void					aStarInitialize(AStarInfo* data);
	bool					aStarUpdate(AStarInfo* data);
	void					aStarCurrentBestPath(AStarInfo* data);
	sortedListIter			findNode(sortedList& list, AiWaypoint* node);

private:
	static AStarManager*	instance_;

	std::list<AStarInfo*>	paths_;
	unsigned int			updatesPerFrame_;
	bool					firstTime_;
};


void AStarLuaRegister(void);

