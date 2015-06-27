#pragma once

#include "MessageDispatcher.h"
#include "Entity.h"
#include "CrudeTimer.h"
#include "EntityManager.h"
#include "MessageTypes.h"

//------------------------------------------------------------------------
//
//  Name:   MessageDispatcher.cpp
//
//  Desc:   A message dispatcher. Manages messages of the type Telegram.
//          Instantiated as a singleton.
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------

#include <iostream>
using std::cout;

using std::set;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif



//------------------------------ Instance -------------------------------------

MessageDispatcher* MessageDispatcher::Instance()
{
	static MessageDispatcher instance;

	return &instance;
}


//----------------------------- Dispatch ---------------------------------
//  
//  see description in header
//------------------------------------------------------------------------
void MessageDispatcher::Discharge(Entity* pReceiver,
								  const Telegram& telegram)
{
	if (!pReceiver->HandleMessage(telegram)) 
	{
//		telegram could not be handled

//		Degugging stuff here
		cout << "Message not handled";
	}
}

//---------------------------- DispatchMsg ---------------------------
//
//  given a message, a receiver, a sender and any time delay , this function
//  routes the message to the correct agent (if no delay) or stores
//  in the message queue to be dispatched at the correct time
//------------------------------------------------------------------------
void MessageDispatcher::DispatchMsg(double  delay,
									int    sender,
									int    receiver,
									int    msg,
									void*  ExtraInfo)
{
	//  SetTextColor(BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

	//get pointers to the sender and receiver
	Entity* pSender   = EntityMgr->GetEntityFromID(sender);
	Entity* pReceiver = EntityMgr->GetEntityFromID(receiver);

	//make sure the receiver is valid
	if (pReceiver == NULL)
	{
		cout << "\nWarning! No Receiver with ID of " << receiver << " found";

		return;
	}

	//create the telegram
	Telegram telegram(0, sender, receiver, msg, ExtraInfo);

	//if there is no delay, route telegram immediately                       
	if (delay <= 0.0f)                                                        
	{
		/*
		cout << "\nInstant telegram dispatched at time: " << Clock->GetCurrentTime()
		<< " by " << GetNameOfEntity(pSender->ID()) << " for " << GetNameOfEntity(pReceiver->ID()) 
		<< ". Msg is "<< MsgToStr(msg);
		*/

		//send the telegram to the recipient
		Discharge(pReceiver, telegram);
	}

	//else calculate the time when the telegram should be dispatched
	else
	{
		double CurrentTime = Clock->GetTime(); 

		telegram.DispatchTime = CurrentTime + delay;

		//and put it in the queue
		PriorityQ.insert(telegram);   

		/*
		cout << "\nDelayed telegram from " << GetNameOfEntity(pSender->ID()) << " recorded at time " 
		<< Clock->GetCurrentTime() << " for " << GetNameOfEntity(pReceiver->ID())
		<< ". Msg is "<< MsgToStr(msg);
		*/

	}
}


//---------------------- DispatchDelayedMessages -------------------------
//
//  This function dispatches any telegrams with a timestamp that has
//  expired. Any dispatched telegrams are removed from the queue
//------------------------------------------------------------------------
void MessageDispatcher::DispatchDelayedMessages()
{
	//SetTextColor(BACKGROUND_RED|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

	//get current time
	double CurrentTime = Clock->GetTime();

	//now peek at the queue to see if any telegrams need dispatching.
	//remove all telegrams from the front of the queue that have gone
	//past their sell by date
	while( !PriorityQ.empty() &&
		(PriorityQ.begin()->DispatchTime < CurrentTime) && 
		(PriorityQ.begin()->DispatchTime > 0) )
	{
		//read the telegram from the front of the queue
		const Telegram& telegram = *PriorityQ.begin();

		//find the recipient
		Entity* pReceiver = EntityMgr->GetEntityFromID(telegram.Receiver);

		/*
		cout << "\nQueued telegram ready for dispatch: Sent to " 
		<< GetNameOfEntity(pReceiver->ID()) << ". Msg is " << MsgToStr(telegram.Msg);
		*/

		//send the telegram to the recipient
		Discharge(pReceiver, telegram);

		//remove it from the queue
		PriorityQ.erase(PriorityQ.begin());
	}
}


//broadcast functionality
void MessageDispatcher::handleTheMail()
{
	collectTheMail();
	distributeTheMail();

}
//collect the mail
void MessageDispatcher::collectTheMail()
{
	//go through all the subscribers and get the mail out of their outboxes.
	for(std::list<MessageComponent*>::iterator sub = subscribers.begin(); sub != subscribers.end(); sub++)
	{
		while(!(*sub)->outbox.empty())
		{
			messages.push((*sub)->outbox.front());
			(*sub)->outbox.pop();
		}
	}

}

//distribute the mail
void MessageDispatcher::distributeTheMail()
{
	while(!messages.empty())
	{
		GameMessage* message = messages.front();
		messages.pop();

		for(std::list<MessageComponent*>::iterator sub = subscribers.begin(); sub != subscribers.end(); sub++)
		{
			if((*sub)->subscribesToType(message->type))
			{
				GameMessage* msg_copy = (GameMessage*)new char[message->message_size];
				memcpy(msg_copy,message,message->message_size);
				(*sub)->inbox.push(msg_copy);
			}
		}
		delete message;
		message = NULL;
	}

}

void MessageComponent::subscribe(message_type type)
{
	subscriptions.push_back(type);
}
void MessageComponent::unsubscribe(message_type type)
{
	subscriptions.remove(type);
}
bool MessageComponent::subscribesToType(message_type type)
{
	for(std::list<message_type>::iterator t = subscriptions.begin(); t != subscriptions.end(); t++)
		if((*t) == type)
			return true;
	return false;
}

void MessageComponent::Broadcast(GameMessage* message)
{
	outbox.push(message);
}
GameMessage* MessageComponent::getNextMessageFromInbox()
{
	if(inbox.empty())
		return 0;
	
	GameMessage* msg = inbox.front();
	inbox.pop();
	return msg;
}

void MessageDispatcher::registerMailbox(MessageComponent* mc)
{
	subscribers.remove(mc);
	subscribers.push_back(mc);
}

void MessageDispatcher::unregisterMailbox(MessageComponent* mc)
{
	subscribers.remove(mc);
}
