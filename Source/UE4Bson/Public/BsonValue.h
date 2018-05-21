// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BsonTypes.h"

class FBsonObject;

/**
*
*/
class UE4BSON_API FBsonValue
{
public:

	/** Returns this value as a double, logging an error and returning zero if this is not an Json Number */
	double AsNumber() const;

	/** Returns this value as a string, logging an error and returning an empty string if not possible */
	FString AsString() const;

	/** Returns this value as a boolean, logging an error and returning false if not possible */
	bool AsBool() const;

	/** Returns this value as an array, logging an error and returning an empty array reference if not possible */
	const TArray<TSharedPtr<FBsonValue>>& AsArray() const;

	/** Returns this value as an object, throwing an error if this is not an Json Object */
	virtual const TSharedPtr<FBsonObject>& AsObject() const;

	/** Tries to convert this value to a number, returning false if not possible */
	virtual bool TryGetNumber(double& OutDouble) const { return false; }

	/** Tries to convert this value to an integer, returning false if not possible */
	bool TryGetNumber(int32& OutNumber) const;

	/** Tries to convert this value to an integer, returning false if not possible */
	bool TryGetNumber(uint32& OutNumber) const;

	/** Tries to convert this value to an integer, returning false if not possible */
	bool TryGetNumber(int64& OutNumber) const;

	/** Tries to convert this value to a string, returning false if not possible */
	virtual bool TryGetString(FString& OutString) const { return false; }

	/** Tries to convert this value to a bool, returning false if not possible */
	virtual bool TryGetBool(bool& OutBool) const { return false; }

	/** Tries to convert this value to an array, returning false if not possible */
	virtual bool TryGetArray(const TArray<TSharedPtr<FBsonValue>>*& OutArray) const { return false; }

	/** Tries to convert this value to an object, returning false if not possible */
	virtual bool TryGetObject(const TSharedPtr<FBsonObject>*& Object) const { return false; }

	/** Returns true if this value is a 'null' */
	bool IsNull() const { return Type == EBson::Null || Type == EBson::None; }

	/** Get a field of the same type as the argument */
	void AsArgumentType(double                          & Value) { Value = AsNumber(); }
	void AsArgumentType(FString                         & Value) { Value = AsString(); }
	void AsArgumentType(bool                            & Value) { Value = AsBool(); }
	void AsArgumentType(TArray<TSharedPtr<FBsonValue>>& Value) { Value = AsArray(); }
	void AsArgumentType(TSharedPtr<FBsonObject>         & Value) { Value = AsObject(); }

	EBson Type;

	static bool CompareEqual(const FBsonValue& Lhs, const FBsonValue& Rhs);
	bool operator==(const FBsonValue& Rhs) const { return CompareEqual(*this, Rhs); }

protected:

	static const TArray<TSharedPtr<FBsonValue>> EMPTY_ARRAY;
	static const TSharedPtr<FBsonObject> EMPTY_OBJECT;

	FBsonValue() : Type(EBson::None) {}
	virtual ~FBsonValue() {}

	virtual FString GetType() const = 0;

	void ErrorMessage(const FString& InType) const;

};


/** A Bson String Value. */
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

/** A Bson Number Value. */
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

/** A Bson Boolean Value. */
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

/** A Json Array Value. */
class UE4BSON_API FBsonValueArray : public FBsonValue
{
public:
	FBsonValueArray(const TArray<TSharedPtr<FBsonValue>> & InArray) : Value(InArray) { Type = EBson::Array; }

	virtual bool TryGetArray(const TArray<TSharedPtr<FBsonValue>>*& OutArray) const override { OutArray = &Value; return true; }

protected:
	TArray<TSharedPtr<FBsonValue>>Value;

	virtual FString GetType() const override { return TEXT("Array"); }
};

/** A Json Object Value. */
class UE4BSON_API FBsonValueObject : public FBsonValue
{
public:
	FBsonValueObject(TSharedPtr<FBsonObject> InObject) : Value(InObject) { Type = EBson::Object; }
	virtual bool TryGetObject(const TSharedPtr<FBsonObject>*& OutObject) const override { OutObject = &Value; return true; }

protected:
	TSharedPtr<FBsonObject> Value;

	virtual FString GetType() const override { return TEXT("Object"); }
};

/** A Bson Null Value. */
class UE4BSON_API FBsonValueNull : public FBsonValue
{
public:
	FBsonValueNull() { Type = EBson::Null; }

protected:
	virtual FString GetType() const override { return TEXT("Null"); }
};
