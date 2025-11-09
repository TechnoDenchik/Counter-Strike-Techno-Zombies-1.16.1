#ifndef MONSTER_MANAGER_H
#define MONSTER_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CZMGatlingManager
{
private:
	CZMGatlingManager(); // singleton creator
	friend inline CZMGatlingManager& GatlingManager()
	{
		static CZMGatlingManager x;
		return x;
	}
	
public:
	~CZMGatlingManager();

private:
	friend class CZSHGatling;
	void OnEntityAdd(CZSHGatling*ent);
	void OnEntityRemove(CZSHGatling*ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CZSHGatling*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CZMGatlingManager&GatlingManager();

#endif