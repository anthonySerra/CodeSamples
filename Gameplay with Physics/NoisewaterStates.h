#ifndef Noisewater_STATES_HPP
#define Noisewater_STATES_HPP

#include "../Statemachine.h"

//------------------------------------------------------------------------
//
//  Name:   NoisewaterStates.hpp
//
//  Desc:   Interface for the states of the Noisewater.
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
class Noisewater;

namespace noisewater
{

	class Global : public State<Noisewater>
	{
	private:

		Global(){}

		Global(const Global&);
		Global& operator=(const Global&);
	public:
		static Global* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};

	class Init : public State<Noisewater>
{
private:

    Init(){}

    Init(const Init&);
    Init& operator=(const Init&);
public:
    static Init* Instance();
    virtual void Enter(Noisewater* instance);
    virtual void Execute(Noisewater* instance);
    virtual void Exit(Noisewater* instance);
	virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
};



	class Ground : public State<Noisewater>
	{
	private:

		Ground(){}

		Ground(const Ground&);
		Ground& operator=(const Ground&);
	public:
		static Ground* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};

	class Air : public State<Noisewater>
	{
	private:

		Air(){}

		Air(const Air&);
		Air& operator=(const Air&);
	public:
		static Air* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};

	class Damaged : public State<Noisewater>
	{
	private:

		Damaged(){}

		Damaged(const Damaged&);
		Damaged& operator=(const Damaged&);
	public:
		static Damaged* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};

	class ReadyParry : public State<Noisewater>
	{
	private:

		ReadyParry(){}

		ReadyParry(const ReadyParry&);
		ReadyParry& operator=(const ReadyParry&);
	public:
		static ReadyParry* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};

	class Parry : public State<Noisewater>
	{
	private:

		Parry(){}

		Parry(const Parry&);
		Parry& operator=(const Parry&);
	public:
		static Parry* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};


	class Faint : public State<Noisewater>
	{
	private:

		Faint(){}

		Faint(const Faint&);
		Faint& operator=(const Faint&);
	public:
		static Faint* Instance();
		virtual void Enter(Noisewater* instance);
		virtual void Execute(Noisewater* instance);
		virtual void Exit(Noisewater* instance);
		virtual bool OnMessage(Noisewater* instance, const Telegram& msg);
	};
}

#endif
