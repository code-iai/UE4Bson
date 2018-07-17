// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BsonValue.h"
#include "Json.h"


/**
* \brief Holds and manages a Bson formatted data stream.
*
* This classes main purpose is to contain a hidden bson_t document.
* It can be edited and read (mostly) in known FJsonObject manner.
*/
class UE4BSON_API FBsonObject
{
private:
	
	struct LibbsonImpl;

	LibbsonImpl *Impl;

public:

	/**
	* Constructor that creates an empty Bson Document.
	*/
	FBsonObject();

	/**
	* Creates a Bson Document from the provided Data, assumes the data is valid Bson format.
	*/
	FBsonObject(const uint8_t* Data, size_t Length);

	FBsonObject(FString Data);

	~FBsonObject();

	/**
	* @return a pointer to the memory region where the bson formatted data is located.
	*/
	const uint8_t* GetDataPointer() const;

	/**
	* @return the length of the data contained in the document
	*/
	const size_t GetDataLength() const;

	/**
	* @return the FBsonObject as a Json formatted String with some extra information.
	*/
	FString PrintAsCanonicalJson() const;

	/**
	* @return the FBsonObject as a simple Json formatted String.
	*/
	FString PrintAsJson() const;

	/**
	* Create a copy of this FBsonObject.
	*
	* @return the copy.
	*/
	TSharedPtr<FBsonObject> Copy() const;

	/**
	* Compares the contents of an FBsonObject to a given other one.
	* 
	* @param ToCompare a reference to the FBsonObject to match.
	* @return true if the contents match.
	*/
	bool Compare(const TSharedPtr<FBsonObject> &ToCompare) const;

	/**
	* Returns a field with the given fieldname(key).
	*
	* @param FieldName The name of the field to check.
	* @return TSharedPtr with an appropriate FBsonValue.
	*/
	TSharedPtr<FBsonValue> GetField(const FString &FieldName) const; // create more options?

	/**
	* Tries to find the specified field and returns it.
	*
	* @param FieldName The name of the field to find.
	* @return a TSharedPtr<FBsonValue> that contains the data at the first key named 'FieldName'. If no key 'FieldName' exists, return nullptr.
	*/
	TSharedPtr<FBsonValue> TryGetField(const FString& FieldName) const;
	
	/**
	* Checks whether the specified field exists.
	*
	* @param FieldName The name of the field to check for.
	* @return true if the field exists, false if not.
	*/
	bool HasField(const FString& FieldName) const;




	/**
	* Finds the field with the specified name and returns it as a double.
	*
	* Assumes that the field is present and is of numerical type.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a double.
	*/
	double GetNumberField(const FString& FieldName) const;

	/**
	* Finds the field with the specified name and returns it as an int32.
	*
	* Assumes that the field is present and is of type castable to double.
	* 
	* @param FieldName The name of the field to get.
	* @return The field's value as an int32.
	*/
	FORCEINLINE int32 GetIntegerField(const FString& FieldName) const
	{
		return (int32)GetNumberField(FieldName);
	}

	/**
	* Tries to find the field with the specified name and return it as a double.
	*
	* @param FieldName The name of the field to get.
	* @param OutNumber A reference for the output to go in.
	* @return false if FieldName doesn't exist or cannot be converted to double.
	*/
	bool TryGetNumberField(const FString& FieldName, double& OutNumber) const;

	/**
	* Finds the field with the specified name and returns it as an FString.
	*
	* Assumes that the field is present and of type string.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as an FString.
	*/
	FString GetStringField(const FString& FieldName) const;

	/**
	* Tries to find the field with the specified name and return it as a string.
	*
	* @param FieldName The name of the field to get.
	* @param OutString a reference for the output to go in.
	* @return false if FieldName doesn't exist or cannot be converted to string.
	*/
	bool TryGetStringField(const FString& FieldName, FString& OutString) const;

	/**
	* Tries to find the field with the specified name and return it as an array of FStrings.
	*
	* @param FieldName The name of the field to get.
	* @param OutNumber A reference for the output to go in.
	* @return false if FieldName doesn't exist or any of its contents cannot be converted to string.
	*/
	bool TryGetStringArrayField(const FString& FieldName, TArray<FString>& OutArray) const;

	/**
	* Finds the field with the specified name and returns it as a boolean.
	*
	* Assumes that the field is present and is of type boolean.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a boolean.
	*/
	bool GetBoolField(const FString& FieldName) const;

	/**
	* Tries to find the field with the specified name and return it as a boolean.
	*
	* @param FieldName The name of the field to get.
	* @param OutNumber A reference for the output to go in.
	* @return false if FieldName doesn't exist or cannot be converted to boolean.
	*/
	bool TryGetBoolField(const FString& FieldName, bool& OutBool) const;

	/**
	* Finds the field with the specified name and returns it as a TArray< TSharedPtr<FBsonValue> >.
	*
	* Assumes that the field is present and is of type array.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a TArray< TSharedPtr<FBsonValue> >.
	*/
	const TArray<TSharedPtr<FBsonValue>> GetArrayField(const FString& FieldName) const;

	/**
	* Tries to find the field with the specified name and return it as a TArray< TSharedPtr<FBsonValue> >.
	*
	* @param FieldName The name of the field to get.
	* @param OutNumber A reference for the output to go in.
	* @return false if FieldName doesn't exist or cannot be converted to TArray< TSharedPtr<FBsonValue> >.
	*/
	bool TryGetArrayField(const FString& FieldName, TArray<TSharedPtr<FBsonValue>>& OutArray) const;

	/**
	* Finds the field with the specified name and returns it as a TSharedPtr<FBsonObject>.
	*
	* Assumes that the field is present and is of type object.
	*
	* @param FieldName The name of the field to get.
	* @return The field's value as a TSharedPtr<FBsonObject>.
	*/
	const TSharedPtr<FBsonObject> GetObjectField(const FString& FieldName) const;

	/** 
	* Adds a field, automatically determines the type of the field to set from the given FBsonValue. 
	* 
	* @param FieldName The name to be given to the field.
	* @param Value A TSharedPtr<FBsonValue> containing the desired value of the field.
	*/
	void SetField(const FString &FieldName, const TSharedPtr<FBsonValue> &Value);

	/**
	* Adds a field of type double (number).
	*
	* @param FieldName The name to be given to the field (key).
	* @param Number The desired value of the field (value).
	*/
	void SetNumberField(const FString &FieldName, double Number);

	/**
	* Adds a field of type string.
	*
	* @param FieldName The name to be given to the field (key).
	* @param StringValue The desired value of the field (value).
	*/
	void SetStringField(const FString &FieldName, const FString &StringValue);

	/**
	* Adds a field of type boolean.
	*
	* @param FieldName The name to be given to the field (key).
	* @param Bool The desired value of the field (value).
	*/
	void SetBoolField(const FString &FieldName, bool Bool);

	/**
	* Adds a field of type Array.
	*
	* @param FieldName The name to be given to the field (key).
	* @param Array The desired value of the field (value).
	*/
	void SetArrayField(const FString &FieldName, const TArray<TSharedPtr<FBsonValue> > &Array);

	/**
	* Adds a field of type double.
	*
	* @param FieldName The name to be given to the field (key).
	* @param Object The desired value of the field (value).
	*/
	void SetObjectField(const FString &FieldName, const TSharedPtr<FBsonObject> &Object);

	/** 
	* Removes the given field.
	*
	* WARNING: INEFFICIENT!
	*
	* @param FieldName The name of the field to remove.
	* @return true if the field was successfully removed.
	*/
	bool RemoveField(const FString& FieldName);



	
};
