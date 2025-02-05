#ifndef ZSH_CONST_H
#define ZSH_CONST_H
#ifdef _WIN32
#pragma once
#endif

enum ZSHMessageTypes : byte
{
	ZSHScoreboard,
	ZSHWeaponboard,
	ZSHMessagebox,
	ZSHMessageTime
};

enum ZSHMessageBuild : byte
{
	ZSHTurret,
	ZSHBase,
	ZSHBase2,
	ZSHBase3,
	ZSHBase4,
	ZSHPost,
	ZSHDodgers,
	ZSHFences,
	ZSHGate,
	ZSHStorage,
	ZSHGenerator,
	ZSHRecovery,
	ZSHTechnical

};

enum ZSHMessageSkills : byte
{
	ZSHWarrior,
	ZSHSurvival,
	ZSHEngineer
};

enum ZSHMessageUi : byte
{
	ZSHBuildbord,
	ZSHSkillsboard,
	ZSHSurvivallose,
	ZSHSurvivalwin,
};

enum ZSHTipKill : byte
{
	ZSHKill
};

#endif