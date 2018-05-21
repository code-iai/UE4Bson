// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BsonValue.h"


/**
* This class' main purpose is to contain a hidden bson_t document.
* It can be edited and read (mostly) in known FJsonObject manner.
*/
class UE4BSON_API FBsonObject
{
private:
	
	struct LibbsonImpl;

	LibbsonImpl *Impl;

public:

	/**
	* @return a pointer to the memory region where the bson formatted data is located.
	*/
	const uint8_t* GetDataPointer() const;

	/**
	* @return the FBsonObject as a Json formatted String.
	*/
	FString GetJsonFormattedString() const;

	/**
	* Compares the contents of an FBsonObject to a given other one.
	* @return true if the contents match.
	*/
	bool Compare(const TSharedPtr<FBsonObject> &ToCompare) const;

	/**
	* @return a TSharedPtr<FBsonValue> that contains the data at the first key named 'FieldName'. If no key 'FieldName' exists, return a TSharedPtr<FBsonValueNull>.
	*/
	TSharedPtr<FBsonValue> GetField(const FString &FieldName) const; // create more options?

	/**
	* @return a TSharedPtr<FBsonValue> that contains the data at the first key named 'FieldName'. If no key 'FieldName' exists, return nullptr.
	*/
	TSharedPtr<FBsonValue> TryGetField(const FString& FieldName) const;
	bool HasField(const FString& FieldName) const;






	/*template<EBson JsonType>
	bool HasTypedField(const FString& FieldName) const
	{
		TSharedPtr<bson_iter_t> iter = MakeShareable(new bson_iter_t);
		if (bson_iter_init_find(iter.Get(), bsonDoc, TCHAR_TO_UTF8(*FieldName))) {
			if (FindEquivalentFieldType(bson_iter_type(iter.Get())) == JsonType)
			{
				return true;
			}
		}
		return false;
	}*/






	/**
	* Gets the field with the specified name as a number.
	*
	* Ensures that the field is present and is of type Json number.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a number.
	*/
	double GetNumberField(const FString& FieldName) const;

	/**
	* Gets a numeric field and casts to an int32
	*/
	FORCEINLINE int32 GetIntegerField(const FString& FieldName) const
	{
		return (int32)GetNumberField(FieldName);
	}

	/** Get the field named FieldName as a number. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetNumberField(const FString& FieldName, double& OutNumber) const;

	/** Get the field named FieldName as a string. */
	FString GetStringField(const FString& FieldName) const;

	/** Get the field named FieldName as a string. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetStringField(const FString& FieldName, FString& OutString) const;

	/** Get the field named FieldName as an array of strings. Returns false if it doesn't exist or any member cannot be converted. */
	bool TryGetStringArrayField(const FString& FieldName, TArray<FString>& OutArray) const;

	/**
	* Gets the field with the specified name as a boolean.
	*
	* Ensures that the field is present and is of type Json number.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a boolean.
	*/
	bool GetBoolField(const FString& FieldName) const;

	/** Get the field named FieldName as a string. Returns false if it doesn't exist or cannot be converted. */
	bool TryGetBoolField(const FString& FieldName, bool& OutBool) const;

	/** Get the field named FieldName as an array. Less efficient than FJsonObject::GetArrayField, the TArray needs to be copied once. */
	const TArray<TSharedPtr<FBsonValue>> GetArrayField(const FString& FieldName) const;

	/** Try to get the field named FieldName as an array, or return false if it's another type. Less efficient than FJsonObject::TryGetArrayField, the TArray needs to be copied once.*/
	bool TryGetArrayField(const FString& FieldName, TArray<TSharedPtr<FBsonValue>>& OutArray) const;

	const TSharedPtr<FBsonObject> GetObjectField(const FString& FieldName) const;

	/** Adds a field, automatically determines the type of the field to set from the given FBsonValue. */
	void SetField(const FString &FieldName, const TSharedPtr<FBsonValue> &Value);

	/** Adds a double field with the given number as value. */
	void SetNumberField(const FString &FieldName, double Number);

	/** Adds a string field with the given Fstring as value. */
	void SetStringField(const FString &FieldName, const FString &StringValue);

	/** Adds a bool field with the given boolean as value. */
	void SetBoolField(const FString &FieldName, bool value);

	/** Adds an array field with the given array as value. */
	void SetArrayField(const FString &FieldName, const TArray<TSharedPtr<FBsonValue> > &Array);

	/** Adds an object field with the bsonDoc contained within the given FBsonObject as value. */
	void SetObjectField(const FString &FieldName, const TSharedPtr<FBsonObject> &Object);

	/** WARNING: VERY INEFFICIENT!
	* Removes the given field.
	*/
	bool RemoveField(const FString& FieldName);



	FBsonObject();
	~FBsonObject();
};
