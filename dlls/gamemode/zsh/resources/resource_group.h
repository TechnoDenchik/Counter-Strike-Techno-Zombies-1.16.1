#pragma once

#include <memory>

class CHostageImprov;
class CResGroup;
class CResGroupMember;

class CResGroup : public std::enable_shared_from_this<CResGroup>
{
	friend class CResGroupMember;

public:
	CResGroup(const Vector &origin);
	~CResGroup();
	std::shared_ptr<CResGroupMember> Join();
	bool IsCurrentAt(const Vector &vecWhere) const;
	//void Merge(CNpcGroup &&other);

private:
	struct impl_t;
	const std::unique_ptr<impl_t> pimpl;
};

class CResGroupMember
{
public:
	CResGroupMember(const std::shared_ptr<CResGroup> &g) : m_spGroup(g) {}
	const std::shared_ptr<const CResGroup> &GetGroup() const { return m_spGroup; }
	bool Apply(CHostageImprov* improv);

private:
	std::shared_ptr<const CResGroup> m_spGroup;
};

std::shared_ptr<CResGroupMember> GetGroup(const Vector &start);