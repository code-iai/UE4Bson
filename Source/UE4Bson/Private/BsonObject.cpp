// Fill out your copyright notice in the Description page of Project Settings.

#include "BsonObject.h"
#include "UE4Bson.h"
#include <bson.h>



struct FBsonObject::LibbsonImpl {
	
	bson_t *bsonDoc = new bson_t;

	LibbsonImpl() {
		bson_init(bsonDoc);
	}

	void SetBsonDoc(bson_t *initDoc) {
		bson_steal(bsonDoc, initDoc);
	}

	~LibbsonImpl() {
		bson_destroy(bsonDoc);
		delete bsonDoc;
	}

	void BsonFromFBsonValueArray(const TArray<TSharedPtr<FBsonValue> > &inArray, TSharedPtr<bson_t> &outArray) {
		int arrayIndex = 0;
		FString indexAsFString;
		TSharedPtr<bson_t> subArray;
		for (auto Value : inArray) {
			indexAsFString = FString::FromInt(arrayIndex);
			switch (Value->Type) {
			case EBson::Array:
				subArray = MakeShareable(new bson_t);
				bson_init(subArray.Get());
				BsonFromFBsonValueArray(Value->AsArray(), subArray);
				BSON_APPEND_ARRAY(outArray.Get(), TCHAR_TO_UTF8(*indexAsFString), subArray.Get());
				break;
			case EBson::Boolean:
				BSON_APPEND_BOOL(outArray.Get(), TCHAR_TO_UTF8(*indexAsFString), Value->AsBool());
				break;
			case EBson::Number:
				BSON_APPEND_DOUBLE(outArray.Get(), TCHAR_TO_UTF8(*indexAsFString), Value->AsNumber());
				break;
			case EBson::Object:
				BSON_APPEND_DOCUMENT(outArray.Get(), TCHAR_TO_UTF8(*indexAsFString), Value->AsObject()->Impl->bsonDoc);
				break;
			case EBson::String:
				BSON_APPEND_UTF8(outArray.Get(), TCHAR_TO_UTF8(*indexAsFString), TCHAR_TO_UTF8(*(Value->AsString())));
				break;
			case EBson::None:

				// TODO

				break;
			}
			arrayIndex++;
		}
	}

	TSharedPtr<FBsonValueArray> FBsonValueArrayFromBson(bson_iter_t *iter) const {
		TArray<TSharedPtr<FBsonValue>> returnArray;
		TSharedPtr<bson_iter_t> recurse;
		while (bson_iter_next(iter)) {
			switch (bson_iter_type(iter)) {
			case BSON_TYPE_ARRAY:
				recurse = MakeShareable(new bson_iter_t);
				bson_iter_recurse(iter, recurse.Get());
				returnArray.Add(FBsonValueArrayFromBson(recurse.Get()));
				break;
			case BSON_TYPE_BOOL:
				returnArray.Add(MakeShareable(new FBsonValueBoolean{ bson_iter_as_bool(iter) }));
				break;
			case BSON_TYPE_DOUBLE:
				returnArray.Add(MakeShareable(new FBsonValueNumber{ bson_iter_as_double(iter) }));
				break;
			case BSON_TYPE_UTF8:
				returnArray.Add(MakeShareable(new FBsonValueString{ bson_iter_utf8(iter, NULL) }));
				break;
			case BSON_TYPE_DOCUMENT:
			{
				uint32_t *length = new uint32_t;
				const void *data = nullptr;

				bson_iter_document(iter, length, (const uint8_t**)&data);
				size_t size = *length;
				TSharedPtr<FBsonObject> NewObject = MakeShareable(new FBsonObject);
				NewObject->Impl->SetBsonDoc(bson_new_from_data((uint8_t*)data, size));
				
				returnArray.Add(MakeShareable(new FBsonValueObject{ NewObject }));
				delete length;

				break;
			}
			}
		}

		return MakeShareable(new FBsonValueArray{ returnArray });
	}
	

	/**
	* Returns the appropriate EBson for a given bson_type_t.
	*
	* @param fieldType The bson_type_t to convert.
	* @return EBson the converted EBson
	*/
	EBson FindEquivalentFieldType(bson_type_t fieldType) const {
		switch (fieldType) {
		case BSON_TYPE_ARRAY:
			return EBson::Array;
		case BSON_TYPE_BOOL:
			return EBson::Boolean;
		case BSON_TYPE_DOUBLE:
			return EBson::Number;
		case BSON_TYPE_UTF8:
			return EBson::String;
		case BSON_TYPE_DOCUMENT:
			return EBson::Object;
		default:
			return EBson::None;
		}
	}
	/**
	* Returns the appropriate bson_type_t for a given EBson.
	*
	* @param fieldType The EBson to convert.
	* @return bson_type_t the converted bson_type_t
	*/
	bson_type_t FindEquivalentFieldType(EBson fieldType) const {
		switch (fieldType) {
		case EBson::Array:
			return BSON_TYPE_ARRAY;
		case EBson::Boolean:
			return BSON_TYPE_BOOL;
		case EBson::Number:
			return BSON_TYPE_DOUBLE;
		case EBson::String:
			return BSON_TYPE_UTF8;
		case EBson::Object:
			return BSON_TYPE_DOCUMENT;
		default:
			return BSON_TYPE_UNDEFINED;
		}
	}

};


FBsonObject::FBsonObject()
{
	Impl = new LibbsonImpl;
	
}

FBsonObject::~FBsonObject() 
{
	delete Impl;
}

const uint8_t* FBsonObject::GetDataPointer() const {
	return bson_get_data(Impl->bsonDoc);
}

bool FBsonObject::Compare(const TSharedPtr<FBsonObject> &ToCompare) const {
	if (bson_compare(Impl->bsonDoc, ToCompare->Impl->bsonDoc) == 0)
		return true;
	return false;
}

FString FBsonObject::GetJsonFormattedString() const {
	return bson_as_canonical_extended_json(Impl->bsonDoc, NULL);
}

/**
* Returns a field with the given fieldname(key)
*
* @param FieldName The name of the field to check.
* @return TSharedPtr with an appropriate FBsonValue.
*/
TSharedPtr<FBsonValue> FBsonObject::GetField(const FString &FieldName) const
{
	bson_iter_t iter;
	if (bson_iter_init_find(&iter, Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName))) {

		// if the requested field is of type BSON_TYPE_ARRAY this will be necessary to recurse into it


		switch (bson_iter_type(&iter)) {
		case BSON_TYPE_ARRAY:
		{
			bson_iter_t arrayIter;
			bson_iter_recurse(&iter, &arrayIter);
			return Impl->FBsonValueArrayFromBson(&arrayIter);
		}
		case BSON_TYPE_BOOL:
			return MakeShareable(new FBsonValueBoolean{ bson_iter_as_bool(&iter) });
		case BSON_TYPE_DOUBLE:
			return MakeShareable(new FBsonValueNumber{ bson_iter_as_double(&iter) });
		case BSON_TYPE_UTF8:
			return MakeShareable(new FBsonValueString{ bson_iter_utf8(&iter, NULL) });
		case BSON_TYPE_DOCUMENT:
		{
			uint32_t *length = new uint32_t;
			void *data = nullptr;
			bson_iter_document(&iter, length, (const uint8_t**)&data);
			size_t size = *length;
			UE_LOG(LogBson, Log, TEXT("Allocated region size: %d"), size);
			TSharedPtr<FBsonObject> NewObject = MakeShareable(new FBsonObject);
			NewObject->Impl->SetBsonDoc(bson_new_from_data((uint8_t*)data, size));
			TSharedPtr<FBsonValueObject> ReturnObject = MakeShareable(new FBsonValueObject{NewObject});
			delete length;
			return ReturnObject;
		}
		default:
			UE_LOG(LogBson, Warning, TEXT("Unsupported Type: %d (see http://mongoc.org/libbson/current/bson_type_t.html for reference)."), bson_iter_type(&iter))
				return MakeShareable(new FBsonValueNull());
		}
	}
	UE_LOG(LogBson, Warning, TEXT("Field %s was not found."), *FieldName);

	return MakeShareable(new FBsonValueNull());
}

/**
* Returns a field with the given fieldname(key), or nullptr if the field doesn't exist.
*
* @param FieldName The name of the field to check.
* @return TSharedPtr with an appropriate FBsonValue or nullptr
*/
TSharedPtr<FBsonValue> FBsonObject::TryGetField(const FString& FieldName) const {
	TSharedPtr<FBsonValue> checkExisting = GetField(FieldName);
	if (&checkExisting != nullptr && checkExisting.IsValid() && checkExisting->Type == EBson::Null) {
		return nullptr;
	}
	return checkExisting;
}

bool FBsonObject::HasField(const FString& FieldName) const {
	return bson_has_field(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName));
}


void FBsonObject::SetField(const FString &FieldName, const TSharedPtr<FBsonValue> &Value)
{
	switch (Value->Type) {
	case EBson::Array:
		SetArrayField(FieldName, Value->AsArray());
		break;
	case EBson::Boolean:
		SetBoolField(FieldName, Value->AsBool());
		break;
	case EBson::Number:
		SetNumberField(FieldName, Value->AsNumber());
		break;
	case EBson::Object:
		SetObjectField(FieldName, Value->AsObject());
		break;
	case EBson::String:
		SetStringField(FieldName, Value->AsString());
		break;
	case EBson::None:

		// TODO

		break;
	default:
		UE_LOG(LogBson, Warning, TEXT("Can not set field of unknown type."))
	}
}

void FBsonObject::SetNumberField(const FString &FieldName, double Number) {
	BSON_APPEND_DOUBLE(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName), Number);
}

void FBsonObject::SetBoolField(const FString &FieldName, bool Value) {
	BSON_APPEND_BOOL(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName), Value);
}

void FBsonObject::SetStringField(const FString &FieldName, const FString &StringValue) {
	BSON_APPEND_UTF8(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName), TCHAR_TO_UTF8(*StringValue));
}

void FBsonObject::SetArrayField(const FString &FieldName, const TArray< TSharedPtr<FBsonValue> > &Array) {
	TSharedPtr<bson_t> tmpBson = MakeShareable(new bson_t);
	bson_init(tmpBson.Get());
	Impl->BsonFromFBsonValueArray(Array, tmpBson);
	BSON_APPEND_ARRAY(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName), tmpBson.Get());
	bson_destroy(tmpBson.Get());
}

void FBsonObject::SetObjectField(const FString &FieldName, const TSharedPtr<FBsonObject> &Object) {
	BSON_APPEND_DOCUMENT(Impl->bsonDoc, TCHAR_TO_UTF8(*FieldName), Object->Impl->bsonDoc);
}

double FBsonObject::GetNumberField(const FString& FieldName) const
{
	return GetField(FieldName)->AsNumber();
}

bool FBsonObject::TryGetNumberField(const FString& FieldName, double& OutNumber) const
{
	TSharedPtr<FBsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetNumber(OutNumber);
}

FString FBsonObject::GetStringField(const FString& FieldName) const
{
	return GetField(FieldName)->AsString();
}

bool FBsonObject::TryGetStringField(const FString& FieldName, FString& OutString) const
{
	TSharedPtr<FBsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetString(OutString);
}

bool FBsonObject::TryGetStringArrayField(const FString& FieldName, TArray<FString>& OutArray) const
{
	TSharedPtr<FBsonValue> Field = TryGetField(FieldName);

	if (!Field.IsValid())
	{
		return false;
	}

	const TArray< TSharedPtr<FBsonValue> > *Array;

	if (!Field->TryGetArray(Array))
	{
		return false;
	}

	for (int Idx = 0; Idx < Array->Num(); Idx++)
	{
		FString Element;

		if (!(*Array)[Idx]->TryGetString(Element))
		{
			return false;
		}

		OutArray.Add(Element);
	}

	return true;
}

bool FBsonObject::GetBoolField(const FString& FieldName) const
{
	return GetField(FieldName)->AsBool();
}

bool FBsonObject::TryGetBoolField(const FString& FieldName, bool& OutBool) const
{
	TSharedPtr<FBsonValue> Field = TryGetField(FieldName);
	return Field.IsValid() && Field->TryGetBool(OutBool);
}

const TArray<TSharedPtr<FBsonValue>> FBsonObject::GetArrayField(const FString& FieldName) const
{
	return GetField(FieldName)->AsArray();
}

bool FBsonObject::TryGetArrayField(const FString& FieldName, TArray<TSharedPtr<FBsonValue>>& OutArray) const
{
	TSharedPtr<FBsonValue> Field = TryGetField(FieldName);
	const TArray<TSharedPtr<FBsonValue>> *copyArray;
	if (Field.IsValid() && Field->TryGetArray(copyArray)) {
		OutArray = *copyArray;
		return true;
	}
	return false;
}

const TSharedPtr<FBsonObject> FBsonObject::GetObjectField(const FString& FieldName) const {
	return GetField(FieldName)->AsObject();
}

bool FBsonObject::RemoveField(const FString& FieldName) {
	if (HasField(FieldName)) {
		TSharedPtr<bson_t> newDoc{ MakeShareable(new bson_t) };
		bson_init(newDoc.Get());
		bson_copy_to_excluding_noinit(Impl->bsonDoc, newDoc.Get(), TCHAR_TO_UTF8(*FieldName), NULL);
		bson_copy_to(newDoc.Get(), Impl->bsonDoc);
		bson_destroy(newDoc.Get());
		return true;
	}
	return false;
}