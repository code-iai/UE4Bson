// Fill out your copyright notice in the Description page of Project Settings.

#include "BsonValue.h"
#include "UE4Bson.h"
#include "BsonObject.h"

const TArray<TSharedPtr<FBsonValue>> FBsonValue::EMPTY_ARRAY;
const TSharedPtr<FBsonObject> FBsonValue::EMPTY_OBJECT(new FBsonObject());

double FBsonValue::AsNumber() const
{
	double Number = 0.0;

	if (!TryGetNumber(Number))
	{
		ErrorMessage(TEXT("Number"));
	}

	return Number;
}


FString FBsonValue::AsString() const
{
	FString String;

	if (!TryGetString(String))
	{
		ErrorMessage(TEXT("String"));
	}

	return String;
}


bool FBsonValue::AsBool() const
{
	bool Bool = false;

	if (!TryGetBool(Bool))
	{
		ErrorMessage(TEXT("Boolean"));
	}

	return Bool;
}


const TArray<TSharedPtr<FBsonValue>>& FBsonValue::AsArray() const
{
	const TArray<TSharedPtr<FBsonValue>> *Array = &EMPTY_ARRAY;

	if (!TryGetArray(Array))
	{
		ErrorMessage(TEXT("Array"));
	}

	return *Array;
}


const TSharedPtr<FBsonObject>& FBsonValue::AsObject() const
{
	const TSharedPtr<FBsonObject> *Object = &EMPTY_OBJECT;

	if (!TryGetObject(Object))
	{
		ErrorMessage(TEXT("Object"));
	}

	return *Object;
}


bool FBsonValue::TryGetNumber(int32& OutNumber) const
{
	double Double;

	if (TryGetNumber(Double) && (Double >= INT_MIN) && (Double <= INT_MAX))
	{
		if (Double >= 0.0)
		{
			OutNumber = (int32)(Double + 0.5);
		}
		else
		{
			OutNumber = (int32)(Double - 0.5);
		}

		return true;
	}

	return false;
}


bool FBsonValue::TryGetNumber(uint32& OutNumber) const
{
	double Double;

	if (TryGetNumber(Double) && (Double >= 0.0) && (Double <= UINT_MAX))
	{
		OutNumber = (uint32)(Double + 0.5);

		return true;
	}

	return false;
}


bool FBsonValue::TryGetNumber(int64& OutNumber) const
{
	double Double;

	if (TryGetNumber(Double) && (Double >= INT64_MIN) && (Double <= INT64_MAX))
	{
		if (Double >= 0.0)
		{
			OutNumber = (int64)(Double + 0.5);
		}
		else
		{
			OutNumber = (int64)(Double - 0.5);
		}

		return true;
	}

	return false;
}


//static 
bool FBsonValue::CompareEqual(const FBsonValue& Lhs, const FBsonValue& Rhs)
{
	if (Lhs.Type != Rhs.Type)
	{
		return false;
	}

	switch (Lhs.Type)
	{
	case EBson::None:
	case EBson::Null:
		return true;

	case EBson::String:
		return Lhs.AsString() == Rhs.AsString();

	case EBson::Number:
		return Lhs.AsNumber() == Rhs.AsNumber();

	case EBson::Boolean:
		return Lhs.AsBool() == Rhs.AsBool();

	case EBson::Array:
	{
		const TArray<TSharedPtr<FBsonValue>>& LhsArray = Lhs.AsArray();
		const TArray<TSharedPtr<FBsonValue>>& RhsArray = Rhs.AsArray();

		if (LhsArray.Num() != RhsArray.Num())
		{
			return false;
		}

		// compare each element
		for (int32 i = 0; i < LhsArray.Num(); ++i)
		{
			if (!CompareEqual(*LhsArray[i], *RhsArray[i]))
			{
				return false;
			}
		}
	}
	return true;

	case EBson::Object:
	{
		const TSharedPtr<FBsonObject>& LhsObject = Lhs.AsObject();
		const TSharedPtr<FBsonObject>& RhsObject = Rhs.AsObject();

		if (LhsObject.IsValid() != RhsObject.IsValid())
		{
			return false;
		}

		if (LhsObject.IsValid())
		{
			return LhsObject->Compare(RhsObject);
		}

		return false;
	}

	default:
		return false;
	}
}

void FBsonValue::ErrorMessage(const FString& InType) const
{
	UE_LOG(LogBson, Error, TEXT("Bson Value of type '%s' used as a '%s'."), *GetType(), *InType);
}