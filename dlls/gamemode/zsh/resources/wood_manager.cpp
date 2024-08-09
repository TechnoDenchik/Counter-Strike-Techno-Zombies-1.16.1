#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "wood.h"

#include "wood_manager.h"

#include <vector>
#include <algorithm>

class CWoodManager::impl_t
{
public:
	std::vector<CWood*> m_vecEntities;
	size_t m_iMaxEntitiesNum;
	float m_flAutoGcRatio;
	void ResizeEntities(size_t new_size)
	{
		if (m_vecEntities.size() <= new_size)
			return;

		std::nth_element(m_vecEntities.begin(), m_vecEntities.begin() + new_size, m_vecEntities.end(), [](CWood*a, CWood*b) {return a->m_flTimeLastActive > b->m_flTimeLastActive; });
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

CWoodManager::CWoodManager() : pimpl(new impl_t())
{
	SetMaxNumOfEntity(256);
	SetAutoGcRatio(0.8f);
}

CWoodManager::~CWoodManager() // frees the pimpl
{

}

void CWoodManager::OnEntityAdd(CWood*ent)
{
	pimpl->AutoResizeCheck();
	pimpl->m_vecEntities.push_back(ent);
}

void CWoodManager::OnEntityRemove(CWood*ent)
{
	auto &v = pimpl->m_vecEntities;
	v.erase(std::find(v.begin(), v.end(), ent), v.end());
}

size_t CWoodManager::EntityCount()
{
	return pimpl->m_vecEntities.size();
}

void CWoodManager::EntityForEach(std::function<void(CWood*)> functor)
{
	auto &v = pimpl->m_vecEntities;
	std::for_each(v.begin(), v.end(), functor);
}

void CWoodManager::SetMaxNumOfEntity(size_t what)
{
	pimpl->m_iMaxEntitiesNum = what;
}

size_t CWoodManager::GetMaxNumOfEntity()
{
	return pimpl->m_iMaxEntitiesNum;
}

void CWoodManager::SetAutoGcRatio(float what)
{
	assert(what >= 0.0f && what <= 1.0f);
	pimpl->m_flAutoGcRatio = what;
}