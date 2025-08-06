// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpartaProject/Public/BlindItem.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBlindItem() {}

// Begin Cross Module References
SPARTAPROJECT_API UClass* Z_Construct_UClass_ABaseItem();
SPARTAPROJECT_API UClass* Z_Construct_UClass_ABlindItem();
SPARTAPROJECT_API UClass* Z_Construct_UClass_ABlindItem_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpartaProject();
// End Cross Module References

// Begin Class ABlindItem
void ABlindItem::StaticRegisterNativesABlindItem()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ABlindItem);
UClass* Z_Construct_UClass_ABlindItem_NoRegister()
{
	return ABlindItem::StaticClass();
}
struct Z_Construct_UClass_ABlindItem_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "BlindItem.h" },
		{ "ModuleRelativePath", "Public/BlindItem.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ABlindItem>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ABlindItem_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ABaseItem,
	(UObject* (*)())Z_Construct_UPackage__Script_SpartaProject,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ABlindItem_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ABlindItem_Statics::ClassParams = {
	&ABlindItem::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ABlindItem_Statics::Class_MetaDataParams), Z_Construct_UClass_ABlindItem_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ABlindItem()
{
	if (!Z_Registration_Info_UClass_ABlindItem.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ABlindItem.OuterSingleton, Z_Construct_UClass_ABlindItem_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ABlindItem.OuterSingleton;
}
template<> SPARTAPROJECT_API UClass* StaticClass<ABlindItem>()
{
	return ABlindItem::StaticClass();
}
ABlindItem::ABlindItem() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(ABlindItem);
ABlindItem::~ABlindItem() {}
// End Class ABlindItem

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_USER_Documents_Unreal_Projects_SpartaProject_Source_SpartaProject_Public_BlindItem_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ABlindItem, ABlindItem::StaticClass, TEXT("ABlindItem"), &Z_Registration_Info_UClass_ABlindItem, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ABlindItem), 921784531U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_USER_Documents_Unreal_Projects_SpartaProject_Source_SpartaProject_Public_BlindItem_h_4094658589(TEXT("/Script/SpartaProject"),
	Z_CompiledInDeferFile_FID_Users_USER_Documents_Unreal_Projects_SpartaProject_Source_SpartaProject_Public_BlindItem_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_USER_Documents_Unreal_Projects_SpartaProject_Source_SpartaProject_Public_BlindItem_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
