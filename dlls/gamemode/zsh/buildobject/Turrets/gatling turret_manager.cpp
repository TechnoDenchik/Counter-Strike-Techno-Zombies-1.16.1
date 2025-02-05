#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "gatling turret.h"
#include "gatling turret_manager.h"

#include <vector>
#include <algorithm>

class CZMGatlingManager::impl_t
{
public:
	std::vector<CZSHGatling*> m_vecEntities;
	size_t m_iMaxEntitiesNum;
	float m_flAutoGcRatio;
	void ResizeEntities(size_t new_size)
	{
		if (m_vecEntities.size() <= new_size)
			return;

		std::nth_element(m_vecEntities.begin(), m_vecEntities.begin() + new_size, m_vecEntities.end(), [](CZSHGatling*a, CZSHGatling*b) {return a->m_flTimeLastActive > b->m_flTimeLastActive; });
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

CZMGatlingManager::CZMGatlingManager() : pimpl(new impl_t())
{
	SetMaxNumOfEntity(256);
	SetAutoGcRatio(0.8f);
}

CZMGatlingManager::~CZMGatlingManager() // frees the pimpl
{

}

void CZMGatlingManager::OnEntityAdd(CZSHGatling*ent)
{
	pimpl->AutoResizeCheck();
	pimpl->m_vecEntities.push_back(ent);
}

void CZMGatlingManager::OnEntityRemove(CZSHGatling*ent)
{
	auto &v = pimpl->m_vecEntities;
	v.erase(std::find(v.begin(), v.end(), ent), v.end());
}

size_t CZMGatlingManager::EntityCount()
{
	return pimpl->m_vecEntities.size();
}

void CZMGatlingManager::EntityForEach(std::function<void(CZSHGatling*)> functor)
{
	auto &v = pimpl->m_vecEntities;
	std::for_each(v.begin(), v.end(), functor);
}

void CZMGatlingManager::SetMaxNumOfEntity(size_t what)
{
	pimpl->m_iMaxEntitiesNum = what;
}

size_t CZMGatlingManager::GetMaxNumOfEntity()
{
	return pimpl->m_iMaxEntitiesNum;
}

void CZMGatlingManager::SetAutoGcRatio(float what)
{
	assert(what >= 0.0f && what <= 1.0f);
	pimpl->m_flAutoGcRatio = what;
}