// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BsonTypes.h"
#include "Json.h"

class FBsonObject;

/**
* \brief A parent class for different types of Bson values.
*
* A base class for easy usage and comparison of Bson values. Only it's derived classes should actually be used.
*/
class UE4BSON_API FBsonValue
{
public:


	/** 
	* Returns this value as a double, logging an error and returning zero if not possible. 
	*
	* @return this value as a double or zero if it cant be converted.
	*/
	double AsNumber() const;

	/**
	* Returns this value as a string, logging an error and returning an empty string if not possible.
	*
	* @return this value as a string or an empty string if it can't be converted.
	*/
	FString AsString() const;

	/**
	* Returns this value as a boolean, logging an error and returning false if not possible.
	*
	* @return this value as a boolean or false if it can't be converted.
	*/
	bool AsBool() const;

	/**
	* Returns this value as a TArray, logging an error and returning an empty TArray if not possible.
	*
	* @return this value as a TArray or an empty TArray if it can't be converted.
	*/
	const TArray<TSharedPtr<FBsonValue>>& AsArray() const;

	/**
	* Returns this value as an FBsonObject, throwing an error if this is not possible.
	*
	* @return this value as an FBsonObject.
	*/
	virtual const TSharedPtr<FBsonObject>& AsObject() const;

	/** 
	* Tries to convert this value to a double, returning false if not possible. 
	* 
	* @param OutDouble a reference to write the converted number into.
	* @return false if value can't be converted, true otherwise.
	*/
	virtual bool TryGetNumber(double& OutDouble) const { return false; }

	/** 
	* Tries to convert this value to an int32, returning false if not possible.
	*
	* @param OutNumber a reference to write the converted number into.
	* @return false if value can't be converted, true otherwise.
	*/
	bool TryGetNumber(int32& OutNumber) const;

	/**
	* Tries to convert this value to a uint32, returning false if not possible.
	*
	* @param OutNumber a reference to write the converted number into.
	* @return false if value can't be converted, true otherwise.
	*/
	bool TryGetNumber(uint32& OutNumber) const;

	/**
	* Tries to convert this value to an int64, returning false if not possible.
	*
	* @param OutNumber a reference to write the converted number into.
	* @return false if value can't be converted, true otherwise.
	*/
	bool TryGetNumber(int64& OutNumber) const;

	/**
	* Tries to convert this value to an FString, returning false if not possible.
	*
	* @param OutString a reference to write the converted FString into.
	* @return false if value can't be converted, true otherwise.
	*/
	virtual bool TryGetString(FString& OutString) const { return false; }

	/**
	* Tries to convert this value to an boolean, returning false if not possible.
	*
	* @param OutBool a reference to write the converted boolean into.
	* @return false if value can't be converted, true otherwise.
	*/
	virtual bool TryGetBool(bool& OutBool) const { return false; }

	/**
	* Tries to convert this value to a TArray of TSharedPtr<FBsonValue>, returning false if not possible.
	*
	* @param OutArray a reference to write the converted TArray into.
	* @return false if value can't be converted, true otherwise.
	*/	
	virtual bool TryGetArray(const TArray<TSharedPtr<FBsonValue>>*& OutArray) const { return false; }

	/**
	* Tries to convert this value to an FBsonObject, returning false if not possible.
	*
	* @param Object a pointer to a reference to write the converted Object into.
	* @return false if value can't be converted, true otherwise.
	*/
	virtual bool TryGetObject(const TSharedPtr<FBsonObject>*& Object) const { return false; }

	/** @return true if this value is 'null'. */
	bool IsNull() const { return Type == EBson::Null || Type == EBson::None; }

	/** 
	* Get a field of the same type as the argument.
	* Mind that this uses the AsType functions which will return specified values if conversion isn't possible.
	*
	* @param Value a reference of the desired type to put the converted value into.
	*/
	void AsArgumentType(double                          & Value) { Value = AsNumber(); }
	void AsArgumentType(FString                         & Value) { Value = AsString(); }
	void AsArgumentType(bool                            & Value) { Value = AsBool(); }
	void AsArgumentType(TArray<TSharedPtr<FBsonValue>>& Value) { Value = AsArray(); }
	void AsArgumentType(TSharedPtr<FBsonObject>         & Value) { Value = AsObject(); }

	EBson Type;

	/**
	* Checks whether to FBsonValues are equal.
	* 
	* @param Lhs first value to compare.
	* @param Rhs second value to compare.
	*/
	static bool CompareEqual(const FBsonValue& Lhs, const FBsonValue& Rhs);

	/**
	* Overloaded == operator that uses the CompareEqual() method for two FBsonValues.
	*/
	bool operator==(const FBsonValue& Rhs) const { return CompareEqual(*this, Rhs); }

protected:

	static const TArray<TSharedPtr<FBsonValue>> EMPTY_ARRAY;
	static const TSharedPtr<FBsonObject> EMPTY_OBJECT;

	FBsonValue() : Type(EBson::None) {}
	virtual ~FBsonValue() {}

	virtual FString GetType() const = 0;

	void ErrorMessage(const FString& InType) const;

};


/** \brief A Bson String Value. */
class UE4BSON_API FBsonValueString : public FBsonValue
{
public:
	FBsonValueString(const FString& InString) : Value(InString) { Type = EBson::String; }

	virtual bool TryGetString(FString& OutString) const override { OutString = Value; return true; }
	virtual bool TryGetNumber(double& OutDouble) const override { if (Value.IsNumeric()) { OutDouble = FCString::Atod(*Value); return true; } else { return false; } }
	virtual bool TryGetBool(bool& OutBool) const override { OutBool = Value.ToBool(); return true; }

protected:
	FString Value;

	virtual FString GetType() const override { return TEXT("String"); }
};

/** \brief A Bson Number Value. */
class UE4BSON_API FBsonValueNumber : public FBsonValue
{
public:
	FBsonValueNumber(double InNumber) : Value(InNumber) { Type = EBson::Number; }
	virtual bool TryGetNumber(double& OutNumber) const override { OutNumber = Value; return true; }
	virtual bool TryGetBool(bool& OutBool) const override { OutBool = (Value != 0.0); return true; }
	virtual bool TryGetString(FString& OutString) const override { OutString = FString::SanitizeFloat(Value, 0); return true; }

protected:
	double Value;

	virtual FString GetType() const override { return TEXT("Number"); }
};

/** \brief A Bson Boolean Value. */
class UE4BSON_API FBsonValueBoolean : public FBsonValue
{
public:
	FBsonValueBoolean(bool InBool) : Value(InBool) { Type = EBson::Boolean; }
	virtual bool TryGetNumber(double& OutNumber) const override { OutNumber = Value ? 1 : 0; return true; }
	virtual bool TryGetBool(bool& OutBool) const override { OutBool = Value; return true; }
	virtual bool TryGetString(FString& OutString) const override { OutString = Value ? TEXT("true") : TEXT("false"); return true; }

protected:
	bool Value;

	virtual FString GetType() const override { return TEXT("Boolean"); }
};

/** \brief A Json Array Value. */
class UE4BSON_API FBsonValueArray : public FBsonValue
{
public:
	FBsonValueArray(const TArray<TSharedPtr<FBsonValue>> & InArray) : Value(InArray) { Type = EBson::Array; }

	virtual bool TryGetArray(const TArray<TSharedPtr<FBsonValue>>*& OutArray) const override { OutArray = &Value; return true; }

protected:
	TArray<TSharedPtr<FBsonValue>>Value;

	virtual FString GetType() const override { return TEXT("Array"); }
};

/** \brief A Json Object Value. */
class UE4BSON_API FBsonValueObject : public FBsonValue
{
public:
	FBsonValueObject(TSharedPtr<FBsonObject> InObject) : Value(InObject) { Type = EBson::Object; }
	virtual bool TryGetObject(const TSharedPtr<FBsonObject>*& OutObject) const override { OutObject = &Value; return true; }
protected:
	TSharedPtr<FBsonObject> Value;

	virtual FString GetType() const override { return TEXT("Object"); }
};

/** \brief A Bson Null Value. */
class UE4BSON_API FBsonValueNull : public FBsonValue
{
public:
	FBsonValueNull() { Type = EBson::Null; }

protected:
	virtual FString GetType() const override { return TEXT("Null"); }
};
