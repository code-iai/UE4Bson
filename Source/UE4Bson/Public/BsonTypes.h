// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class Error;

/**
 * Represents the most important types a Bson Value can be.
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