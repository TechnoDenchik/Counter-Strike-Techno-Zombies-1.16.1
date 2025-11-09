#ifndef MONSTER_MANAGER_H
#define MONSTER_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CZMTankManager
{
private:
	CZMTankManager(); // singleton creator
	friend inline CZMTankManager& TankManager()
	{
		static CZMTankManager x;
		return x;
	}
	
public:
	~CZMTankManager();

private:
	friend class CZMTank;
	void OnEntityAdd(CZMTank*ent);
	void OnEntityRemove(CZMTank*ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CZMTank*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CZMTankManager&TankManager();

#endif