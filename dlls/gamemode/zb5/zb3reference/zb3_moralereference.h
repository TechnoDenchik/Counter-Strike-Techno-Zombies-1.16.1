/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

enum ZB5HumanMoraleType_e : byte;

class CZB3HumanMoraleR
{
public:
	constexpr CZB3HumanMoraleR() : m_iHumanMoraleLevel(0) {}

	constexpr float DamageModifier(ZB5HumanMoraleType_e type) const 
	{
		return type == ZB5_MORALE_STRENGTHEN ?
		       1.3f + m_iHumanMoraleLevel * 0.1f :
			1.0f + m_iHumanMoraleLevel * 0.1f;
	}

	constexpr int GetMoraleLevel() const { return m_iHumanMoraleLevel; }

	bool LevelUp() 
	{
		if (m_iHumanMoraleLevel < 10)
		{
			m_iHumanMoraleLevel += 2;
			return true;
		}
		return false;
	}
	bool LevelUp2()
	{
		if (m_iHumanMoraleLevel < 10)
		{
			m_iHumanMoraleLevel += 3;
			return true;
		}
		return false;
	}

	bool LevelDown()
	{
		if (m_iHumanMoraleLevel > 2)
		{
			m_iHumanMoraleLevel -= 3;
			return true;
		}
		return false;
	}
	
	void UpdateHUD(CBasePlayer * pPlayer) const;
	int m_iHumanMoraleLevel;
};