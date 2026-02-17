// Fill out your copyright notice in the Description page of Project Settings.


#include "MobEnemySpirit.h"

AMobEnemySpirit::AMobEnemySpirit()
{
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	OverlapEnemyDamage = 10.0f;
	DropExperienceAmount = 10.0f;
}