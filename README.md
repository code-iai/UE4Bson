# UE4Bson
This is a UE4 Plugin which adds basic Bson Support to your Unreal Engine.
Currently the only supported platform is Win64.

## Usage notes
The usage is somewhat similar to the native Json functionality of UE4. Once the plugin is packaged, installed and added to your dependencies you can include FBson.h and then use FBsonObject just like you would use FJsonObject. There is some example code below.


## Example code
```
// do not forget this
#include "FBson.h"

TSharedPtr<FBsonObject> MyObject = MakeShareable(new FBsonObject);
MyObject->SetNumberField("Number", 42);
MyObject->SetStringField("String", "funnyString");
MyObject->SetBoolField("Bool", true);

UE_LOG(LogTemp, Log, TEXT("MyObject formatted as json string: %s"), *(MyObject->GetJsonFormattedString()));

TArray<TSharedPtr<FBsonValue>> MyArray;
MyArray.Add(MakeShareable(new FBsonValueString("MyArrayString")));
MyArray.Add(MakeShareable(new FBsonValueNumber(1337)));
MyArray.Add(MakeShareable(new FBsonValueObject(MyObject)));

TSharedPtr<FBsonObject> MySubObject = MakeShareable(new FBsonObject);
MySubObject->SetBoolField("SubBool", false);
MySubObject->SetArrayField("SubArray", MyArray);

UE_LOG(LogTemp, Log, TEXT("MySubObject formatted as json string: %s"), *(MySubObject->GetJsonFormattedString()));

MyObject->SetObjectField("SubObject", MySubObject);

UE_LOG(LogTemp, Log, TEXT("MyObject formatted as json string ... again: %s"), *(MyObject->GetJsonFormattedString()));
```

When this code is executed you should see in your Output Log something similar to this:
(You can access the Output Log UE4 editor via Window -> Developer Tools -> Output Log)
```
LogTemp: MyObject formatted as json string: { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true }
LogTemp: MySubObject formatted as json string: { "SubBool" : false, "SubArray" : [ "MyArrayString", { "$numberDouble" : "1337.0" }, { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true } ] }
LogTemp: MyObject formatted as json string ... again: { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true, "SubObject" : { "SubBool" : false, "SubArray" : [ "MyArrayString", { "$numberDouble" : "1337.0" }, { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true } ] } }
```
