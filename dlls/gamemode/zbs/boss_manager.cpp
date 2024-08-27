#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "monster_entity_boss.h"

#include "boss_manager.h"

#include <vector>
#include <algorithm>

class CBossManager::impl_t
{
public:
	std::vector<CMonsterBoss*> m_vecEntities;
	size_t m_iMaxEntitiesNum;
	float m_flAutoGcRatio;
	void ResizeEntities(size_t new_size)
	{
		if (m_vecEntities.size() <= new_size)
			return;

		std::nth_element(m_vecEntities.begin(), m_vecEntities.begin() + new_size, m_vecEntities.end(), [](CMonsterBoss* a, CMonsterBoss* b) {return a->m_flTimeLastActive > b->m_flTimeLastActive; });
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

CBossManager::CBossManager() : pimpl(new impl_t())
{
	SetMaxNumOfEntity(256);
	SetAutoGcRatio(0.8f);
}

CBossManager::~CBossManager() // frees the pimpl
{

}

void CBossManager::OnEntityAdd(CMonsterBoss* ent)
{
	pimpl->AutoResizeCheck();
	pimpl->m_vecEntities.push_back(ent);
}

void CBossManager::OnEntityRemove(CMonsterBoss* ent)
{
	auto& v = pimpl->m_vecEntities;
	v.erase(std::find(v.begin(), v.end(), ent), v.end());
}

size_t CBossManager::EntityCount()
{
	return pimpl->m_vecEntities.size();
}

void CBossManager::EntityForEach(std::function<void(CMonsterBoss*)> functor)
{
	auto& v = pimpl->m_vecEntities;
	std::for_each(v.begin(), v.end(), functor);
}

void CBossManager::SetMaxNumOfEntity(size_t what)
{
	pimpl->m_iMaxEntitiesNum = what;
}

size_t CBossManager::GetMaxNumOfEntity()
{
	return pimpl->m_iMaxEntitiesNum;
}

void CBossManager::SetAutoGcRatio(float what)
{
	assert(what >= 0.0f && what <= 1.0f);
	pimpl->m_flAutoGcRatio = what;
}