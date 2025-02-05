#ifndef MONSTER_MANAGER2_H
#define MONSTER_MANAGER2_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CZMLightManager
{
private:
	CZMLightManager(); // singleton creator
	friend inline CZMLightManager& LightManager()
	{
		static CZMLightManager x;
		return x;
	}

public:
	~CZMLightManager();

private:
	friend class CZMLight;
	void OnEntityAdd(CZMLight* ent);
	void OnEntityRemove(CZMLight* ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CZMLight*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CZMLightManager& LightManager();

#endif
