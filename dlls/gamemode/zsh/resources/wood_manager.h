#ifndef WOOD_MANAGER_H
#define WOOD_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CWoodManager
{
private:
	CWoodManager(); // singleton creator
	friend inline CWoodManager& WoodManager()
	{
		static CWoodManager x;
		return x;
	}
	
public:
	~CWoodManager();

private:
	friend class CWood;
	void OnEntityAdd(CWood*ent);
	void OnEntityRemove(CWood*ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CWood*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CWoodManager &WoodManager();

#endif