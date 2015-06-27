#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
//------------------------------------------------------------------------
//
//  Name:   MessageDispatcher.h
//
//  Desc:   A message dispatcher. Manages messages of the type Telegram.
//          Instantiated as a singleton.
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
#pragma warning (disable:4786)

#include <set>
#include <queue>
#include <list>

#include "Telegram.h"
#include "MessageTypes.h"

class Entity;


//to make code easier to read
const double SEND_MSG_IMMEDIATELY = 0.0f;
const int   NO_ADDITIONAL_INFO   = 0;

//to make life easier...
#define Dispatch MessageDispatcher::Instance()


class MessageComponent;

class MessageDispatcher
{
  
private:  
  
  //a std::set is used as the container for the delayed messages
  //because of the benefit of automatic sorting and avoidance
  //of duplicates. Messages are sorted by their dispatch time.
  std::set<Telegram> PriorityQ;

  //this method is utilized by DispatchMessage or DispatchDelayedMessages.
  //This method calls the message handling member function of the receiving
  //entity, pReceiver, with the newly created telegram
  void Discharge(Entity* pReceiver, const Telegram& msg);

  void collectTheMail();
  void distributeTheMail();
  std::list<MessageComponent*> subscribers;
  std::queue<GameMessage*> messages;

  MessageDispatcher(){}

  //copy ctor and assignment should be private
  MessageDispatcher(const MessageDispatcher&);
  MessageDispatcher& operator=(const MessageDispatcher&);

public:

  //this class is a singleton
  static MessageDispatcher* Instance();

  //send a message to another agent. Receiving agent is referenced by ID.
  void DispatchMsg(double  delay,
                       int    sender,
                       int    receiver,
                       int    msg,
                       void*  ExtraInfo);

  //send out any delayed messages. This method is called each time through   
  //the main game loop.
  void DispatchDelayedMessages();
  
  //broadcast functionality
  void handleTheMail();
  void registerMailbox(MessageComponent*);
  void unregisterMailbox(MessageComponent*);

  

 



};

class MessageComponent
{
	friend class MessageDispatcher;
public:
	MessageComponent()  {  MessageDispatcher::Instance()->registerMailbox(this); }
	~MessageComponent() {  MessageDispatcher::Instance()->unregisterMailbox(this); }

	void Broadcast(GameMessage* message);
	GameMessage* getNextMessageFromInbox();
	void subscribe(message_type type);
	void unsubscribe(message_type type);
	bool subscribesToType(message_type type);

private:
	
	std::queue<GameMessage*> inbox;
	std::queue<GameMessage*> outbox;
	std::list<message_type> subscriptions;
};




#endif