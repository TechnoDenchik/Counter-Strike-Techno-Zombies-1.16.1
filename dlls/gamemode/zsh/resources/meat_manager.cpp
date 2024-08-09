#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "meat.h"

#include "meat_manager.h"

#include <vector>
#include <algorithm>

class CMeatManager::impl_t
{
public:
	std::vector<CMeat*> m_vecEntities;
	size_t m_iMaxEntitiesNum;
	float m_flAutoGcRatio;
	void ResizeEntities(size_t new_size)
	{
		if (m_vecEntities.size() <= new_size)
			return;

		std::nth_element(m_vecEntities.begin(), m_vecEntities.begin() + new_size, m_vecEntities.end(), [](CMeat* a, CMeat* b) {return a->m_flTimeLastActive > b->m_flTimeLastActive; });
		std::for_each(m_vecEntities.begin() + new_size, m_vecEntities.end(), std::bind(&CBaseEntity::Killed, std::placeholders::_1, nullptr, GIB_NORMAL));
	}

	void AutoResizeCheck()
	{
		if (m_vecEntities.size() >= m_iMaxEntitiesNum)
		{
			// kills a quarter inactive zombies
			ResizeEntities(m_iMaxEntitiesNum * m_flAutoGcRatio);
		}
	}
};

CMeatManager::CMeatManager() : pimpl(new impl_t())
{
	SetMaxNumOfEntity(256);
	SetAutoGcRatio(0.8f);
}

CMeatManager::~CMeatManager() // frees the pimpl
{

}

void CMeatManager::OnEntityAdd(CMeat* ent)
{
	pimpl->AutoResizeCheck();
	pimpl->m_vecEntities.push_back(ent);
}

void CMeatManager::OnEntityRemove(CMeat* ent)
{
	auto& v = pimpl->m_vecEntities;
	v.erase(std::find(v.begin(), v.end(), ent), v.end());
}

size_t CMeatManager::EntityCount()
{
	return pimpl->m_vecEntities.size();
}

void CMeatManager::EntityForEach(std::function<void(CMeat*)> functor)
{
	auto& v = pimpl->m_vecEntities;
	std::for_each(v.begin(), v.end(), functor);
}

void CMeatManager::SetMaxNumOfEntity(size_t what)
{
	pimpl->m_iMaxEntitiesNum = what;
}

size_t CMeatManager::GetMaxNumOfEntity()
{
	return pimpl->m_iMaxEntitiesNum;
}

void CMeatManager::SetAutoGcRatio(float what)
{
	assert(what >= 0.0f && what <= 1.0f);
	pimpl->m_flAutoGcRatio = what;
}