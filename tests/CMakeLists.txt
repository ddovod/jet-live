
cmake_minimum_required(VERSION 3.6)
project(tests)

add_executable(tests "")
target_sources(tests
  PRIVATE
  src/ClassInstanceMethod.cpp
  src/ClassStaticMethod.cpp
  src/ClassVirtualMethod.cpp
  src/GlobalFreeFunction.cpp
  src/InternalLinkageFunction.cpp
  src/LambdaFunction.cpp
  src/LambdaFunctionWithCapturesBadCase2.cpp
  src/LambdaFunctionWithCapturesBadCase.cpp
  src/LambdaFunctionWithCaptures.cpp
  src/LambdaFunctionWithCapturesGoodCase.cpp
  src/main.cpp
  src/StaticVariable.cpp
  src/StaticInternalVariable.cpp
  src/StaticFunctionLocalVariable.cpp
  src/TestDelegate.cpp
  src/WaitForReload.cpp

  src/ClassInstanceMethod_test.cpp
  src/ClassStaticMethod_test.cpp
  src/ClassVirtualMethod_test.cpp
  src/GlobalFreeFunction_test.cpp
  src/InternalLinkageFunction_test.cpp
  src/LambdaFunction_test.cpp
  src/LambdaFunctionWithCapturesBadCase2_test.cpp
  src/LambdaFunctionWithCapturesBadCase_test.cpp
  src/LambdaFunctionWithCapturesGoodCase_test.cpp
  src/LambdaFunctionWithCaptures_test.cpp
  src/StaticVariable_test.cpp
  src/StaticInternalVariable_test.cpp
  src/StaticFunctionLocalVariable_test.cpp
  )
target_include_directories(tests
  PRIVATE
  src
  ../libs/Catch2
  )
target_link_libraries(tests
  PRIVATE
  jet-live
  )
set_target_properties(tests
  PROPERTIES
  CXX_STANDARD 11
  )
target_compile_options(tests
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )

