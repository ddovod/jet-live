
cmake_minimum_required(VERSION 3.6)
project(tests)

add_library(undefined_symbols "")
target_sources(undefined_symbols
  PRIVATE
  src/utility/UndefinedSymbol2.cpp
  src/utility/UndefinedSymbolDeep/UndefinedSymbolDeep1.cpp
  src/utility/UndefinedSymbolDeep/UndefinedSymbolDeep2.cpp
#  src/utility/MultipleDefinitions/MultipleDefinitions1.cpp # <jet_tag: mul_def:2>
  src/utility/MultipleDefinitions/MultipleDefinitions2.cpp
  )
target_include_directories(undefined_symbols
  PRIVATE
  src
  )
set_target_properties(undefined_symbols
  PROPERTIES
  CXX_STANDARD 11
  )

add_executable(tests "")
target_sources(tests
  PRIVATE
  src/main.cpp
  src/TestListener.cpp
  src/WaitForReload.cpp

  src/utility/LambdaFunctionWithCapturesBadCase2.cpp
  src/utility/LambdaFunctionWithCapturesGoodCase.cpp
  src/utility/LambdaFunctionWithCaptures.cpp
  src/utility/SeveralReloads.cpp
  src/utility/UndefinedSymbol.cpp
  src/utility/GlobalVariable.cpp
  src/utility/StaticVariableSameName1.cpp
  src/utility/ClassVirtualMethod.cpp
  src/utility/GlobalFreeFunction.cpp
  src/utility/ExternGlobalVariable.cpp
  src/utility/StaticInternalVariableAddress.cpp
  src/utility/LambdaFunction.cpp
  src/utility/InternalLinkageFunction.cpp
  src/utility/StaticVariableAddress.cpp
  src/utility/LambdaFunctionWithCapturesBadCase.cpp
  src/utility/ClassInstanceMethod.cpp
  src/utility/ClassStaticMethod.cpp
  src/utility/StaticFunctionLocalVariableAddress.cpp
  src/utility/StaticInternalVariable.cpp
  src/utility/GlobalVariableAddress.cpp
  src/utility/StaticVariable.cpp
  src/utility/StaticVariableSameName2.cpp
  src/utility/StaticFunctionLocalVariable.cpp
  src/utility/ExternGlobalVariableAddress.cpp
  src/utility/CommonSection.c
  src/utility/ReloadOnSignal.cpp
  src/utility/ModifyFileDuringReload.cpp
  src/utility/NewCompilationUnit.cpp
#  src/utility/NewCompilationUnit/NewCompilationUnit2.cpp # <jet_tag: new_cu:2>
  src/utility/NewHeaderDependency.cpp
  src/utility/UndefinedSymbolDeep.cpp
  src/utility/MultipleDefinitions.cpp
  src/utility/FailedCompilationCrash.cpp
  src/utility/LostModification.cpp
#  src/utility/LostModification2.cpp # <jet_tag: lost_mod:3>
  src/utility/ReloadAfterFailedCompilation1.cpp
  src/utility/ReloadAfterFailedCompilation2.cpp

  src/good/ClassInstanceMethod_test.cpp
  src/good/CommonSection_test.cpp
  src/good/StaticInternalVariable_test.cpp
  src/good/GlobalFreeFunction_test.cpp
  src/good/GlobalVariableAddress_test.cpp
  src/good/StaticVariableAddress_test.cpp
  src/good/StaticVariable_test.cpp
  src/good/SeveralReloads_test.cpp
  src/good/ExternGlobalVariableAddress_test.cpp
  src/good/InternalLinkageFunction_test.cpp
  src/good/StaticVariableSameName_test.cpp
  src/good/StaticInternalVariableAddress_test.cpp
  src/good/ClassVirtualMethod_test.cpp
  src/good/StaticFunctionLocalVariableAddress_test.cpp
  src/good/UndefinedSymbol_test.cpp
  src/good/LambdaFunctionWithCapturesGoodCase_test.cpp
  src/good/GlobalVariable_test.cpp
  src/good/LambdaFunctionWithCaptures_test.cpp
  src/good/LambdaFunction_test.cpp
  src/good/ClassStaticMethod_test.cpp
  src/good/ExternGlobalVariable_test.cpp
  src/good/StaticFunctionLocalVariable_test.cpp
  src/good/ReloadOnSignal_test.cpp
  src/good/ModifyFileDuringReload_test.cpp
  src/good/NewCompilationUnit_test.cpp
  src/good/NewHeaderDependency_test.cpp
  src/good/UndefinedSymbolDeep_test.cpp
  src/good/MultipleDefinitions_test.cpp
  src/good/FailedCompilationCrash_test.cpp
  src/good/LostModification_test.cpp
  src/good/ReloadAfterFailedCompilation_test.cpp

  src/bad/LambdaFunctionWithCapturesBadCase2_test.cpp
  src/bad/LambdaFunctionWithCapturesBadCase_test.cpp
  )
target_include_directories(tests
  PRIVATE
  src
  ../libs/Catch2
  )
target_link_libraries(tests
  PRIVATE
  jet-live
  undefined_symbols
  )
set_target_properties(tests
  PROPERTIES
  CXX_STANDARD 11
  )
target_compile_options(tests
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )

