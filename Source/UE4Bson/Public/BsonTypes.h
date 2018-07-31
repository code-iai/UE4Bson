// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Error;

/**
* \brief Represents the general types a Bson Value can be.
*
* Represents the general types a Bson Value can be.
*/
enum class EBson
{
	None,
	Null,
	String,
	Number,
	Boolean,
	Array,
	Object
};