#ifndef MONSTER_MANAGER2_H
#define MONSTER_MANAGER2_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CBossManager
{
private:
	CBossManager(); // singleton creator
	friend inline CBossManager& BossManager()
	{
		static CBossManager x;
		return x;
	}

public:
	~CBossManager();

private:
	friend class CMonsterBoss;
	void OnEntityAdd(CMonsterBoss* ent);
	void OnEntityRemove(CMonsterBoss* ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CMonsterBoss*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CBossManager& BossManager();

#endif
