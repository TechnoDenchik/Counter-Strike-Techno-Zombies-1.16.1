#ifndef MEAT_MANAGER_H
#define MEAT_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include <memory>
#include <functional>

class CMeatManager
{
private:
	CMeatManager(); // singleton creator
	friend inline CMeatManager& MeatManager()
	{
		static CMeatManager x;
		return x;
	}

public:
	~CMeatManager();

private:
	friend class CMeat;
	void OnEntityAdd(CMeat* ent);
	void OnEntityRemove(CMeat* ent);

public:
	size_t EntityCount();
	void EntityForEach(std::function<void(CMeat*)> functor);

public: // max monster num settings
	void SetMaxNumOfEntity(size_t what);
	size_t GetMaxNumOfEntity();
	void SetAutoGcRatio(float what);

private:
	class impl_t;
	std::unique_ptr<impl_t> pimpl;

};

inline CMeatManager& MeatManager();

#endif
