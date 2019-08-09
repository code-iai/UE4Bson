# UE4Bson
This is an **experimental** Unreal Engine 4 plugin which adds basic Bson support.
Currently the only supported platform is Win64.

## Getting it to work
Clone this Github into your UE4Project/Plugins folder. Add "UE4Bson" to your PrivateDependencyModules and rebuild the project. That should be it, you're ready to go.

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

UE_LOG(LogTemp, Log, TEXT("MyObject formatted as json string: %s"), *(MyObject->PrintAsJson()));

TArray<TSharedPtr<FBsonValue>> MyArray;
MyArray.Add(MakeShareable(new FBsonValueString("MyArrayString")));
MyArray.Add(MakeShareable(new FBsonValueNumber(1337)));
MyArray.Add(MakeShareable(new FBsonValueObject(MyObject)));

TSharedPtr<FBsonObject> MySubObject = MakeShareable(new FBsonObject);
MySubObject->SetBoolField("SubBool", false);
MySubObject->SetArrayField("SubArray", MyArray);

UE_LOG(LogTemp, Log, TEXT("MySubObject formatted as json string: %s"), *(MySubObject->PrintAsJson()));

MyObject->SetObjectField("SubObject", MySubObject);

UE_LOG(LogTemp, Log, TEXT("MyObject formatted as json string ... again: %s"), *(MyObject->PrintAsJson()));
```

When this code is executed you should see in your Output Log something similar to this:
```
LogTemp: MyObject formatted as json string: { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true }
LogTemp: MySubObject formatted as json string: { "SubBool" : false, "SubArray" : [ "MyArrayString", { "$numberDouble" : "1337.0" }, { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true } ] }
LogTemp: MyObject formatted as json string ... again: { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true, "SubObject" : { "SubBool" : false, "SubArray" : [ "MyArrayString", { "$numberDouble" : "1337.0" }, { "Number" : { "$numberDouble" : "42.0" }, "String" : "funnyString", "Bool" : true } ] } }
```
(You can access the Output Log in the UE4 editor via Window -> Developer Tools -> Output Log)
